//
// file_logger.hpp : implements file logging
//
// Copyright (c) 2017 Jack Heeley, all rights reserved
//
#pragma once

#ifdef BASICUNIVERSALCPPSUPPORT_EXPORTS
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <string>
#include <fstream>

#include "logger.hpp"
#include "spimpl.hpp"

#pragma warning (disable:4251 4290)

///<summary>file logger for ansi c++11/utf8 code clients</summary>
class BASICUNIVERSALCPPSUPPORT_API file_logger : public logger
{
public:
   ///<summary> construct a default file_logger.</summary>
   file_logger();

   ///<summary>construct logger</summary>
   ///<param name='fileName'>path and name of the log file.</param>
   ///<param name='filter'>bitmask used to filter log write events.</param>
   ///<exception cref='std::exception'>if underlying ofstream could not be opened</exception>
   file_logger(std::string fileName, LogFilter filter);

   ///<summary> equals comparison operator.</summary>
   ///<remarks> defines equals to mean identical member content.</remarks>
   bool operator==(const file_logger& other) const;

   ///<summary> not equals comparison operator.</summary>
   ///<remarks> defines not equals to mean differing member content.</remarks>
   bool operator!=(const file_logger& other) const;

   ///<summary> used to determine which messages get logged. loggers compare the filter bitmask supplied
   /// here (or at construction time) with the single bit level supplied as parameter to write operations.</summary>
   ///<param name='filter'>bitmask used to filter log write events.</param>
   virtual void set_log_filter(LogFilter filter);

   ///<summary> get log filter.</summary>
   ///<returns>the current filter.</returns>
   virtual LogFilter get_log_filter();

   ///<summary> Write message to log without newline.</summary>
   ///<remarks> writeln does nothing if the single bit level parameter is not set in the loggers current LogFilter bitmask.</remarks>
   ///<param name="level"> the LogLevel used to filter log messages.</param>
   ///<param name="text"> The message to log.</param>
   virtual void write(LogLevel level, std::string line);

   ///<summary> Write message to log with newline.</summary>
   ///<remarks> writeln does nothing if the single bit level parameter is not set in the loggers current LogFilter bitmask.</remarks>
   ///<param name="level"> the LogLevel used to filter log messages.</param>
   ///<param name="line"> The message to log.</param>
   virtual void writeln(LogLevel level, std::string line);

   ///<summary> Write exception to log (multi-line).</summary>
   ///<remarks> writeln does nothing if the single bit level parameter is not set in the loggers current LogFilter bitmask.</remarks>
   ///<param name="level"> the LogLevel used to filter log messages.</param>
   ///<param name="line">The message to log</param>
   virtual void write(LogLevel level, std::exception e);

   ///<summary>Read the complete log file. </summary>
   ///<returns>The log file contents in raw bytes</returns>
   virtual std::string read_all();

   ///<summary>Clear log file.</summary>
   virtual void clear();

private:
   ///<summary> forward reference to private implementation.</summary>
   class impl;

   ///<summary> smart unique pointer to private implementation.</summary>
   ///<remarks> with default copy, move and compare support.</remarks>
   spimpl::impl_ptr<impl> impl_;
};

#pragma warning (default:4251 4290)