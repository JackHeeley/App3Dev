//
// logger_factory.hpp : implements the concrete logger factory
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

#include <string>

#include "logger.hpp"
#include "default_logger.hpp"
#include "file_logger.hpp"

class BASICUNIVERSALCPPSUPPORT_API logger_factory
{
public:
   
   ///<summary>enumeration of the logger types supported.</summary>
   enum class type : int
   {
      default_logger = 0,  // null (do nothing) waste of space
      file_logger = 1      // a thread-safe file based logger
   };
   
   ///<summary> getInstance - a static singleton is chosen so that we have exactly one logger.</summary>
   ///<remarks> the responsibility to create a logger early falls to the main program, but the logger
   /// created is then available to all dynamically linked components equally. If a component dll logs before
   /// main has created a file logger, then the whole program will fallback to default (null logging).</remarks>
   ///<param name='logger_type'> If this is the first call to getInstance, this type will be used to 
   /// factory construct a logger instance of this type. On subsequent calls the parameter is ignored.</param>
   ///<param name='filePath'> If this is the first call to getInstance, and type is file_logger, then this 
   /// string will be used as the name of the log file (otherwise the parameter is ignored).</param>
   ///<param name='logFilter'> If this is the first call to getInstance, and type is file_logger, then this 
   /// value will be used to select which messages will be logged, (otherwise the parameter is ignored).</param>
   ///<returns> a shared pointer to the singleton logger instance.</returns>
   static std::shared_ptr<logger> getInstance(type logger_type=type::default_logger, std::string filePath="", LogFilter logFilter = LogFilter::None)
   {
      static std::shared_ptr<logger> singleton = createLogger(logger_type, filePath, logFilter);
      return singleton;
   }

private:
 
   ///<summary> static createLogger.</summary>
   ///<param name='logger_type'> factory construct a logger instance of this type.</param>
   ///<returns> a shared pointer to the logger instance created.</returns>
   static std::shared_ptr<logger> createLogger(type logger_type, std::string filePath, LogFilter logFilter)
   {
      switch (logger_type)
      {
      case type::file_logger:
         return std::make_shared<file_logger>(filePath, logFilter);
         
      case type::default_logger:
      default:
         return std::make_shared<default_logger>();
     }
   }
};