//
// logger.hpp : implements logging, and exception message support
//
// Copyright (c) 2017-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
//    This program is free software : you can redistribute itand /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.If not, see < http://www.gnu.org/licenses/>.
//
#pragma once

#ifdef BASICUNIVERSALCPPSUPPORT_EXPORTS
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

///<summary> strong type naming all allowable combinations of (single bit) flags as used to configure a logger.</summary>
enum class BASICUNIVERSALCPPSUPPORT_API LogFilter : int
{
   None = 0x00,
   Trace = 0x01,
   Debug = 0x02,
   Info = 0x04,
   Warning = 0x08,
   Error = 0x10,

   Normal = Info | Warning | Error,
   Full = Trace | Debug | Info | Warning | Error
};

///<summary> strong type naming all the (single bit) flags as used as used in logger write functions.</summary>
enum class BASICUNIVERSALCPPSUPPORT_API LogLevel : int
{
   None = static_cast<int>(LogFilter::None),   // keep, it _is_ useful
   Trace = static_cast<int>(LogFilter::Trace),
   Debug = static_cast<int>(LogFilter::Debug),
   Info = static_cast<int>(LogFilter::Info),
   Warning = static_cast<int>(LogFilter::Warning),
   Error = static_cast<int>(LogFilter::Error)
};

// decoration of log entries: by default include source and line number info in debug build, exclude it in the release build
#ifndef LOG_TEXT

// developers often want to temporaily fine control the logging levels
// you can/should do this WITHOUT CHANGING THE LOG STATEMENT (temporary changes just get forgotten)
// better is to control logging by locally redefining these log flag macros (Eg #define LOG_INFO /*nothing*/)
// the compiler will raise a MACRO REDEFINITION WARNING to remind you of where you have done this

// USE THESE LOG_LEVEL_MACROS IN LOG WRITE LINES 
#define LOGGER Logger
#define LOG_NONE(text) (LOGGER.writeln(LogLevel::None, LOG_TEXT(text)))
#define LOG_TRACE(text) (LOGGER.writeln(LogLevel::Trace, LOG_TEXT(text)))
#define LOG_DEBUG(text) (LOGGER.writeln(LogLevel::Debug, LOG_TEXT(text)))
#define LOG_INFO(text) (LOGGER.writeln(LogLevel::Info, LOG_TEXT(text)))
#define LOG_WARNING(text) (LOGGER.writeln(LogLevel::Warning, LOG_TEXT(text)))
#define LOG_ERROR(text) (LOGGER.writeln(LogLevel::Error, LOG_TEXT(text)))

#define S1(x) #x
#define S2(x) S1(x)

#define DECORATED_LOG_TEXT(y) std::string(__FILE__ " : " S2(__LINE__) " ").append(y)
#define PLAIN_LOG_TEXT(y) y

#define LOG_LEVEL(level) (LOGGER.test_log_level(level))
#define TOGGLE_LOG_LEVEL(level) (LOGGER.toggle_log_level(level))

#ifdef _DEBUG
   //* <<< add or delete a second leading '/' on this line to toggle the global decoration option for Debug configuration
   // use this to decorate the logger::write line parameter so that location details are included in the logentry
#define LOG_TEXT DECORATED_LOG_TEXT
   /*/
   // use this for undecorated version
   #define LOG_TEXT PLAIN_LOG_TEXT
   //*/
#else
   /* <<< add or delete a second leading '/' on this line to toggle the global decoration option for Release configuration
   // use this to decorate the logger::write line parameter so that location details are included in the logentry
   #define LOG_TEXT DECORATED_LOG_TEXT
   /*/
   // use this for undecorated version
#define LOG_TEXT PLAIN_LOG_TEXT
   //*/
#endif
#endif

///<summary>abstract base class for loggers</summary>
class logger
{
public:

   ///<summary> get a textual description of the log level. Use this when constructing a log entry.</summary>
   ///<returns> text identifying the LogLevel (extended with whitespace to help log alignment).</returns>
   ///<exception cref='std::invalid_argument'> if LogLevel::none is supplied. Puropse is to comment log entries, and in this context LogLevel::None is never valid.</exception>
   ///<exception cref='std::logic_error'> if (after implementation change) an unexpected LogLevel value is supplied that is not (yet) supported here.</exception>
   BASICUNIVERSALCPPSUPPORT_API const std::string log_level(LogLevel level) const
   {
      std::string result;

      switch (level)
      {
      case LogLevel::None:
         throw std::invalid_argument("LogLevel::None is invalid in this context (caller should filter this out)");
         break;

      case LogLevel::Trace:
         result = "Trace   ";
         break;

      case LogLevel::Debug:
         result = "Debug   ";
         break;

      case LogLevel::Info:
         result = "Info    ";
         break;

      case LogLevel::Warning:
         result = "Warning ";
         break;

      case LogLevel::Error:
         result = "Error   ";
         break;

      default:
         throw std::logic_error("LogLevel redefined? Review and revise case statements in log_level() implementation");
      }
      return result;
   }

   ///<summary> default constructor.</summary>
   BASICUNIVERSALCPPSUPPORT_API logger() = default;
      
   ///<summary> copy constructor.</summary>
   BASICUNIVERSALCPPSUPPORT_API logger(const logger& other) = default;

   ///<summary> move constructor.</summary>
   BASICUNIVERSALCPPSUPPORT_API logger(logger&& other) = default;

   ///<summary> virtualize copy assignment operator.</summary>
   BASICUNIVERSALCPPSUPPORT_API virtual logger& operator=(logger& other) = default;
 
   ///<summary> virtualize move assignment operator.</summary>
   BASICUNIVERSALCPPSUPPORT_API virtual logger& operator=(logger&& other) = default;

   ///<summary> virtual destructor.</summary>
   BASICUNIVERSALCPPSUPPORT_API virtual ~logger() = default;

   ///<summary> used to determine which messages get logged
   /// logger implementors compare the bitmask parameter constructed or supplied here 
   /// to the filter parameter supplied with write operations.</summary>
   ///<param name='filter'>LogFilter bitmask value to set. filter is used by logger implementers to select which messages get logged</param>
   BASICUNIVERSALCPPSUPPORT_API virtual void set_log_filter(LogFilter filter) = 0;

   ///<summary> Get log flags.</summary>
   ///<returns> The current LogFilter bitmask.</returns>
   BASICUNIVERSALCPPSUPPORT_API virtual LogFilter get_log_filter() const = 0;

   ///<summary> test a log level. compares the parameter with the log flags of this logger.</summary>
   ///<param name='aLevel'> the LogLevel to test.</param>
   ///<returns> true if the flag is set in the LogFilter of this logger.</returns> 
   BASICUNIVERSALCPPSUPPORT_API bool inline test_log_level(LogLevel aLevel) const
   { 
      return (static_cast<int>(get_log_filter()) & static_cast<int>(aLevel)) ? true : false; 
   }

   ///<summary> toggle a log level. changes the level in the log flags of this logger.</summary>
   ///<param name='aFlag'> the LogLevel to toggle.</param>
   BASICUNIVERSALCPPSUPPORT_API void inline toggle_log_level(LogLevel aLevel)
   { 
      set_log_filter(test_log_level(aLevel) 
         ? static_cast<LogFilter>((static_cast<int>(get_log_filter()) & ~static_cast<int>(aLevel))) 
         : static_cast<LogFilter>((static_cast<int>(get_log_filter()) | static_cast<int>(aLevel)))); 
   }

   /// <summary> Write message to log without newline.</summary>
   /// <param name="level">the LogLevel used to filter log messages.
   /// write does nothing if the single bit level parameter is not set in the loggers current LogFilter bitmask</param>
   /// <param name="line">The message to log</param>
   BASICUNIVERSALCPPSUPPORT_API virtual void write(LogLevel level, std::string line) = 0;

   /// <summary> Write message to log with newline.</summary>
   /// <param name="level">the LogLevel used to filter log messages.
   /// writeln does nothing if the single bit level parameter is not set in the loggers current LogFilter bitmask</param>
   /// <param name="line">The message to log</param>
   BASICUNIVERSALCPPSUPPORT_API virtual void writeln(LogLevel level, std::string line) = 0;

   /// <summary> Write exception to log.</summary>
   /// <param name="level">the LogLevel used to filter log messages.
   /// write does nothing if the single bit level parameter is not set in the loggers current LogFilter bitmask</param>
   /// <param name="line">The message to log</param>
   BASICUNIVERSALCPPSUPPORT_API virtual void write(LogLevel level, std::exception e) = 0;

   /// <summary> Read the complete log file.</summary>
   /// <returns>The log file contents as a std::string</returns>
   BASICUNIVERSALCPPSUPPORT_API virtual std::string read_all() const = 0;

   /// <summary> Clear log file.</summary>
   BASICUNIVERSALCPPSUPPORT_API virtual void clear() = 0;
};
