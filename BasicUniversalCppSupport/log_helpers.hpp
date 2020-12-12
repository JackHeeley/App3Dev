//
// log_helpers.hpp : static helper functions to support log macros, and error_context.
//
// The main purpose for this header is to promote readability in "logger.hpp" where a
// programmer can make compile time logging choices (based upon what he/she reads there).
//
// Copyright (c) 2017-2020 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
//    This program is free software : you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.If not, see < http://www.gnu.org/licenses/ >.
//
#pragma once

#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "gsl.hpp"
#include "logger.hpp"
#include "logger_factory.hpp"

#define LOG_HELPERS_WARNINGS_SUPPRESSED 26429 26481
#pragma warning(disable: LOG_HELPERS_WARNINGS_SUPPRESSED)

// Extend the standard macros with a __SHORT_FILE__ option
#define __SHORT_FILE__ (get_short_file(__FILE__))

///<summary>implements the __SHORT_FILE__ macro support.</summary>
///<param name='fullPath'> normally the __FILE__ macro.</param>
///<returns> a constexpr referencing the filename part of the filepath parameter</returns>
///<remarks>Don't use directly. Use macro __SHORT_FILE__ instead.</remarks>
static constexpr const char* get_short_file(const char* const full_path)
{
   const char* file_part = full_path;
   const char options[] = { '\\', '/' };
   for (const char slash : options)
   {
      for (const char* pos = full_path; *pos != 0x0; ++pos)
      {
         if (*pos == slash)
         {
            if (file_part <= pos)
               file_part = pos + 1;
         }
      }
   }
   return file_part;
};

#pragma warning(default: LOG_HELPERS_WARNINGS_SUPPRESSED)


//
// HELPERS FOR LOGGING AND EXCEPTION MESSAGE DECORATION 
//

namespace logging
{
   ///<summary>a compile time check to see if a logging level has been enabled.</summary>
   ///<remarks>constexpr allows (but doesn't force) the compiler to optimize away inactive logging code.
   /// This optimizing opportunity comes at the (small) cost of requiring log levels to be pre-determined and immutable.</remarks>
   ///<param name='a_level'>the LogLevel to be tested.</param>
   ///<returns>true if the loglevel is enabled in the logger, otherwise false.</returns>
   static constexpr const bool is_enabled_constexpr(const LogLevel a_level)
   {
      return ((static_cast<int>(a_level) /*bitwise*/& static_cast<int>(DEFAULT_LOG_FILTER)) != 0);
   };

   ///<summary>a runtime time check to see if a logging level has been enabled.</summary>
   ///<remarks>this check requires a (small) runtime overhead (check) with all logging.
   /// The benefit is that log levels can be changed at runtime (should that be needed).</remarks>
   ///<param name='a_level'>the LogLevel to be tested.</param>
   ///<returns>true if the loglevel is enabled in the logger, otherwise false.</returns>
   static const bool is_enabled_runtime(const LogLevel a_level)
   {
      return ((static_cast<int>(a_level) /*bitwise*/& static_cast<int>(logger_factory::getInstance()->get_log_filter())) != 0);
   };

   ///<summary>Create the singleton logger (FOR ENTRYPOINTS ONLY).</summary>
   ///<param name='logType'>the type of logger to use (E.g. file_logger).</param>
   ///<param name='logFilePath'>if file logger this parameter is the path to the log file. The file will be created if necessary.</param>
   ///<param name='logFilter'>a bit mapped filter used to select which types of log events should be recorded in this log.</param>
   ///<returns> a shared pointer to the abstract_logger instance representing the singleton logger (just created, or created earlier).</returns>
   ///<remarks>Don't use directly, ENTRYPOINTS should use the macro: CREATE_LOG(logger_factory::type::file_logger, "ripper.log", LogFilter::Full)</remarks>
   static const std::shared_ptr<logger_interface> create_logger(logger_factory::logger_type logType = logger_factory::logger_type::null_logger, const std::string& logFilePath = std::string(), LogFilter logFilter = DEFAULT_LOG_FILTER)
   {
      return logger_factory::getInstance(logType, logFilePath, logFilter);
   };

   ///<summary>Emit log message</summary>
   ///<param name='level'>value used to filter log entry recording.</param>
   ///<remarks>Don't use directly, favour logger macros instead. E.g. LOG_ERROR("there was an error")</remarks>
   static void log_it(LogLevel level, const std::string& text)
   {
      try
      {
         logger_factory::getInstance()->writeln(level, text);
      }
      catch (...)
      {
         std::cerr << "logging failed(lvl'" << gsl::narrow_cast<int>(level) << "'): '" << text << "'" << std::endl;
      }
   };

   ///<summary>Query active logging level(s)</summary>
   ///<param name='level'>a log level to test.</param>
   ///<remarks>Don't use directly, favour logger macros instead: TEST_LOG_LEVEL(LogLevel::Debug)</remarks>
   ///<returns>true if the level parameter is currently set in the log filter (meaning this level will be recorded), otherwise false.</returns>
   ///<remarks>#ifdef STATIC_LOG_FILTERING (see Logger.hpp) then this (the logger's internal filter) is ignored.</remarks>
   static bool test_log_level(LogLevel level)
   {
      return logger_factory::getInstance()->test_log_level(level);
   };

   ///<summary>Toggle an active logging level(s)</summary>
   ///<param name='level'>a log level to test.</param>
   ///<remarks>Don't use directly, favour logger macros instead: TOGGLE_LOG_LEVEL(LogLevel::Debug)
   ///On return, if the level was previously set it will now be clear. If it was clear, it will now be reset.</remarks>
   ///<remarks>#ifdef STATIC_LOG_FILTERING (see Logger.hpp) then this (the logger's internal filter) is ignored.</remarks>
   static void toggle_log_level(LogLevel level)
   {
      logger_factory::getInstance()->toggle_log_level(level);
   };

   ///<summary>Fetch log content (provided primarily as unit test support).</summary>
   ///<returns>Entire content of the log file as a string (or an empty string if an error occurred).</returns>
   ///<remarks>Don't use directly, favour logger macros instead: LOG_FILE_CONTENTS().</remarks>
   static const std::string read_all()
   {
      std::string log_content;
      try
      {
         log_content = logger_factory::getInstance()->read_all();
      }
      catch (...)
      {
         log_it(LogLevel::Error, "reading log file failed.");
      }
      return log_content;
   };

   ///<summary>build file detail use to 'decorate' log and exception text</summary>
   ///<remarks>Don't use directly, favour logger macros instead. E.g. LOG_INFO() etc.</remarks>
   static const std::string build_file_detail(const std::string& file, const int lineNo)
   {
      return std::string(file) + "(" + std::to_string(lineNo) + ")";
   };

   ///<summary>'decorate' log and exception text</summary>
   ///<remarks>Don't use directly, favour logger macros instead. E.g. LOG_INFO() etc.</remarks>
   static const std::string decorate_log_text(const std::string& file, int lineNo, const std::string& text)
   {
      std::stringstream stream;
      stream << std::left << std::setw(FILE_DETAIL_WIDTH) << build_file_detail(file, lineNo) << ": " << text;
      return stream.str();
   };

   ///<summary>'decorate' error context</summary>
   ///<remarks>Don't use directly, favour logger macros instead.E.g. throw error_context("some description of cause");</remarks>
   static const std::string decorate_error_context(const std::string& pathName, int lineNo, const std::string& function, const std::string& text, const std::string& reason)
   {
       return build_file_detail(pathName, lineNo) + " " + function + ": " + text + " - " + reason;
   };
};