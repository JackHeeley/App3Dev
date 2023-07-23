//
// null_logger.hpp : implements null logging
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
#ifndef __NULL_LOGGER_HPP__
#define __NULL_LOGGER_HPP__

#ifdef BASICUNIVERSALCPPSUPPORT_EXPORTS
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include "logger_interface.hpp"

///<summary>null logger for ansi c++17/utf8 code clients</summary>
class null_logger : public logger_interface
{
public:
   ///<summary> construct a default file_logger.</summary>
   BASICUNIVERSALCPPSUPPORT_API null_logger() noexcept {}

   ///<summary>construct logger</summary>
   ///<param name='fileName'>path and name of the log file.</param>
   ///<param name='filter'>bitmask used to filter log write events.</param>
   BASICUNIVERSALCPPSUPPORT_API null_logger(const std::string fileName, LogFilter filter) noexcept {}

   ///<summary> equals comparison operator.</summary>
   ///<remarks> defines equals always true.</remarks>
   BASICUNIVERSALCPPSUPPORT_API bool operator==(const null_logger& other) const noexcept {
      return true;
   }

   ///<summary> not equals comparison operator.</summary>
   ///<remarks> defines not equals never true.</remarks>
   BASICUNIVERSALCPPSUPPORT_API bool operator!=(const null_logger& other) const noexcept {
      return false;
   }

   ///<summary> used to determine which messages get logged. loggers compare the filter bitmask supplied
   /// here (or at construction time) with the single bit level supplied as parameter to write operations.</summary>
   ///<param name='filter'>bitmask used to filter log write events.</param>
   BASICUNIVERSALCPPSUPPORT_API void set_log_filter(LogFilter filter) noexcept override {}

   ///<summary> get log filter.</summary>
   ///<returns>the current filter.</returns>
   BASICUNIVERSALCPPSUPPORT_API LogFilter get_log_filter() const noexcept override { return LogFilter::None; }

   ///<summary> Write message to log without newline.</summary>
   ///<remarks> writeln does nothing if the single bit level parameter is not set in the loggers current LogFilter bitmask.</remarks>
   ///<param name="level"> the LogLevel used to filter log messages.</param>
   ///<param name="text"> The message to log.</param>
   BASICUNIVERSALCPPSUPPORT_API void write(LogLevel level, const std::string& line) noexcept override {}

   ///<summary> Write message to log with newline.</summary>
   ///<remarks> writeln does nothing if the single bit level parameter is not set in the loggers current LogFilter bitmask.</remarks>
   ///<param name="level"> the LogLevel used to filter log messages.</param>
   ///<param name="line"> The message to log.</param>
   BASICUNIVERSALCPPSUPPORT_API void writeln(LogLevel level, const std::string& line) noexcept override {}

   ///<summary> Write exception to log (multi-line).</summary>
   ///<remarks> writeln does nothing if the single bit level parameter is not set in the loggers current LogFilter bitmask.</remarks>
   ///<param name="level"> the LogLevel used to filter log messages.</param>
   ///<param name="line">The message to log</param>
   BASICUNIVERSALCPPSUPPORT_API void write(LogLevel level, const std::exception& e) noexcept override {}

   ///<summary>Read the complete log file. </summary>
   ///<returns>The log file contents in raw bytes</returns>
   BASICUNIVERSALCPPSUPPORT_API std::string read_all() const noexcept override { return std::string(); }

   ///<summary>Clear log file.</summary>
   BASICUNIVERSALCPPSUPPORT_API void clear() noexcept override {}
};

#endif //__NULL_LOGGER_HPP__
