// File  : logstream.cpp
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

#include "logstream.hpp"

namespace kisscpp
{

bool                    LogStream::log2consoleFlag;
boost::mutex            LogStream::objMutex;
std::string             LogStream::outFilePath;
std::ofstream           LogStream::outFile;
std::deque<std::string> LogStream::stringPool;

//--------------------------------------------------------------------------------
void LogStream::set2ReOpen()
{
  boost::lock_guard<boost::mutex> guard(objMutex);
  outFile.close();
  // TODO: Rename the file.
}

//--------------------------------------------------------------------------------
void LogStream::start_write()
{
  if(!myMaskEnabled()) return;

  std::string msg;
  msg  = "+ [" + getSource() + "]";
  locked_write(msg);
}

//--------------------------------------------------------------------------------
void LogStream::end_write()
{
  if(!myMaskEnabled()) return;

  std::string msg;
  msg  = "- [" + getSource() + "]";
  locked_write(msg);
}

//--------------------------------------------------------------------------------
void LogStream::write()
{
  if(!myMaskEnabled()) return;

  std::string msg;

  msg  = ": " + mBuf.str();

  locked_write(msg);
  
  lssTemp.reset();
}

//--------------------------------------------------------------------------------
void LogStream::locked_write(std::string &s)
{
  boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
  std::string              str = boost::posix_time::to_iso_string(now) + " " + s;

  try {
    boost::lock_guard<boost::mutex> guard(objMutex);
    if(!outFile.is_open()) {
      openLogFile();
    }

    if(!outFile.is_open()) {
      stringPool.push_back(str);
    } else {
      while(stringPool.size() > 0) {
        outFile << stringPool.front() << std::endl;
        if(log2consoleFlag) {
          std::cout << stringPool.front() << std::endl;
        }
        stringPool.pop_front();
      }

      outFile << str << std::endl;
      if(log2consoleFlag) {
        std::cout << str << std::endl;
      }
    }
  } catch(std::runtime_error &e) {
    stringPool.push_back(str);
  }
}

//--------------------------------------------------------------------------------
void LogStream::openLogFile()
{
  if(outFilePath.size() <= 0) {
    throw std::runtime_error("Log System has no file path set!");
  } else {
    try {
      outFile.open(outFilePath.c_str(), std::ios::out | std::ios::app);
    } catch(const std::ios_base::failure& e) {
      std::stringstream tss;
      tss << "Failed to open log file [" << outFilePath << "]" << "(" << e.what() << ")";
      throw std::runtime_error(tss.str());
    }
  }
}

}

