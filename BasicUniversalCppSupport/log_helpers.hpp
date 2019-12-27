//
// log_helpers.hpp : lambdas invoked by log macros, and error_context
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

#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "gsl.hpp"
#include "logger_factory.hpp"

#pragma warning(disable: 26429 26489)

// Extend the standard macros with a __SHORT_FILE__ option
#define __SHORT_FILE__ (get_short_file(__FILE__))

///<summary>implements the __SHORT_FILE__ macro support.</summary>
///<param name='fullPath'> normally the __FILE__ macro.</param>
///<returns> a constexpr referencing the filename part of the filepath parameter</returns>
///<remarks>Don't use directly, favour using macros instead. E.g. __SHORT_PATH__</remarks>
static constexpr const char* get_short_file(const char* const full_path)
{
   const char* file_part = full_path;
   const char options[] = { '\\', '/'};
   for (const char slash : options)
   {
      for (const char* pos = full_path; *pos != 0x0; ++pos)
      {
         if (*pos == slash)
            file_part = pos + 1;
      }
      if (file_part != full_path) break;
   }
   return file_part;
}

#pragma warning(default: 26429 26489)

//
// HELPER LAMBDAS FOR LOGGING AND EXCEPTION MESSAGE DECORATION 
//

namespace logging
{
#pragma warning(disable: 26486) // accept lifetime warning on invoking the logger via a shared pointer. Its a static singleton (best service we can hope for).

   ///<summary>Create the singleton logger (FOR ENTRYPOINTS ONLY).</summary>
   ///<param name='logType'>the type of logger to use (E.g. file_logger).</param>
   ///<param name='logFilePath'>if file logger this parameter is the path to the log file. The file will be created if necessary.</param>
   ///<param name='logFilter'>a bit mapped filter used to select which types of log events should be recorded in this log.</param>
   ///<remarks>Don't use directly, ENTRYPOINTS should use the macro: CREATE_LOG(logger_factory::type::file_logger, "ripper.log", LogFilter::Full)</remarks>
   const auto create_logger = [](logger_factory::type logType = logger_factory::type::default_logger, const std::string logFilePath = std::string(), LogFilter logFilter = LogFilter::None)
   {
      return logger_factory::getInstance(logType, logFilePath, logFilter);
   };

   ///<summary>Emit log message</summary>
   ///<param name='level'>value used to filter log entry recording.</param>
   ///<remarks>Don't use directly, favour using macros instead. E.g. LOG_ERROR("there was an error")</remarks>
   auto log_it = [](LogLevel level, std::string text)
   {
      try
      {
         logger_factory::getInstance()->writeln(level, text);
      }
      catch (...)
      {
         std::cerr << "logging failed (at level'" << gsl::narrow_cast<int>(level) << "') with log text: '" << text << "'" << std::endl;
      }
   };

   ///<summary>Query active logging level(s)</summary>
   ///<param name='level'>a log level to test.</param>
   ///<remarks>Don't use directly, favour using macro instead: TEST_LOG_LEVEL(LogLevel::Debug)</remarks>
   ///<returns>true if the level parameter is currently set in the log filter (meaning this level will be recorded), otherwise false.</returns>
   auto test_log_level = [](LogLevel level)
   {
      return logger_factory::getInstance()->test_log_level(level);
   };

   ///<summary>Toggle an active logging level(s)</summary>
   ///<param name='level'>a log level to test.</param>
   ///<remarks>Don't use directly, favour using macros instead: TOGGLE_LOG_LEVEL(LogLevel::Debug)
   ///On return, if the level was previously set it will now be clear. If it was clear, it will now be reset.</remarks>
   const auto toggle_log_level = [](LogLevel level)
   {
      logger_factory::getInstance()->toggle_log_level(level);
   };

   ///<summary>Fetch log content (provided primarily as unit test support).</summary>
   ///<remarks>Don't use directly, favour using macro instead: LOG_FILE_CONTENTS()</remarks>
   auto read_all = []()
   {
      std::string log_content;
      try
      {
         log_content = logger_factory::getInstance()->read_all();
      }
      catch (...)
      {
         std::cerr << "reading log file failed." << std::endl;
      }
      return log_content;
   };

   ///<summary>width of the log file file detail column (you might want to configure this differently)</summary>
   enum { FILE_DETAIL_WIDTH = 32 };

   const auto build_file_detail = [](std::string file, const int lineNo)
   {
      std::string detail = file.append("()");
      return detail.insert(detail.find_last_of(")"), std::to_string(lineNo));
   };

   ///<summary>lambda to 'decorate' log and exception text</summary>
   ///<remarks>Don't use directly, favour using macros instead. E.g. LOG_INFO() etc.</remarks>
   const auto decorate_log_text = [](std::string file, int lineNo, std::string text)
   {
      std::stringstream stream;
      stream << std::left << std::setw(FILE_DETAIL_WIDTH) << build_file_detail(file, lineNo) << ": " << text;
      return stream.str();
   };

   ///<summary>lambda to 'decorate' error context</summary>
   ///<remarks>Don't use directly, favour using macro instead: throw error_context("some description of cause")</remarks>
   auto decorate_error_context = [](std::string pathName, int lineNo, std::string function, std::string text, std::string reason)
   {
      std::stringstream stream;
      stream << build_file_detail(pathName, lineNo) << " " << function << ": " << text << " - " << reason ;
      return stream.str();
   };

#pragma warning(default: 26486)
};