// File  : logstream.hpp
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

#ifndef _LOGSTREAM_HPP_
#define _LOGSTREAM_HPP_

#include <sstream>
#include <fstream>
#include <stdint.h>
#include <iostream>
#include <stdexcept>
#include <deque>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define DEFAULT_MAX_BUFF_SIZE 5000

namespace kisscpp
{
  //--------------------------------------------------------------------------------
  //! Used in conjunction with log_serverity as a log filter.
  enum log_type {
    LT_DEBUG = 0, //!< A log for debugging purposes only.
    LT_INFO  = 1, //!< A purely informational log
    LT_ERROR = 2  //!< An error log
  };

  //! Used in conjunction with log_type as a log filter.
  enum log_severity {
    LS_LOW    = 0,   //!< Low
    LS_NORMAL = 1,   //!< Normal
    LS_HIGH   = 2    //!< High
  };

  //--------------------------------------------------------------------------------
  class LogStream;

  #pragma GCC visibility push(default)

  template<typename T> class ManipResult;

  //--------------------------------------------------------------------------------
  // Free functions which manipulate the stream. Similar to standard C++ stream manipulators.
  namespace manip
  {
    // ------------------------------------
    // LogStream Manipulators (no parameters)
    LogStream& flush       (LogStream& s);
    LogStream& endl        (LogStream& s);
    LogStream& dec         (LogStream& s);
    LogStream& hex         (LogStream& s);
    LogStream& oct         (LogStream& s);
    LogStream& base        (LogStream& s);

    LogStream& error       (LogStream& s, const bool permanent = false);
    LogStream& info        (LogStream& s, const bool permanent = false);
    LogStream& debug       (LogStream& s, const bool permanent = false);

    LogStream& high        (LogStream& s, const bool permanent = false);
    LogStream& normal      (LogStream& s, const bool permanent = false);
    LogStream& low         (LogStream& s, const bool permanent = false);

    LogStream& error_high  (LogStream& s, const bool permanent = false);
    LogStream& error_normal(LogStream& s, const bool permanent = false);
    LogStream& error_low   (LogStream& s, const bool permanent = false);
    LogStream& debug_high  (LogStream& s, const bool permanent = false);
    LogStream& debug_normal(LogStream& s, const bool permanent = false);
    LogStream& debug_low   (LogStream& s, const bool permanent = false);
    LogStream& info_high   (LogStream& s, const bool permanent = false);
    LogStream& info_normal (LogStream& s, const bool permanent = false);
    LogStream& info_low    (LogStream& s, const bool permanent = false);

    //--------------------------------------------------------------------------
    // Stream manipulators (with parameters)
    //
    // ManipResult is the type passed into operator<< because manipulators that take parameters are function calls, as opposed to the name of a function pointer type, as in the case of the no-parameter manipulators.
    // -------------------------------------------------------------------------

    namespace helpers
    {
      LogStream& ent   (LogStream& s, std::string ent_name);
      LogStream& source(LogStream& s, std::string source  );
    }

    ManipResult<std::string> ent (const std::string& ent_name);
    ManipResult<std::string> src (const std::string& source  );

  } // manip

  using namespace manip;

  //--------------------------------------------------------------------------------
  // Internally-used class to temporarily store settings changed by the stream manipulators.
  class LogStreamSettings
  {
    public:
      LogStreamSettings() : messageType(LT_INFO), severity(LS_NORMAL), entityName(""), source("") {}

      LogStreamSettings& operator=(const LogStreamSettings& lss)
      {
        if(&lss != this) {
          messageType = lss.getMessageType();
          severity    = lss.getSeverity();
          entityName  = lss.getEntityName();
          source      = lss.getSource();
        }
        return *this;
      }

      log_type           getMessageType() const throw()              { return messageType; }
      log_severity       getSeverity   () const throw()              { return severity;    }
      const std::string& getEntityName () const throw()              { return entityName;  }
      const std::string& getSource     () const throw()              { return source;      }

      void               setMessageType(log_type                 mt) { messageType = mt;}
      void               setSeverity   (log_severity             ms) { severity    = ms;}
      void               setEntityName (const std::string       &en) { entityName  = en;}
      void               setSource     (const std::string       &s ) { source      = s ;}

    private:
      log_type                 messageType;
      log_severity             severity;
      std::string              entityName;
      std::string              source; // The source of the log message.
  };

  //--------------------------------------------------------------------------------
  class LogStream
  {
    public:
      typedef LogStream& (*manip_func)  (LogStream&);                       // LogStream manipulator function which is passed to operator<< without any arguments (other than the implicit %LogStream).
      typedef LogStream& (*manip_func1) (LogStream&, const bool permanent); // LogStream manipulator function which is passed to operator<< with a single boolean argument.
                                                                            // The boolean is called with `false` from within operator<<, and `true` from the constructor.
      // constructors
      // ------------
      explicit
      LogStream(manip_func1  manip = info_normal)
      {
        manip(*this, false);
        lssTemp = lssPerm;
        doFlush = false;
      }

      // ------------
      LogStream(manip_func1 manip, const std::string &src)
      {
        lssTemp = lssPerm;
        doFlush = false;
        manip(*this, false);
        setSource(src);
      }

      // ------------
      LogStream(const std::string &src)
      {
        lssTemp = lssPerm;
        doFlush = false;
        info_normal(*this, false);
        setSource(src);
        start_write();
      }

      // ------------
      LogStream(const std::string &src,
                const std::string &path,
                const bool         log2console = false,
                const unsigned int i           = DEFAULT_MAX_BUFF_SIZE)
      {
        lssTemp = lssPerm;
        doFlush = false;
        info_normal(*this, true);
        setSource(src);
        log2consoleFlag = log2console;
        outFilePath     = path;
        maxBufferSize   = i;
        start_write();
      }

      LogStream(const LogStream &o)
      {
        lssTemp       = o.lssTemp;
        doFlush       = false;
        maxBufferSize = o.maxBufferSize;
        start_write();
      }

      ~LogStream()
      {
        end_write();
        if(mBuf.tellp() > 0) { flush(); }
      };

      LogStream& operator=(const LogStream& o)
      { 
        if(&o != this) {
          lssTemp       = o.lssTemp;
          maxBufferSize = o.maxBufferSize;
        }
        return *this;
      }

      // Output operator for manipulators
      LogStream& operator<< (manip_func1 f) { return f(*this, false); }; // with one argument (other than the implicit LogStream).
      LogStream& operator<< (manip_func  f) { return f(*this); };        // with no arguments (other than the implicit LogStream).

      LogStream& operator<< (const std::string&       v) { mBuf <<                      v ; return *this; }
      LogStream& operator<< (const char               v) { mBuf <<   static_cast<short>(v); return *this; }
      LogStream& operator<< (const short              v) { mBuf <<                      v ; return *this; }
      LogStream& operator<< (const int                v) { mBuf <<                      v ; return *this; }
      LogStream& operator<< (const long               v) { mBuf <<                      v ; return *this; }
      LogStream& operator<< (const float              v) { mBuf <<                      v ; return *this; }
      LogStream& operator<< (const double             v) { mBuf <<                      v ; return *this; }
      LogStream& operator<< (const u_char             v) { mBuf << static_cast<u_short>(v); return *this; }
      LogStream& operator<< (const u_short            v) { mBuf <<                      v ; return *this; }
      LogStream& operator<< (const u_int              v) { mBuf <<                      v ; return *this; }
      LogStream& operator<< (const u_long             v) { mBuf <<                      v ; return *this; }
      LogStream& operator<< (const unsigned long long v) { mBuf <<                      v ; return *this; }

      LogStream& flush() { do_write(true);  return *this; }; // Forced Flush buffer to log, empties buffer and stringPool.
      LogStream& endl () { do_write(false); return *this; }; // Suggested Flush buffer to log, empties mBuf into stringPool

      // Print numbers in 
      LogStream& dec  () { mBuf << std::dec;      return *this; }; // decimal.
      LogStream& hex  () { mBuf << std::hex;      return *this; }; // hex.
      LogStream& oct  () { mBuf << std::oct;      return *this; }; // octal.

      LogStream& base () { mBuf << std::showbase; return *this; }; // Show the base when printing numbers.

      // getter methods
      log_type           getMessageType () const throw() { return lssPerm.getMessageType(); }
      log_severity       getSeverity    () const throw() { return lssPerm.getSeverity(); }
      const std::string& getEntityName  () const throw() { if(!lssTemp.getEntityName().empty()) return lssTemp.getEntityName(); else return lssPerm.getEntityName(); }
      const std::string& getSource      () const throw() { if(!lssTemp.getSource    ().empty()) return lssTemp.getSource();     else return lssPerm.getSource();     }
      LogStreamSettings& getPermSettings()               { return lssTemp; }
      LogStreamSettings& getTempSettings()               { return lssPerm; }

      // setter methods
      LogStream& setMessageType    (log_type           mt, const bool perminant = false) { if(perminant) { lssPerm.setMessageType(mt);} lssTemp.setMessageType(mt); return *this; }
      LogStream& setSeverity       (log_severity       ms, const bool perminant = false) { if(perminant) { lssPerm.setSeverity   (ms);} lssTemp.setSeverity   (ms); return *this; }
      LogStream& setEntityName     (const std::string& en, const bool perminant = false) { if(perminant) { lssPerm.setEntityName (en);} lssTemp.setEntityName (en); return *this; }
      LogStream& setSource         (const std::string& s , const bool perminant = false) { if(perminant) { lssPerm.setSource     (s) ;} lssTemp.setSource     (s) ; return *this; }
      LogStream& setLevel          (manip_func1        f)                                { f(*this, true); return *this; }

      void       setOutFilePath    (std::string        path)                             { outFilePath     = path; };
      void       setLog2consoleFlag(bool               b)                                { log2consoleFlag = b;    }
      void       set2ReOpen        ();

    private:
      template<typename T>
      void put         (const T            t)         { mBuf << t; } // Writes a value (template)
      void put         (const std::string &s)         { mBuf << s; } // Writes a value
      void do_write    (bool               b = false);
      void start_write ();
      void end_write   ();
      void write       ();                            // Writes to the underlying logging object. This results in a new, timestamped line in the logfile.
      void locked_write(std::string &s);
      void openLogFile ();
      void writeLogFile();

      LogStreamSettings              lssTemp;         // temporary settings
      std::ostringstream             mBuf;            // The buffer where the log message is built up in.

      static bool                    doFlush;
      static LogStreamSettings       lssPerm;         // perminant settings
      static bool                    log2consoleFlag;
      static boost::mutex            objMutex;
      static std::string             outFilePath;
      static std::ofstream           outFile;
      static std::deque<std::string> stringPool;
      static unsigned int            maxBufferSize;   // the maximum number of log lines in stringPool, before a write is forced.
  };

  //--------------------------------------------------------------------------------
  // Internally-used helper constructs.
  // The output operator used to write manipulators that take a single parameter.
  // s - The log stream.
  // m - The manipulator functor (internally used).
  // return A reference to the modified log stream.
  template<typename T>
  inline LogStream& operator<< (LogStream& s, ManipResult<T> m) { return m(s); }

  //--------------------------------------------------------------------------------
  // What makes these cases unique is that insertion operator is being passed the
  // result of a function call, as opposed to a function pointer
  // ss << "Testing, 1 2 3" << kisscpp::manip::endl;
  // T - The manipulator's argument type.
  template<typename T> class ManipResult
  {
    public:
      typedef LogStream& (*func_type) (LogStream&, T);             // The function type.

      ManipResult(func_type f, T t) : mFunc(f), mT(t) {}           // f - The function.
                                                                   // t - The manipulator's parameter.
                                                                   // Stored until operator<< calls operator(), which will pass in the stream.

      LogStream& operator()(LogStream& s) { return mFunc(s, mT); } // Calls the stored function, passing into it its stream argument and the stored parameter.
    private:
      func_type mFunc;                                             // The function.
      T         mT;                                                // The parameter that will be passed into @a func.
  };

  //--------------------------------------------------------------------------------
  namespace manip
  {
    // Flushes the stream (write a line to the log).
    inline LogStream& flush       (LogStream& s) { return s.flush(); }; 
    inline LogStream& endl        (LogStream& s) { return s.endl (); };

    // Enables decimal, hexadicimal and octal output for numbers.
    inline LogStream& dec         (LogStream& s) { return s.dec  (); };
    inline LogStream& hex         (LogStream& s) { return s.hex  (); };
    inline LogStream& oct         (LogStream& s) { return s.oct  (); };

    // Show the base when writing numbers.
    inline LogStream& base        (LogStream& s) { return s.base (); };

    // Sets the log type
    inline LogStream& error       (LogStream& s, const bool permanent) { return s.setMessageType(LT_ERROR , permanent); };
    inline LogStream& info        (LogStream& s, const bool permanent) { return s.setMessageType(LT_INFO  , permanent); };
    inline LogStream& debug       (LogStream& s, const bool permanent) { return s.setMessageType(LT_DEBUG , permanent); };

    // Sets the log severity
    inline LogStream& high        (LogStream& s, const bool permanent) { return s.setSeverity   (LS_HIGH  , permanent); };
    inline LogStream& normal      (LogStream& s, const bool permanent) { return s.setSeverity   (LS_NORMAL, permanent); };
    inline LogStream& low         (LogStream& s, const bool permanent) { return s.setSeverity   (LS_LOW   , permanent); };

    // Combined setting of log type and severity
    inline LogStream& error_high  (LogStream& s, const bool permanent) { return error(high  (s, permanent), permanent); };
    inline LogStream& error_normal(LogStream& s, const bool permanent) { return error(normal(s, permanent), permanent); };
    inline LogStream& error_low   (LogStream& s, const bool permanent) { return error(low   (s, permanent), permanent); };
    inline LogStream& debug_high  (LogStream& s, const bool permanent) { return debug(high  (s, permanent), permanent); };
    inline LogStream& debug_normal(LogStream& s, const bool permanent) { return debug(normal(s, permanent), permanent); };
    inline LogStream& debug_low   (LogStream& s, const bool permanent) { return debug(low   (s, permanent), permanent); };
    inline LogStream& info_high   (LogStream& s, const bool permanent) { return info (high  (s, permanent), permanent); };
    inline LogStream& info_normal (LogStream& s, const bool permanent) { return info (normal(s, permanent), permanent); };
    inline LogStream& info_low    (LogStream& s, const bool permanent) { return info (low   (s, permanent), permanent); };

    namespace helpers
    {
      inline LogStream& ent   (LogStream& s, std::string ent_name) { s.setEntityName(ent_name, false); return s; }
      inline LogStream& source(LogStream& s, std::string source  ) { s.setSource    (source  , false); return s; }
    }

    // Sets the message's entity. A better name for this would've been "subject", perhaps. An example would be a transaction id.
    inline ManipResult<std::string> ent (const std::string& ent_name) { return ManipResult<std::string>(manip::helpers::ent   , ent_name); };

    // Sets the message's source. This would be, for example, the name of the calling object/function
    inline ManipResult<std::string> src (const std::string& source  ) { return ManipResult<std::string>(manip::helpers::source, source  ); };
  };

#pragma GCC visibility pop

}

#endif
