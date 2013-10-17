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

namespace kisscpp
{

//--------------------------------------------------------------------------------
//! Used in conjunction with log_serverity as a log filter.
enum log_type {
  mtERROR, //!< An error log
  mtINFO , //!< A purely informational log
  mtDEBUG  //!< A log for debugging purposes only.
};

//! Used in conjunction with log_type as a log filter.
enum log_severity {
  msHIGH ,  //!< High
  msNORMAL, //!< Normal
  msLOW     //!< Low
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

  LogStream& error       (LogStream& s, const bool permanent);
  LogStream& info        (LogStream& s, const bool permanent);
  LogStream& debug       (LogStream& s, const bool permanent);
                      
  LogStream& high        (LogStream& s, const bool permanent);
  LogStream& normal      (LogStream& s, const bool permanent);
  LogStream& low         (LogStream& s, const bool permanent);

  LogStream& error_high  (LogStream& s, const bool permanent);
  LogStream& error_normal(LogStream& s, const bool permanent);
  LogStream& error_low   (LogStream& s, const bool permanent);
  LogStream& debug_high  (LogStream& s, const bool permanent);
  LogStream& debug_normal(LogStream& s, const bool permanent);
  LogStream& debug_low   (LogStream& s, const bool permanent);
  LogStream& info_high   (LogStream& s, const bool permanent);
  LogStream& info_normal (LogStream& s, const bool permanent);
  LogStream& info_low    (LogStream& s, const bool permanent);

  //--------------------------------------------------------------------------
  // Stream manipulators (with parameters)
  //
  // ManipResult is the type passed into operator<< because manipulators that
  // take parameters are function calls, as opposed to the name of a function
  // pointer type, as in the case of the no-parameter manipulators.
  // -------------------------------------------------------------------------

  namespace helpers
  {
    LogStream& mask  (LogStream& s, uint32_t    m       );
    LogStream& ent   (LogStream& s, std::string ent_name);
    LogStream& source(LogStream& s, std::string source  );
  }

  ManipResult<uint32_t>    mask(uint32_t           m       );
  ManipResult<std::string> ent (const std::string& ent_name);
  ManipResult<std::string> src (const std::string& source  );

} // manip

using namespace manip;

//--------------------------------------------------------------------------------
// Internally-used class to temporarily store settings changed by the stream manipulators.
class LogStreamSettings
{
  public:
    LogStreamSettings() :
      messageType   (mtINFO),
      severity      (msNORMAL),
      entityName    (""),
      source        (""),
      mask          (-1),
      owner         (NULL) {}

    LogStreamSettings& operator=(const LogStreamSettings& lss)
    {
      if(&lss != this) {
        messageType = lss.getMessageType();
        severity    = lss.getSeverity();
        entityName  = lss.getEntityName();
        source      = lss.getSource();
        mask        = lss.getMask();
        owner       = &lss;
      }
      return *this;
    }

    log_type           getMessageType() const throw() { return messageType; }
    log_severity       getSeverity   () const throw() { return severity;    }
    const std::string& getEntityName () const throw() { return entityName;  }
    const std::string& getSource     () const throw() { return source;      }
    uint32_t           getMask       () const throw() { return mask;        }

    void setMessageType(log_type           mt) { messageType = mt;}
    void setSeverity   (log_severity       ms) { severity    = ms;}
    void setEntityName (const std::string& en) { entityName  = en;}
    void setSource     (const std::string& s ) { source      = s ;}
    void setMask       (uint32_t           m ) { mask        = m ;}

    void reset() // Resets all fields to the values of the log stream.
    {
      if(owner) {
        messageType = owner->getMessageType();
        severity    = owner->getSeverity();
        entityName  = owner->getEntityName();
        source.clear();
        mask        = owner->getMask();
      } else {
        messageType = mtINFO;
        severity    = msNORMAL;
        entityName  = "";
        source      = "";
        mask        = -1;
      }
    }

  private:
    log_type         messageType;
    log_severity     severity;
    std::string      entityName;
    std::string      source;          // The source of the log message.
    uint32_t         mask;            // The application subsystem mask.

    const LogStreamSettings *owner;  // The LogStreamSettings instance to take settings from on reset().
};

//--------------------------------------------------------------------------------
// A thin wrapper around VMLog which provides a stream-like interface.
// @author Francois Kritzinger
class LogStream
{
  static const uint32_t DEFAULT_MASK; // The default application susbsystem mask: matches all masks.

  public:
    typedef LogStream& (*manip_func)  (LogStream&);                              // LogStream manipulator function which is passed to operator<< without any arguments (other than the implicit %LogStream).
    typedef LogStream& (*manip_func1) (LogStream&, const bool permanent);        // LogStream manipulator function which is passed to operator<< with a single boolean argument.
                                                                                 // The boolean is called with `false` from within operator<<, and `true` from the constructor.
    explicit
    LogStream(uint32_t         mask, manip_func1 manip = info_normal)           : lssPerm(), lssTemp(lssPerm) { setMask(mask); manip      (*this, true);                 }
    LogStream(uint32_t         mask, manip_func1 manip, const std::string& src) : lssPerm(), lssTemp(lssPerm) { setMask(mask); manip      (*this, true); setSource(src); }
    LogStream(uint32_t         mask, const std::string& src)                    : lssPerm(), lssTemp(lssPerm) { setMask(mask); info_normal(*this, true); setSource(src); start_write(); }

    LogStream(uint32_t           mask,
              const std::string& src,
              const std::string& path,
              const bool         log2console = false) :
      lssPerm(),
      lssTemp(lssPerm)
    {
      log2consoleFlag = log2console;
      outFilePath     = path;
      setMask(mask);
      info_normal(*this, true);
      setSource(src);
      start_write();
    }

    LogStream(const LogStream &o)
    {
      lssPerm = o.lssPerm;
      lssTemp = o.lssTemp;
      start_write();
    }

    ~LogStream()
    {
      end_write();
      if(mBuf.tellp() > 0) {
        flush();
      }
      //outFile.close();
    };

    LogStream& operator=(const LogStream& o)
    { 
      if(&o != this) {
        lssPerm = o.lssPerm;
        lssTemp = o.lssTemp;
      }
      return *this;
    }

    LogStream& operator<< (manip_func1 f) { return f(*this, false); }; // Output operator for manipulators with one argument (other than the implicit LogStream).
    LogStream& operator<< (manip_func  f) { return f(*this); };        // Output operator for manipulators with no arguments (other than the implicit LogStream).

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

    LogStream& flush() { write(); mBuf.str(""); return *this; }; // Causes the contents of the buffer to be written to the log, and empties the buffer.
    LogStream& endl () { return flush();                      }; // Currently synonymous to flush().
    LogStream& dec  () { mBuf << std::dec;      return *this; }; // Print numbers in decimal.
    LogStream& hex  () { mBuf << std::hex;      return *this; }; // Print numbers in hex.
    LogStream& oct  () { mBuf << std::oct;      return *this; }; // Print numbers in octal.
    LogStream& base () { mBuf << std::showbase; return *this; }; // Show the base when printing numbers.

    log_type           getMessageType() const throw() { return lssPerm.getMessageType(); }
    log_severity       getSeverity   () const throw() { return lssPerm.getSeverity(); }
    const std::string& getEntityName () const throw() { if(!lssTemp.getEntityName().empty()) return lssTemp.getEntityName(); else return lssPerm.getEntityName(); }
    uint32_t           getMask       () const throw() { return lssPerm.getMask(); }
    const std::string& getSource     () const throw() { if(!lssTemp.getSource    ().empty()) return lssTemp.getSource();     else return lssPerm.getSource();     }

    LogStream& setMessageType(log_type           mt, const bool perminant = true ){ if(perminant) { lssPerm.setMessageType(mt);} lssTemp.setMessageType(mt); return *this; }
    LogStream& setSeverity   (log_severity       ms, const bool perminant = true ){ if(perminant) { lssPerm.setSeverity   (ms);} lssTemp.setSeverity   (ms); return *this; }
    LogStream& setEntityName (const std::string& en, const bool perminant = false){ if(perminant) { lssPerm.setEntityName (en);} lssTemp.setEntityName (en); return *this; }
    LogStream& setSource     (const std::string& s , const bool perminant = true ){ if(perminant) { lssPerm.setSource     (s) ;} lssTemp.setSource     (s) ; return *this; }

    LogStream& setMask (uint32_t    m) { lssPerm.setMask(m); return *this; }
    LogStream& setLevel(manip_func1 f) { f(*this, true)    ; return *this; }

    LogStreamSettings& getPermSettings() { return lssTemp; }
    LogStreamSettings& getTempSettings() { return lssPerm; }

    void setOutFilePath(std::string path)
    {
      outFilePath = path;
    };

    void setLog2consoleFlag(bool b)
    {
      log2consoleFlag = b;
    }

    void set2ReOpen();

  private:
    template<typename T>
    void put(const T            t) { if(myMaskEnabled()) { mBuf << t; } } // Writes a value unless the mask is disabled.

    void put(const std::string &s) { if(myMaskEnabled()) { mBuf << s; } } // Writes a value unless the mask is disabled.

    // Checks whether this instance's log mask is set in the global, application-wide mask.
    bool myMaskEnabled() const
    {
      //static const VMLog& global_log = *VMLog::getInstance();
      //return (global_log.completeMask() & getMask());
      return true;
    };


    void start_write();
    void end_write();
    void write();                     // Writes to the underlying logging object. This results in a new, timestamped line in the logfile.
    void locked_write(std::string &s);
    void openLogFile();

    LogStreamSettings    lssPerm; // perminant settings
    LogStreamSettings    lssTemp; // temporary settings
    std::ostringstream   mBuf;    // The buffer where the log message is built up in. Gets cleared by flush().
    static bool          log2consoleFlag;
    static boost::mutex  objMutex;
    static std::string   outFilePath;
    static std::ofstream outFile;

    static std::deque<std::string> stringPool;
};

// ==================================
// Internally-used helper constructs.
// ==================================

// The output operator used to write manipulators that take a single parameter.
// s - The log stream.
// m - The manipulator functor (internally used).
// return A reference to the modified log stream.
template<typename T> inline LogStream& operator<< (LogStream& s, ManipResult<T> m) { return m(s); }

//#pragma GCC visibility pop

//#pragma GCC visibility push(default)

//--------------------------------------------------------------------------------
// Used to implement stream manipulators that are called (with arguments) by the user as part of the output operation.
// E.g.:
// ss << via::vmlog::src("MyLoggingFunction") << via::vmlog::endl;
//
// What makes these cases unique is that @c operator<< is being passed the
// result of a function call, as opposed to a function pointer, as in the case
// of something such as via::vmlog::endl:
// ss << "Testing, 1 2 3" << via::vmlog::endl;
// T - The manipulator's argument type.
template<typename T> class ManipResult
{
  public:
    typedef LogStream& (*func_type) (LogStream&, T);             // The function type.

    ManipResult(func_type f, T t) : mFunc(f), mT(t) {}           // f - The function.
                                                                 // t - The manipulator's parameter. Stored until operator<< calls operator(), which will pass in the stream.

    LogStream& operator()(LogStream& s) { return mFunc(s, mT); } // Calls the stored function, passing into it its stream argument and the stored parameter.
  private:
    func_type mFunc;                                             // The function.
    T         mT;                                                // The parameter that will be passed into @a func.
};

namespace manip
{
  inline LogStream& flush       (LogStream& s) { return s.flush(); }; // Flushes the stream (write a line to the log).
  inline LogStream& endl        (LogStream& s) { return s.endl (); }; // Flushes the stream (write a line to the log).
  inline LogStream& dec         (LogStream& s) { return s.dec  (); }; // Enables decimal output for numbers.
  inline LogStream& hex         (LogStream& s) { return s.hex  (); }; // Enables hexadecimal output for numbers.
  inline LogStream& oct         (LogStream& s) { return s.oct  (); }; // Enables octal output for numbers.
  inline LogStream& base        (LogStream& s) { return s.base (); }; // Show the base when writing numbers.

  inline LogStream& error       (LogStream& s, const bool permanent) { return s.setMessageType(mtERROR , permanent); }; // Sets the message type to error
  inline LogStream& info        (LogStream& s, const bool permanent) { return s.setMessageType(mtINFO  , permanent); }; // Sets the message type to info.
  inline LogStream& debug       (LogStream& s, const bool permanent) { return s.setMessageType(mtDEBUG , permanent); }; // Sets the message type to debug.
                      
  inline LogStream& high        (LogStream& s, const bool permanent) { return s.setSeverity   (msHIGH  , permanent); }; // Sets the severity to high.
  inline LogStream& normal      (LogStream& s, const bool permanent) { return s.setSeverity   (msNORMAL, permanent); }; // Sets the severity to med.
  inline LogStream& low         (LogStream& s, const bool permanent) { return s.setSeverity   (msLOW   , permanent); }; // Sets the severity to low.

  inline LogStream& error_high  (LogStream& s, const bool permanent) { return error(high  (s, permanent), permanent); }; // Sets the message type to err and the severity to high.
  inline LogStream& error_normal(LogStream& s, const bool permanent) { return error(normal(s, permanent), permanent); }; // Sets the message type to err and the severity to med.
  inline LogStream& error_low   (LogStream& s, const bool permanent) { return error(low   (s, permanent), permanent); }; // Sets the message type to err and the severity to low.
  inline LogStream& debug_high  (LogStream& s, const bool permanent) { return debug(high  (s, permanent), permanent); }; // Sets the message type to dbg and the severity to high.
  inline LogStream& debug_normal(LogStream& s, const bool permanent) { return debug(normal(s, permanent), permanent); }; // Sets the message type to dbg and the severity to med.
  inline LogStream& debug_low   (LogStream& s, const bool permanent) { return debug(low   (s, permanent), permanent); }; // Sets the message type to dbg and the severity to low.
  inline LogStream& info_high   (LogStream& s, const bool permanent) { return info (high  (s, permanent), permanent); }; // Sets the message type to info and the severity to crit.
  inline LogStream& info_normal (LogStream& s, const bool permanent) { return info (normal(s, permanent), permanent); }; // Sets the message type to info and the severity to high.
  inline LogStream& info_low    (LogStream& s, const bool permanent) { return info (low   (s, permanent), permanent); }; // Sets the message type to info and the severity to low.

  namespace helpers
  {
    inline LogStream& mask  (LogStream& s, uint32_t    m       ) { s.setMask      (m)              ; return s; }
    inline LogStream& ent   (LogStream& s, std::string ent_name) { s.setEntityName(ent_name, false); return s; }
    inline LogStream& source(LogStream& s, std::string source  ) { s.setSource    (source  , false); return s; }
  }

  inline ManipResult<uint32_t>    mask(uint32_t           m       ) { return ManipResult<uint32_t>   (manip::helpers::mask  , m       ); }; // Sets a log stream's application subsystem mask to m
  inline ManipResult<std::string> ent (const std::string& ent_name) { return ManipResult<std::string>(manip::helpers::ent   , ent_name); }; // Sets the message's entity. A better name for this would've been "subject", perhaps.
                                                                                                                                     // An example would be the transaction's ID.
  inline ManipResult<std::string> src (const std::string& source  ) { return ManipResult<std::string>(manip::helpers::source, source  ); }; // Sets the message's source. This would be, for example, the name of the calling object
};

#pragma GCC visibility pop

}

#endif
