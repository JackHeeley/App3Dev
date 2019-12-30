//
// logger.hpp : exposes logging support to client code.
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

#include "log_helpers.hpp"



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HELPER ANSI 'C' CONSTRUCTS TO PROVIDE LOCATION OF A LOG ENTRY LINE IN THE SOURCE CODE 
// This can be super useful in some situations, E.g. if you need better feedback from the field.
//
// __SHORT_FILE__ is selected because code is compiled with explicit /ZI /FC options (which influence __FILE__ to be full path
//   if you choose __FILE__ you will also need to widen the log file column (see "log_helpers.hpp" FILE_DETAIL_WIDTH )
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

//* <<< add or delete a second leading '/' on this line to enable or disable logging at compile time

//ACTIVE LOGGING

#define CREATE_LOGGER(logType, logFilePath, logFilter)                                                \
__pragma(warning(push))                                                                               \
__pragma(warning(disable:26426))                                                                      \
std::shared_ptr<abstract_logger>the_logger = logging::create_logger(logType, logFilePath, logFilter)  \
__pragma(warning(pop))

#define LOG_NONE(text) if (IS_ACTIVE(LogLevel::None)) { logging::log_it(LogLevel::None, LOG_TEXT(text)); } 
#define LOG_TRACE(text) if (IS_ACTIVE(LogLevel::Trace)) { logging::log_it(LogLevel::Trace, LOG_TEXT(text)); }
#define LOG_DEBUG(text) if (IS_ACTIVE(LogLevel::Debug)) { logging::log_it(LogLevel::Debug, LOG_TEXT(text)); }
#define LOG_INFO(text) if (IS_ACTIVE(LogLevel::Info)) { logging::log_it(LogLevel::Info, LOG_TEXT(text)); }
#define LOG_WARNING(text) if (IS_ACTIVE(LogLevel::Warning)) { logging::log_it(LogLevel::Warning, LOG_TEXT(text)); }
#define LOG_ERROR(text) if (IS_ACTIVE(LogLevel::Error)) { logging::log_it(LogLevel::Error, LOG_TEXT(text)); }

#define TEST_LOG_LEVEL(level) logging::test_log_level(level)
#define TOGGLE_LOG_LEVEL(level) logging::toggle_log_level(level)
#define LOG_FILE_CONTENTS logging::read_all()
/*/

// INACTIVE LOGGING (log lines are passive code comments)

#ifdef UNREFERENCED_PARAMETER
#undef UNREFERENCED_PARAMETER
#endif
#define UNREFERENCED_PARAMETER(p) try { (void)(p); } catch (...) { }

#define CREATE_LOGGER(logType, logFilePath, logFilter)

#define LOG_NONE(text) UNREFERENCED_PARAMETER(text)
#define LOG_TRACE(text) UNREFERENCED_PARAMETER(text)
#define LOG_DEBUG(text) UNREFERENCED_PARAMETER(text)
#define LOG_INFO(text) UNREFERENCED_PARAMETER(text)
#define LOG_WARNING(text) UNREFERENCED_PARAMETER(text)
#define LOG_ERROR(text) UNREFERENCED_PARAMETER(text)

#define TEST_LOG_LEVEL(level) (false)
#define TOGGLE_LOG_LEVEL(level) {}
#define LOG_FILE_CONTENTS std::string()

//*/
