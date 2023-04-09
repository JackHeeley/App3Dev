//
// file_logger.cpp : implements file logging
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
#include "stdafx.h"

#include <fstream>
#include <mutex>

#define FILE_LOGGER_WARNINGS_SUPRESSED 26447
#pragma warning (disable: FILE_LOGGER_WARNINGS_SUPRESSED)

/*
* ***************************************************************************
* PIMPL idiom - private implementation of file_logger class (Rule of 5)
* ***************************************************************************
*/

///<summary> the private implementation of file_logger.</summary>
///<remarks> Windows types used internally, adheres to "utf8 everywhere" paradigm at public interface</remarks>
class file_logger::impl : logger_interface
{
private:
   LogFilter filter;
   std::string fileName;
   std::ofstream stream;
   std::mutex the_mutex;

public:
   ///<summary> default constructor.</summary>
   impl() noexcept :
      filter(LogFilter::None),
      fileName("LogFile.log"),
      stream(std::ofstream(fileName, std::ofstream::out | std::ofstream::app)),
      the_mutex()
   {
   };

   ///<summary> normal constructor.</summary>
   impl(const std::string fileName, LogFilter filter) noexcept :
      filter(filter),
      fileName(fileName),
      stream(std::ofstream(fileName, std::ofstream::out | std::ofstream::app)),
      the_mutex()
   {
   };

   ///<summary> copy constructor.</summary>
   impl(const impl& other) noexcept :
      filter(other.filter),
      fileName(other.fileName),
      stream(std::ofstream(fileName, std::ofstream::out | std::ofstream::app)),
      the_mutex()
   {
   }

   ///<summary> move constructor.</summary>
   impl(impl&& other) noexcept :
      filter(other.filter),
      fileName(other.fileName),
      stream(),
      the_mutex()
   {
      stream.swap(other.stream);
   }
   
   ///<summary> destructor.</summary>
   ~impl() noexcept
   {
      try
      {
         stream.clear();
      }
      catch (const std::exception& e)
      {
         LOG_WARNING(e.what());
      }
   }
   
   ///<summary> copy assignment operator.</summary>
   impl& operator=(impl& other)
   {
      if (this != &other)
      {
         fileName = other.fileName;
         filter = other.filter;
         stream = std::ofstream(fileName, std::ofstream::out | std::ofstream::app);
      }
      return (*this);
   }

   ///<summary> move assignment operator.</summary>
   impl& operator=(impl&& other) noexcept
   {
      if (this != &other)
      {
         fileName = std::move(other.fileName);
         filter = std::move(other.filter);
         stream.swap(other.stream); 
      }
      return (*this);
   }

   ///<summary> equals comparison operator.</summary>
   ///<remarks> defines equals to mean identical fileName member content.</remarks>
   bool operator==(const impl& other) const noexcept
   {
      return fileName == other.fileName;
   }

   ///<summary> not equals comparison operator.</summary>
   ///<remarks> defines not equals to mean differing fileName member content.</remarks>
   bool operator!=(const impl& other) const noexcept
   {
      return !(*this == other);
   }

   ///<summary> set log filter.</summary>
   void set_log_filter(LogFilter filter) noexcept override
   {
      this->filter = filter;
   }

   ///<summary> get log filter.</summary>
   LogFilter get_log_filter() const noexcept override
   {
      return this->filter;
   }

   ///<summary> write (text).</summary>
   void write(LogLevel level, const std::string& line) override
   {
      std::lock_guard<std::mutex> lock(the_mutex);
      stream << log_level(level) << ": " << utc_timestamp() << " " << line;
   }

   ///<summary> writeln.</summary>
   void writeln(LogLevel level, const std::string& line) override
   {
      std::lock_guard<std::mutex> lock(the_mutex);
      stream << log_level(level) << ": " << utc_timestamp() << " " << line << std::endl;
   }

   ///<summary> write (exception).</summary>
   void write(LogLevel level, const std::exception& e) override
   {
      //TODO: stack trace 
      writeln(level, e.what());
   }

   ///<summary> read_all.</summary>
   std::string read_all() const override
   {
      std::ifstream t = std::ifstream(fileName);
      std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
      return str;
   }

   ///<summary> clear.</summary>
   void clear() override
   {
       stream.clear();
   }
};

/*
* ***************************************************************************
* PIMPL idiom - public interface for file_logger implementation (Rule of 0)
* ***************************************************************************
*/

///<summary> default constructor.</summary>
file_logger::file_logger() noexcept :
   pimpl(spimpl::make_impl<impl>())
{
}

///<summary> normal constructor for a file_logger.</summary>
file_logger::file_logger(const std::string& fileName, LogFilter filter) noexcept :
   pimpl(spimpl::make_impl<impl>(fileName, filter))
{
}

///<summary> equals comparison operator.</summary>
///<remarks> defines equals to mean identical fileName members.</remarks>
bool file_logger::operator==(const file_logger& other) const noexcept
{
   return *pimpl.get() == *other.pimpl.get();
}

///<summary> not equals comparison operator.</summary>
///<remarks> defines not equals to mean differing fileName members.</remarks>
bool file_logger::operator!=(const file_logger& other) const noexcept
{
   return !(*this == other);
}

///<summary> set log filter.</summary>
void file_logger::set_log_filter(LogFilter filter) noexcept
{
   pimpl->set_log_filter(filter);
}

///<summary> get log filter.</summary>
LogFilter file_logger::get_log_filter() const noexcept
{
   return pimpl->get_log_filter();
}

///<summary> write (text).</summary>
void file_logger::write(LogLevel level, const std::string& line) 
{
   return pimpl->write(level, line);
}

///<summary> writeln.</summary>
void file_logger::writeln(LogLevel level, const std::string& line)
{
   return pimpl->writeln(level, line);
}

///<summary> write (exception).</summary>
void file_logger::write(LogLevel level, const std::exception& e)
{
   return pimpl->write(level, e);
}

///<summary> read all.</summary>
std::string file_logger::read_all() const
{
   return pimpl->read_all();
}

///<summary> clear.</summary>
void file_logger::clear()
{
   pimpl->clear();
}

#pragma warning (default: FILE_LOGGER_WARNINGS_SUPRESSED)
