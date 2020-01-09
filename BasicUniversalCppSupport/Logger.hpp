//
// logger.hpp : exposes logging support to client code. 
//
// ************************************************************************************************
// THIS FILE IS THE SINGLE EXCLUSIVE PLACE WHERE PROGRAMMERS CAN/SHOULD MAKE LOGGING OPTION CHOICES
// ************************************************************************************************
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LOGGING CONFIGURATION CONSISTS OF THE FOLLOWING FIVE OPTIONS 
//
// STATIC_LOG_FILTERING    - Default option assumes a single logger, with a LogFilter that is immutable. LogLevel tests
//                           in the macros LOG_WARNING etc. filter logging at compile time. 
//
// FILE_DETAIL_WIDTH       - Log file layout by default assumes that the file part of the source file name plus line number
//                           used to decorate log entries will not exceed 32 characters. If you choose full path form of the
//                           file name or for other reasons have very long file names you might want to adjust this to
//                           maintain cosmetic appearance in logs
// 
// DEFAULT_LOG_FILTER      - The default (immutable) log filter enabling log levels (ERROR, WARNING, TRACE etc) 
//                           This can be set differently per build configuration.
//
// LOG_TEXT                - Use decorated or non-decorated text in the log entries. You can also vary this per build 
//                           configuration.
//
// LOGGING_INACTIVE        - Switch logging globally ON or OFF.
//

#define STATIC_LOG_FILTERING  // Comment out this line if you need the ability to change log filtering at runtime

#ifndef STATIC_LOG_FILTERING
#define STRINGIZE(x) #x
#define TO_STRING_LITERAL(x) STRINGIZE(x)
#pragma message("Message: LogLevel filtering being is applied at runtime. This has a performance cost. (See: " __FILE__ " near L" TO_STRING_LITERAL(__LINE__) ")" )
#endif

// width of the log file file detail column. In some situations you might want to change this value.
#define FILE_DETAIL_WIDTH 32

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COMPILE-TIME SELECTION OF LOG FILTERING (YOU CAN VARY THIS PER BUILD CONFIGURATION)
//

#ifdef _DEBUG
// Configure for ALL log entry types.
#define DEFAULT_LOG_FILTER LogFilter::Full
#else
// Configure only for errors and warnings.
#define DEFAULT_LOG_FILTER LogFilter::Normal
#endif

#include "log_helpers.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HELPER ANSI 'C' MACRO CONSTRUCTS TO PROVIDE LOCATION OF A LOG ENTRY LINE IN THE SOURCE CODE 
// This can be super useful in some situations, E.g. if you need better feedback from the field.
//
// __SHORT_FILE__ is selected because code is compiled with explicit /ZI /FC options (which influence __FILE__ to be full path
//   if you choose __FILE__ you will also need to widen the log file column (see FILE_DETAIL_WIDTH above)
//
#define DECORATED_LOG_TEXT(text)                                        \
__pragma(warning(push))                                                 \
__pragma(warning(disable:26444 26447))                                  \
std::string(logging::decorate_log_text(__SHORT_FILE__, __LINE__, text)) \
__pragma(warning(pop))

#define PLAIN_LOG_TEXT(text)           \
__pragma(warning(push))                \
__pragma(warning(disable:26444 26447)) \
text                                   \
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

//#define LOGGING_INACTIVE     // uncomment this line if you want to disable logging globally

#ifndef LOGGING_INACTIVE
#define CREATE_LOGGER(logType, logFilePath, logFilter)                                                \
__pragma(warning(push))                                                                               \
__pragma(warning(disable:26426))                                                                      \
std::shared_ptr<logger_interface>the_logger = logging::create_logger(logType, logFilePath, logFilter) \
__pragma(warning(pop))

#define TEST_LOG_LEVEL(level) logging::test_log_level(level)
#define LOG_FILE_CONTENTS logging::read_all()

#ifdef STATIC_LOG_FILTERING
#define IS_ENABLED(a_level) logging::is_enabled_constexpr(a_level)
#define TOGGLE_LOG_LEVEL(level) throw std::runtime_error("Static log filters cannot be changed at runtime");
#else
#define IS_ENABLED(a_level) logging::is_enabled_runtime(a_level)
#define TOGGLE_LOG_LEVEL(level) logging::toggle_log_level(level)
#endif

#define LOG_NONE(text) if (IS_ENABLED(LogLevel::None)) { logging::log_it(LogLevel::None, LOG_TEXT(text)); } 
#define LOG_TRACE(text) if (IS_ENABLED(LogLevel::Trace)) { logging::log_it(LogLevel::Trace, LOG_TEXT(text)); }
#define LOG_DEBUG(text) if (IS_ENABLED(LogLevel::Debug)) { logging::log_it(LogLevel::Debug, LOG_TEXT(text)); }
#define LOG_INFO(text) if (IS_ENABLED(LogLevel::Info)) { logging::log_it(LogLevel::Info, LOG_TEXT(text)); }
#define LOG_WARNING(text) if (IS_ENABLED(LogLevel::Warning)) { logging::log_it(LogLevel::Warning, LOG_TEXT(text)); }
#define LOG_ERROR(text) if (IS_ENABLED(LogLevel::Error)) { logging::log_it(LogLevel::Error, LOG_TEXT(text)); }
#else
// INACTIVE LOGGING (all log lines are just passive code comments)

#ifdef UNREFERENCED_PARAMETER
#undef UNREFERENCED_PARAMETER
#endif
#define UNREFERENCED_PARAMETER(p) try { (void)(p); } catch (...) { }

#define CREATE_LOGGER(logType, logFilePath, logFilter)
#define TEST_LOG_LEVEL(level) (false)
#define LOG_FILE_CONTENTS std::string()
#define TOGGLE_LOG_LEVEL(level) {}

#define LOG_NONE(text) UNREFERENCED_PARAMETER(text)
#define LOG_TRACE(text) UNREFERENCED_PARAMETER(text)
#define LOG_DEBUG(text) UNREFERENCED_PARAMETER(text)
#define LOG_INFO(text) UNREFERENCED_PARAMETER(text)
#define LOG_WARNING(text) UNREFERENCED_PARAMETER(text)
#define LOG_ERROR(text) UNREFERENCED_PARAMETER(text)
#endif
