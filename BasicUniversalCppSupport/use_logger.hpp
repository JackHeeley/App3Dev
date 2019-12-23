//
// use_logger.hpp : exposes logging support to client code.
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

#include "gsl.hpp"
#include "logger_factory.hpp"

#include <functional>
#include <iostream>
#include <string>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HELPER LAMDAS FOR LOGGING
//

// accept lifetime warning on invoking the logger via a shared pointer. Its a static singleton, and that gives best service we can hope to expect.
#pragma warning(disable: 26486) 

///<summary>lambda to create a log. Only main should do this.</summary>
///<param name='logType'>the type of logger to use (E.g. file_logger).</param>
///<param name='logFilePath'>if file logger this parameter is the path to the log file (it will be created if necessary).</param>
///<param name='logFilter'>a bit mapped filter used to select which types of log events should be recorded in this log.</param>
///<remarks>Don't use directly, favour using macros instead. E.g. CREATE_LOG(logger_factory::type::file_logger, "ripper.log", LogFilter::Full)</remarks>
auto create_logger = [](logger_factory::type logType = logger_factory::type::default_logger, std::string logFilePath = std::string(), LogFilter logFilter = LogFilter::None)
{
   return logger_factory::getInstance(logType, logFilePath, logFilter);
};

///<summary>lambda to emit logging messages</summary>
///<remarks>Don't use directly, favour using macros instead. E.g. LOG_ERROR("there was an error")</remarks>
auto log_it = [](LogLevel level, std::string text)
{
   try
   {
      logger_factory::getInstance()->writeln(level, text);
   }
   catch (...)
   {
      std::cerr << "logging failed (at level'" << gsl::narrow_cast<int>(level) << "') with log text: '" << text << "'"<< std::endl;
   }
};

///<summary>lambda to query the active logging level(s)</summary>
///<param name='level'>a log level to test.</param>
///<remarks>Don't use directly, favour using macros instead. E.g. LOG_LEVEL(LogLevel::Debug)</remarks>
///<returns>true if the level is currently set otherwise false.</returns>
auto test_log_level = [](LogLevel level)
{
   return logger_factory::getInstance()->test_log_level(level);
};

///<summary>lambda to toggle an active logging level(s)</summary>
///<param name='level'>a log level to test.</param>
///<remarks>Don't use directly, favour using macros instead. E.g. TOGGLE_LOG_LEVEL(LogLevel::Debug)
///On return, if the level was previously set it will now be clear. If it was clear, it will now be reset.</remarks>
auto toggle_log_level = [](LogLevel level)
{
   logger_factory::getInstance()->toggle_log_level(level);
};

#pragma warning(default: 26486)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HELPER ANSI 'C' CONSTRUCTS TO PROVIDE LOCATION OF A LOG ENTRY LINE IN THE SOURCE CODE 
// This can be super useful in some situations, E.g. if you need better feedback from the field.

#define S1(x) #x
#define S2(x) S1(x)

#define DECORATED_LOG_TEXT(y)                            \
__pragma(warning(push))                                  \
__pragma(warning(disable:26447))                         \
std::string(__FILE__ " : " S2(__LINE__) " ").append(y)   \
__pragma(warning(pop))

#define PLAIN_LOG_TEXT(y)                                \
__pragma(warning(push))                                  \
__pragma(warning(disable:26447))                         \
y                                                        \
__pragma(warning(pop))



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COMPILE TIME SELECTION - DECORATE THE LOGGING TEXT:  
// by default here we ADD source file and line number details into debug build log entries, but not in the release builds
#ifndef LOG_TEXT

#ifdef _DEBUG
//* <<< add or delete a second leading '/' on this line to change the global decoration option for DEBUG configuration
#define LOG_TEXT DECORATED_LOG_TEXT

/*/
#define LOG_TEXT PLAIN_LOG_TEXT
//*/
#else
/* <<< add or delete a second leading '/' on this line to change the global decoration option for RELEASE configuration
// use this to decorate the log_it text parameter so that location details are included in the logentry
#define LOG_TEXT DECORATED_LOG_TEXT
/*/
#define LOG_TEXT PLAIN_LOG_TEXT
//*/
#endif

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COMPILE TIME SELECTION - DO LOGGING YES/NO:  
// by default here we ENABLE logging in BOTH the debug build, and the release build

//* <<< add or delete a second leading '/' on this line to enable or disable logging at compile time

//ACTIVE LOGGING

#define CREATE_LOGGER(logType, logFilePath, logFilter)                              \
__pragma(warning(push))                                                             \
__pragma(warning(disable:26426))                                                    \
std::shared_ptr<logger>the_logger = create_logger(logType, logFilePath, logFilter)  \
__pragma(warning(pop))

#define USES(parm)

#define LOG_NONE(text) log_it(LogLevel::None, LOG_TEXT(text))
#define LOG_TRACE(text) log_it(LogLevel::Trace, LOG_TEXT(text))
#define LOG_DEBUG(text) log_it(LogLevel::Debug, LOG_TEXT(text))
#define LOG_INFO(text) log_it(LogLevel::Info, LOG_TEXT(text))
#define LOG_WARNING(text) log_it(LogLevel::Warning, LOG_TEXT(text))
#define LOG_ERROR(text) log_it(LogLevel::Error, LOG_TEXT(text))

#define TEST_LOG_LEVEL(level) test_log_level(level)
#define TOGGLE_LOG_LEVEL(level) toggle_log_level(level)
/*/

// INACTIVE LOGGING (log lines are passive code comments)

#define USES(parm) parm

#define CREATE_LOGGER(logType, logFilePath, logFilter)

#define LOG_NONE(text)
#define LOG_TRACE(text)
#define LOG_DEBUG(text)
#define LOG_INFO(text)
#define LOG_WARNING(text)
#define LOG_ERROR(text)

#define TEST_LOG_LEVEL(level) (false)
#define TOGGLE_LOG_LEVEL(level) {}

//*/
