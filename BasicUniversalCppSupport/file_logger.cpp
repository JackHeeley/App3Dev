//
// file_logger.hpp : implements file logging
//
// Copyright (c) 2017 Jack Heeley, all rights reserved
//

#include "stdafx.h"

/*
* ***************************************************************************
* PIMPL idiom - private implementation of file_logger class (Rule of 5)
* ***************************************************************************
*/

///<summary> the private implementation of file_logger.</summary>
///<remarks> Windows types used internally, adheres to "utf8 everywhere" paradigm at public interface</remarks>
class file_logger::impl : logger
{
private:
   LogFilter filter;
   std::string fileName;
   std::ofstream stream;

public:
#pragma warning (disable:26439)
   impl() /*noexcept*/ :
#pragma warning (default:26439)
      fileName("LogFile.log"),
      filter(LogFilter::None),
      stream(std::ofstream(fileName, std::ofstream::out | std::ofstream::app))
   {
   };

   impl(const std::string fileName, LogFilter filter) /*noexcept*/ :
      fileName(fileName),
      filter(filter),
      stream(std::ofstream(fileName, std::ofstream::out | std::ofstream::app))
   {
   };

   ///<summary> copy constructor.</summary>
   impl(const impl& other) :
      fileName(other.fileName),
      filter(other.filter),
      stream(std::ofstream(fileName, std::ofstream::out | std::ofstream::app))
   {
   }

   ///<summary> move constructor.</summary>
#pragma warning (disable:26439)
   impl(impl&& other) /*noexcept*/ :
#pragma warning (default:26439)
      fileName(other.fileName),
      filter(other.filter),
      stream(std::ofstream(fileName, std::ofstream::out | std::ofstream::app))
   {
   }

   ///<summary> destructor.</summary>
   ~impl() noexcept
   {
      try
      {
#pragma warning (disable:26447)
         clear();
#pragma warning (default:26447)
      }
      catch (std::exception&)
      {
         // catch and dismiss is least bad option
      }
   }
   
   ///<summary> copy assignment operator.</summary>
   impl& impl::operator=(impl& other)
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
#pragma warning (disable:26439)
   impl& impl::operator=(impl&& other) /*noexcept*/
#pragma warning (default:26439)

   {
      if (this != &other)
      {
         fileName = other.fileName;
         filter = other.filter;
         stream = std::ofstream(fileName, std::ofstream::out | std::ofstream::app);
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

   void set_log_filter(LogFilter filter) noexcept override
   {
      this->filter = filter;
   }

   LogFilter get_log_filter() const noexcept override
   {
      return this->filter;
   }

   void write(LogLevel level, std::string line) override
   {
      if (!(static_cast<int>(filter) /*bitwise*/& static_cast<int>(level)))
         return;

      stream << log_level(level) << ": " << utc_timestamp() << " " << line;
   }

   void writeln(LogLevel level, std::string line) override
   {
      if (!(static_cast<int>(filter) /*bitwise*/& static_cast<int>(level)))
         return;

      stream << log_level(level) << ": " << utc_timestamp() << " " << line << std::endl;
   }

   void write(LogLevel level, std::exception e) override
   {
      //TODO: stack trace 
      writeln(level, e.what());
   }

   std::string read_all() override
   {
      std::ifstream t = std::ifstream(fileName);
      std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
      return str;
   }

   void clear() override
   {
       stream.clear();
   }
};

/*
* ***************************************************************************
* PIMPL idiom - public interface for SystemError implementation (Rule of 0)
* ***************************************************************************
*/

///<summary> constructs a default file_logger.</summary>
file_logger::file_logger() noexcept :
   impl_(spimpl::make_impl<impl>())
{
}

file_logger::file_logger(std::string fileName, LogFilter filter) :
   impl_(spimpl::make_impl<impl>(fileName, filter))
{
}

///<summary> equals comparison operator.</summary>
///<remarks> defines equals to mean identical fileName members.</remarks>
bool file_logger::operator==(const file_logger& other) const noexcept
{
   return *impl_.get() == *other.impl_.get();
}

///<summary> not equals comparison operator.</summary>
///<remarks> defines not equals to mean differing fileName members.</remarks>
bool file_logger::operator!=(const file_logger& other) const noexcept
{
   return !(*this == other);
}

void file_logger::set_log_filter(LogFilter filter) noexcept
{
   impl_->set_log_filter(filter);
}

LogFilter file_logger::get_log_filter() const noexcept
{
   return impl_->get_log_filter();
}

void file_logger::write(LogLevel level, std::string line)
{
   return impl_->write(level, line);
}

void file_logger::writeln(LogLevel level, std::string line)
{
   return impl_->writeln(level, line);
}

void file_logger::write(LogLevel level, std::exception e)
{
   return impl_->write(level, e);
}

std::string file_logger::read_all()
{
   return impl_->read_all();
}

void file_logger::clear()
{
   impl_->clear();
}

