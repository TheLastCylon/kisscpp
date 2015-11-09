// File  : persisted_queue.tpp
// Author: Dirk J. Botha <bothadj@gmail.com>
//
// This file is part of kisscpp library.
//
// The kisscpp library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// The kisscpp library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with the kisscpp library. If not, see <http://www.gnu.org/licenses/>.

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
PersistedQueue<_qoT, _sT>::PersistedQueue(const std::string& queueName,
                                                     const std::string& queueWorkingDir,
                                                     const unsigned     maxItemsPerPage) :
  _queueName(queueName),
  _workingDirectory(queueWorkingDir),
  _maxItemsPerPage(maxItemsPerPage)
{
  std::stringstream tmpRegex;
  tmpRegex << "^" << _queueName << "_[0-9]*";
  _queueFileRegex = tmpRegex.str();

  makeStateFilePath();

  load();
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
PersistedQueue<_qoT, _sT>::~PersistedQueue()
{
  writeFirstAndLastPage();
  writeStateFile();
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
void PersistedQueue<_qoT, _sT>::push_back(boost::shared_ptr< _qoT > p)
{
  if(lastPage->size() >= _maxItemsPerPage) {
    if(lastPage == firstPage) {
      lastPage.reset(new std::deque<boost::shared_ptr< _qoT > >());
    } else {
      persistToFile(seqNumber(), lastPage); 
      lastPage.reset(new std::deque<boost::shared_ptr< _qoT > >());
    }
  }

  lastPage->push_back(p);
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
void PersistedQueue<_qoT, _sT>::push_front(boost::shared_ptr< _qoT > p)
{
  firstPage->push_front(p);
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
boost::shared_ptr<_qoT> PersistedQueue<_qoT, _sT>::pop_front()
{
  boost::shared_ptr< _qoT >         retval;

  if(firstPage->empty()) {
    if(persistedFileNames.empty()) {
      firstPage = lastPage;
    } else {
      firstPage = loadFromFile(persistedFileNames.front());
      persistedFileNames.pop_front();
    }
  }

  if(firstPage->empty()) {
    retval.reset();
  } else {
    retval = firstPage->front();
    firstPage->pop_front();
  }

  return retval;
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
boost::shared_ptr< _qoT > PersistedQueue<_qoT, _sT>::front()
{
  boost::shared_ptr< _qoT > retval;

  if(firstPage->empty()) {
    if(persistedFileNames.empty()) {
      firstPage = lastPage;
    } else {
      firstPage = loadFromFile(persistedFileNames.front());
      persistedFileNames.pop_front();
    }
  }

  if(firstPage->empty()) {
    retval.reset();
  } else {
    retval = firstPage->front();
  }

  return retval;
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
void PersistedQueue<_qoT, _sT>::clear()
{
  while(!empty()) { pop_front(); } // this is not the best way to clear this kind of queue.
                                   // there are faster, more complicated ways.
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
bool PersistedQueue<_qoT, _sT>::empty()
{
  return (firstPage->empty() && lastPage->empty() && persistedFileNames.empty());
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
size_t PersistedQueue<_qoT, _sT>::size()
{
  return (firstPage->size() + lastPage->size()  + (persistedFileNames.size() * _maxItemsPerPage));
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
void PersistedQueue<_qoT, _sT>::setWorkingDirectory(std::string wdir)
{
  _workingDirectory  = boost::filesystem::path(wdir, boost::filesystem::native);
};

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
void PersistedQueue<_qoT, _sT>::persistToFile(std::string seq, boost::shared_ptr<std::deque<boost::shared_ptr< _qoT > > > p)
{
  std::stringstream       fileName; 
  std::ofstream           outFile;
  boost::filesystem::path nativeFilePath = _workingDirectory;
  std::string             nativeFilePathStr;

  fileName << _queueName << "_" << seq;
    
  nativeFilePath /= fileName.str();

  nativeFilePathStr = nativeFilePath.native();

  if(seq != "0") {
    persistedFileNames.push_back(nativeFilePathStr);
  } else {
    persistedFileNames.push_front(nativeFilePathStr);
  }

  outFile.open(nativeFilePathStr.c_str());

  std::stringstream tmpstrm;
  for(typename std::deque<boost::shared_ptr<_qoT > >::iterator i = p->begin(); i != p->end(); ++i) {
    tmpstrm << (biCoder.encode(*i))->c_str() << '\n';
  }

  outFile << tmpstrm.str(); // write once, strategy, to minimize disk i/o

  outFile.close();
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
boost::shared_ptr<std::deque<boost::shared_ptr< _qoT > > > PersistedQueue<_qoT, _sT>::loadFromFile(const std::string& path2File)
{
  std::string   record;
  std::ifstream inFile;
  boost::shared_ptr<std::deque<boost::shared_ptr< _qoT > > >  tmpQueue;
  std::string   contents;
  
  tmpQueue.reset(new std::deque<boost::shared_ptr< _qoT > >());

  inFile.open(path2File.c_str(), std::ios::in | std::ios::binary);

  if(inFile)
  {
    inFile.seekg(0, std::ios::end);
    contents.resize(inFile.tellg());
    inFile.seekg(0, std::ios::beg);
    inFile.read(&contents[0], contents.size());
    inFile.close();
    boost::filesystem::path file2remove(path2File);
    boost::filesystem::remove(file2remove); // Once the file is loaded it should be removed.


    std::stringstream fileBuf(contents);

    std::getline(fileBuf, record);

    while(!fileBuf.eof()) {
      tmpQueue->push_back(biCoder.decode(record));
      std::getline(fileBuf, record);
    }
  } else {
    // TODO: Some kind of error processing/notification.
  }

  return tmpQueue;
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
void PersistedQueue<_qoT, _sT>::load()
{
  loadPersistedFileNames();
  loadFirstAndLastPage();
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
void PersistedQueue<_qoT, _sT>::loadFirstAndLastPage()
{
  if(persistedFileNames.size() > 0) {
    if(persistedFileNames.size() == 1) {
      firstPage = loadFromFile(persistedFileNames.front());
      lastPage  = firstPage;
      persistedFileNames.pop_front();
    } else {
      firstPage = loadFromFile(persistedFileNames.front());
      lastPage  = loadFromFile(persistedFileNames.back());
      persistedFileNames.pop_front();
      persistedFileNames.pop_back();

      if(persistedFileNames.size() > 0) {
        std::string tmpstr = persistedFileNames[persistedFileNames.size()-1];
      }
    }
  } else {
    firstPage.reset(new std::deque<boost::shared_ptr< _qoT > >());
    lastPage = firstPage;
  }
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
void PersistedQueue<_qoT, _sT>::loadPersistedFileNames()
{
  if(stateFileExists()) {
    loadStateFile();
  } else {
    loadOrphanedQueueFiles();
  }
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
void PersistedQueue<_qoT, _sT>::loadOrphanedQueueFiles()
{
  std::vector<boost::filesystem::path> v;

  std::copy(boost::filesystem::directory_iterator(_workingDirectory), boost::filesystem::directory_iterator(), std::back_inserter(v));

  if(v.size() > 0) {
    for(std::vector<boost::filesystem::path>::const_iterator it (v.begin()); it != v.end(); ++it) {
      if(boost::filesystem::is_regular_file(*it)) {

        boost::cmatch match;
        std::string   fileName = (it->filename()).string();

        if(boost::regex_match(fileName.c_str(), match, _queueFileRegex)) {
          persistedFileNames.push_back(it->native());
        }
      }
    }

    std::sort(persistedFileNames.begin(), persistedFileNames.end());
  }
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
void PersistedQueue<_qoT, _sT>::makeStateFilePath()
{
  std::stringstream fileName; 

  _stateFilePath = _workingDirectory;

  fileName << _queueName << "_STATE";

  _stateFilePath /= fileName.str();
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
bool PersistedQueue<_qoT, _sT>::stateFileExists()
{
  bool retval = false;

  if(boost::filesystem::exists(_stateFilePath) && boost::filesystem::is_regular_file(_stateFilePath)) {
    retval = true;
  }

  return retval;
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
void PersistedQueue<_qoT, _sT>::loadStateFile()
{
  std::ifstream stateFile;
  std::string   record;

  stateFile.open((_stateFilePath.native()).c_str());
  std::getline(stateFile, record);
  while(!stateFile.eof()) {
    persistedFileNames.push_back(record);
    std::getline(stateFile, record);
  }
  stateFile.close();

  boost::filesystem::remove(_stateFilePath);
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
void PersistedQueue<_qoT, _sT>::writeFirstAndLastPage()
{
  if(firstPage->size() > 0) {
    persistToFile("0", firstPage); 
  }

  if(lastPage != firstPage) {
    persistToFile(seqNumber(), lastPage); 
  }
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
void PersistedQueue<_qoT, _sT>::writeStateFile()
{
  if(persistedFileNames.size() > 0) {
    std::ofstream outFile;

    outFile.open(_stateFilePath.c_str());

    while(persistedFileNames.size() > 0) {
      outFile << persistedFileNames.front() << "\n";
      persistedFileNames.pop_front();
    }

    outFile.close();
  }
}

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
std::string PersistedQueue<_qoT, _sT>::seqNumber()
{
  std::stringstream retval;
  retval << time(NULL);
  return retval.str();
}

