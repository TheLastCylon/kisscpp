// File  : persisted_queue.hpp
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

#ifndef _PERSISTED_QUEUE_HPP_
#define _PERSISTED_QUEUE_HPP_

#include <sstream>
#include <string>
#include <fstream>
#include <deque>
#include <algorithm>
#include <cerrno>
#include <ctime>
#include <stdexcept>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/exception.hpp>

#include "logstream.hpp"
#include "statable_queue.hpp"

//--------------------------------------------------------------------------------
using namespace boost::archive::iterators;

namespace kisscpp
{

//--------------------------------------------------------------------------------
template <class T>
class Base64BiCoder
{
  public:
    typedef transform_width< binary_from_base64<remove_whitespace<std::string::const_iterator> > , 8, 6 > BinaryType;
    typedef base64_from_binary<transform_width<std::string::const_iterator,6,8> >                         Base64Type;

     Base64BiCoder() {};
    ~Base64BiCoder() {};

    virtual boost::shared_ptr<std::string> encode(const boost::shared_ptr<T> obj2encode) = 0;
    virtual boost::shared_ptr<T>           decode(const std::string&         str2decode) = 0;

  protected:
    boost::shared_ptr<std::string> encodeToBase64String(const std::string&  s)
    {
      LogStream                      log(__PRETTY_FUNCTION__);
      unsigned int                   writePaddChars = (3 - s.length() % 3) % 3;
      boost::shared_ptr<std::string> base64;

      base64.reset(new std::string(Base64Type(s.begin()),Base64Type(s.end())));

      base64->append(writePaddChars,'=');

      return base64;
    }

    boost::shared_ptr<std::string> decodeFromBase64(const std::string& s)
    {
      LogStream                      log(__PRETTY_FUNCTION__);
      unsigned int                   paddChars = count(s.begin(), s.end(), '=');
      std::string                    is        = s;
      boost::shared_ptr<std::string> result;

      log << manip::debug_normal << "base 64 string to decode : " << s << endl;

      if(is[is.size()-1] == '=') is[is.size()-1] = 'A';
      if(is[is.size()-2] == '=') is[is.size()-2] = 'A';

      result.reset(new std::string(BinaryType(is.begin()), BinaryType(is.end()))); // decode

      result->erase(result->end()-paddChars,result->end());  // erase padding '\0' characters

      log << manip::debug_normal << "decoded base 64 string: " << result->c_str() << endl;

      return result;
    }

  private:
};

//--------------------------------------------------------------------------------
class PersistedQueueError : public std::runtime_error
{
  public:
  protected:
    explicit PersistedQueueError(const std::string& msg) : std::runtime_error(msg) {}

    //PersistedQueueError(const CommandStatus stat);
    //PersistedQueueError(const CommandStatus stat, const std::string& msg);

    virtual ~PersistedQueueError() throw() {};

  private:
};

//--------------------------------------------------------------------------------
class CouldNotLoadQueueFile : public PersistedQueueError
{
  public:
    explicit CouldNotLoadQueueFile(const std::string file_name,const std::string error_msg) : PersistedQueueError("Could not load queue file [" + file_name + "] error: " + error_msg) {};
};

//--------------------------------------------------------------------------------
class CouldNotWriteQueueFile : public PersistedQueueError
{
  public:
    explicit CouldNotWriteQueueFile(const std::string file_name,const std::string error_msg) : PersistedQueueError("Could not write queue file [" + file_name + "] error: " + error_msg) {};
};

//--------------------------------------------------------------------------------
class QueueShutDown : public PersistedQueueError
{
  public:
    explicit QueueShutDown() : PersistedQueueError("No action allowed. Queue has been shut down") {};
};

//--------------------------------------------------------------------------------
// These define statements exist for code readabililty purposes
// Normally they should be typedef statements, but this is a templitized class.
// There are probably better ways of dealing with this, but I'm rather pushed for
// time right now, and this is cleanest, most convenient solution available to me.
//#define QueuedObjectPointerType boost::shared_ptr< _qoT >
//#define QueueType               std::deque<QueuedObjectPointerType >
//#define QueueTypePtr            boost::shared_ptr<QueueType >
//#define QueueTypeIterator       typename std::deque<boost::shared_ptr<_qoT > >::iterator
//#define value_type              QueueType::value_type

//--------------------------------------------------------------------------------
template <class _qoT, class _sT>
class PersistedQueue : public StatAbleQueue, public boost::noncopyable
{
//typedef boost::shared_ptr< _qoT >                                  QueuedObjectPointerType;
//typedef std::deque<boost::shared_ptr< _qoT > >                     QueueType              ;
//typedef boost::shared_ptr<std::deque<boost::shared_ptr< _qoT > > > QueueTypePtr           ;
//typedef typename std::deque<boost::shared_ptr<_qoT > >::iterator   QueueTypeIterator      ;
//typedef typename QueueType::value_type                             value_type             ;

  public:
             PersistedQueue(const std::string& queueName,
                            const std::string& queueWorkingDir,
                            const unsigned     maxItemsPerPage);

    virtual ~PersistedQueue();

    void                      push_back (boost::shared_ptr< _qoT > p);
    void                      push_front(boost::shared_ptr< _qoT > p);
    boost::shared_ptr< _qoT > pop_front ();
    boost::shared_ptr< _qoT > front();
    void                      shutdown();
    void                      clear();
    bool                      empty();
    size_t                    size();

  protected:

  private:
    void                                                       setWorkingDirectory   (std::string wdir);
    void                                                       persistToFile         (std::string seq, boost::shared_ptr<std::deque<boost::shared_ptr< _qoT > > > p);
    boost::shared_ptr<std::deque<boost::shared_ptr< _qoT > > > loadFromFile          (const std::string& path2File);
    boost::shared_ptr<std::deque<boost::shared_ptr< _qoT > > > loadFrontFile         ();
    boost::shared_ptr<std::deque<boost::shared_ptr< _qoT > > > loadBackFile          ();
    void                                                       load                  ();
    void                                                       loadFirstAndLastPage  ();
    void                                                       loadPersistedFileNames();
    void                                                       loadOrphanedQueueFiles();
    void                                                       makeStateFilePath     ();
    bool                                                       stateFileExists       ();
    void                                                       loadStateFile         ();
    void                                                       writeFirstAndLastPage ();
    void                                                       writeStateFile        ();
    std::string                                                seqNumber             ();

    bool                    _shut_down;
    time_t                  _lastSeqNum;
    unsigned int            _seqNumCount;
    std::string             _queueName;
    boost::filesystem::path _workingDirectory;
    boost::filesystem::path _stateFilePath;
    unsigned                _maxItemsPerPage;
    boost::regex            _queueFileRegex;

    std::deque<std::string> persistedFileNames;

    boost::shared_ptr<std::deque<boost::shared_ptr< _qoT > > > firstPage;
    boost::shared_ptr<std::deque<boost::shared_ptr< _qoT > > > lastPage;
    boost::shared_ptr<std::deque<boost::shared_ptr< _qoT > > > swapPage;

    _sT                     biCoder;
};

#include "persisted_queue.tpp"

}

#endif

