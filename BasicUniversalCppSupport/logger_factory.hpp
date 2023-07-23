//
// logger_factory.hpp : implements the concrete logger factory
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
#ifndef __LOGGER_FACTORY_HPP__
#define __LOGGER_FACTORY_HPP__

#ifdef BASICUNIVERSALCPPSUPPORT_EXPORTS
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include "logger_interface.hpp"
#include "file_logger.hpp"
#include "null_logger.hpp"

class BASICUNIVERSALCPPSUPPORT_API logger_factory
{
public:
   
   ///<summary>enumeration of the logger types supported.</summary>
   enum class logger_type : int
   {
      null_logger = 0,     // null (do nothing) waste of space
      file_logger = 1      // a thread-safe file based logger
   };
   
   ///<summary> getInstance - a static singleton is chosen so that we have exactly one logger.</summary>
   ///<remarks> the responsibility to create a logger early falls to the main program, but the logger
   /// created is then available to all dynamically linked components equally. If a component dll logs before
   /// main has created a file logger, then the whole program will fallback to default (null logging).</remarks>
   ///<param name='loggerType'> If this is the first call to getInstance, this type will be used to 
   /// factory construct a logger instance of this type. On subsequent calls the parameter is ignored.</param>
   ///<param name='filePath'> If this is the first call to getInstance, and type is file_logger, then this 
   /// string will be used as the name of the log file (otherwise the parameter is ignored).</param>
   ///<param name='logFilter'> If this is the first call to getInstance, and type is file_logger, then this 
   /// value will be used to select which messages will be logged, (otherwise the parameter is ignored).</param>
   ///<returns> a shared pointer to the singleton logger instance.</returns>
   static std::shared_ptr<logger_interface> getInstance(logger_type loggerType=logger_type::null_logger, const std::string& filePath="", LogFilter logFilter = LogFilter::None)
   {
      static std::shared_ptr<logger_interface> singleton = createLogger(loggerType, filePath, logFilter);
      return singleton;
   }

private:
 
   ///<summary> static createLogger.</summary>
   ///<param name='loggerType'> factory construct a logger instance of this type.</param>
   ///<returns> a shared pointer to the logger instance created.</returns>
   static std::shared_ptr<logger_interface> createLogger(logger_type loggerType, const std::string& filePath, LogFilter logFilter)
   {
      switch (loggerType)
      {
      case logger_type::file_logger:
         return std::make_shared<file_logger>(filePath, logFilter);
         
      case logger_type::null_logger:
      default:
         return std::make_shared<null_logger>();
     }
   }
};

#endif // __LOGGER_FACTORY_HPP__
