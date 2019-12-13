//
// UnitTestFileLogger.cpp : a utf8 everywhere component unit test 
//
// Copyright (c) 2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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
#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace utf8;

namespace UnitTestBasicUniversalCppSupport
{
   TEST_CLASS(UnitTestFileLogger)
   {
   public:

      TEST_METHOD(TestErrorContext)
      {
         try
         {
            // perform the operation under test...
            throw error_context("we blew it");
         }
         catch (const error::context& e)
         {
#pragma warning(disable:26489)
            const std::string what_string(e.full_what());
#pragma warning(default:26489)
            utf8::Assert::IsTrue(what_string.find("UnitTestFileLogger.cpp") != std::string::npos, "Didn't find __FILE__ in exception what().");
            utf8::Assert::IsTrue(what_string.find("L35") != std::string::npos, "Didn't find __LINE__ in exception what().");
            utf8::Assert::IsTrue(what_string.find("TestErrorContext") != std::string::npos, "Didn't find __func__ in exception what().");
         }
      }

      TEST_METHOD(TestFileLoggerSimpleLogEntry)
      {
         try
         {
            // prepare for test...
            const std::string log_file_name = "logfile.log";
            file_logger Logger(log_file_name, LogFilter::Full);

            // perform the operation under test...
            std::string log_text = "this is a log entry "; log_text.append(utc_timestamp()); // unique every run
            LOG_TRACE(log_text);

            // test succeeds if log_text with this unique timestamp attached, is found in the log...
            utf8::Assert::IsFalse((Logger.read_all().find(log_text) == std::string::npos), "logged text was not found in log");
         }
         catch (const error::context& e)
         {
            utf8::Assert::IsTrue(false, e.full_what()); // something went wrong
         }
      }

   };
}