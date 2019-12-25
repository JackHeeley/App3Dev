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

#include "..\SampleProgram\thread_RAII.hpp"  // TODO: could be moved from program to basic support dll
#include <atomic>
#include <chrono>
#include <iomanip>
#include <thread>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace utf8;

using namespace std::chrono_literals;

namespace UnitTestBasicUniversalCppSupport
{
   TEST_CLASS(UnitTestFileLogger)
   {

   public:
      TEST_METHOD(TestErrorContext)
      {
         // Building 'line' (instead of hard coding) stops this test from breaking every time the source line numbers change
         std::string line("L");

         try
         {
            // perform the operation under test...
            line.append(S2(__LINE__)); throw error_context("we blew it"); //  // we look for line number e.g. "L46" in the exception full_what.
         }
         catch (const error::context & e)
         {
#pragma warning(disable:26489)
            const std::string what_string(e.full_what());
#pragma warning(default:26489)
            utf8::Assert::IsTrue(what_string.find("UnitTestFileLogger.cpp") != std::string::npos, "Didn't find __FILE__ in exception what().");
            utf8::Assert::IsTrue(what_string.find(line) != std::string::npos, "Didn't find __LINE__ in exception what().");
            utf8::Assert::IsTrue(what_string.find("TestErrorContext") != std::string::npos, "Didn't find __func__ in exception what().");
         }
      }

      TEST_METHOD(TestFileLoggerSimpleLogEntry)
      {
         try
         {
            // prepare for test...
            CREATE_LOGGER(logger_factory::type::file_logger, log_file_name, LogFilter::Full);

            // perform the operation under test...
            std::string log_text = "this is a log entry "; log_text.append(utc_timestamp()); // unique every run
            LOG_TRACE(log_text);

            // test succeeds if log_text with this unique timestamp attached, is found in the log...
            utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_text) == std::string::npos), "logged text was not found in log");
         }
         catch (const error::context & e)
         {
            utf8::Assert::IsTrue(false, e.full_what()); // something went wrong
         }
      }

      TEST_METHOD(TestFileLoggerIsCopyable)
      {
         try
         {
            // prepare for test...
            CREATE_LOGGER(logger_factory::type::file_logger, log_file_name, LogFilter::Full);
            std::string log_text_0 = "this is copy test log entry log_text_0 "; log_text_0.append(utc_timestamp()); // unique every run
            LOG_TRACE(log_text_0);

            // create another logger - contains a handle to same log file as used by default logging macros, but with a log filter that could vary independently...
            file_logger file_logger_duplicate(log_file_name, LogFilter::Full);

            // do the equivalent of "LOG_TRACE(log_text_1);" - but adjusted explicitly to use the duplicate logger (including its unique filter)
            std::string log_text_1 = "this is copy test log entry log_text_1 "; log_text_1.append(utc_timestamp()); // unique every run
            file_logger_duplicate.writeln(LogLevel::Trace, log_text_1);

            // perform the operation under test...
            file_logger moved_duplicate_logger = file_logger_duplicate; // copy will NOT invalidate file_logger_duplicate.

            // do the equivalent of "LOG_TRACE(log_text_2);" - but adjusted explicitly to use the moved duplicate logger (including its unique filter)
            std::string log_text_2 = "this is copy test log entry log_text_2 "; log_text_2.append(utc_timestamp()); // unique every run
            moved_duplicate_logger.writeln(LogLevel::Trace, log_text_2);

            // test succeeds if all three log_texts with their unique timestamps attached, are found in the log content (as fetched from both of the remaining valid loggers)...

            utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_text_0) == std::string::npos), "log_text_0 was not found in log");
            utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_text_1) == std::string::npos), "log_text_1 was not found in log");
            utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_text_2) == std::string::npos), "log_text_2 was not found in log");

            // after the duplicate has been copied from, it REMAINS a viable object!
            utf8::Assert::IsFalse((file_logger_duplicate.read_all().find(log_text_0) == std::string::npos), "log_text_0 was not found in duplicate log");
            utf8::Assert::IsFalse((file_logger_duplicate.read_all().find(log_text_1) == std::string::npos), "log_text_1 was not found in duplicate log");
            utf8::Assert::IsFalse((file_logger_duplicate.read_all().find(log_text_2) == std::string::npos), "log_text_2 was not found in duplicate log");

            utf8::Assert::IsFalse((moved_duplicate_logger.read_all().find(log_text_0) == std::string::npos), "log_text_0 was not found in moved log");
            utf8::Assert::IsFalse((moved_duplicate_logger.read_all().find(log_text_1) == std::string::npos), "log_text_1 was not found in moved log");
            utf8::Assert::IsFalse((moved_duplicate_logger.read_all().find(log_text_2) == std::string::npos), "log_text_2 was not found in moved log");
         }
         catch (const error::context & e)
         {
            utf8::Assert::IsTrue(false, e.full_what()); // something went wrong
         }
      }

      TEST_METHOD(TestFileLoggerIsMovable)
      {
         try
         {
            CREATE_LOGGER(logger_factory::type::file_logger, log_file_name, LogFilter::Full);
            // prepare for test...
            std::string log_text_0 = "this is move test log entry log_text_0 "; log_text_0.append(utc_timestamp()); // unique every run
            LOG_TRACE(log_text_0);

            // create another logger - contains a handle to same log file as used by default logging macros, but with a log filter that could vary independently...
            file_logger file_logger_duplicate(log_file_name, LogFilter::Full);

            // do the equivalent of "LOG_TRACE(log_text_1);" - but adjusted explicitly to use the duplicate logger (including its unique filter)
            std::string log_text_1 = "this is move test log entry log_text_1 "; log_text_1.append(utc_timestamp()); // unique every run
            file_logger_duplicate.writeln(LogLevel::Trace, log_text_1);

            // perform the operation under test...
            file_logger moved_duplicate_logger = std::move(file_logger_duplicate); // move will (correctly) invalidate file_logger_duplicate.

            // do the equivalent of "LOG_TRACE(log_text_2);" - but adjusted explicitly to use the moved duplicate logger (including its unique filter)
            std::string log_text_2 = "this is move test log entry log_text_2 "; log_text_2.append(utc_timestamp()); // unique every run
            moved_duplicate_logger.writeln(LogLevel::Trace, log_text_2);

            // test succeeds if all three log_texts with their unique timestamps attached, are found in the log content (as fetched from both of the remaining valid loggers)...

            utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_text_0) == std::string::npos), "log_text_0 was not found in log");
            utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_text_1) == std::string::npos), "log_text_1 was not found in log");
            utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_text_2) == std::string::npos), "log_text_2 was not found in log");

            //DUUUH - after the duplicate has been moved from, it is no longer a viable object!
            //utf8::Assert::IsFalse((file_logger_duplicate.read_all().find(log_text_0) == std::string::npos), "log_text_0 was not found in duplicate log");
            //utf8::Assert::IsFalse((file_logger_duplicate.read_all().find(log_text_1) == std::string::npos), "log_text_1 was not found in duplicate log");
            //utf8::Assert::IsFalse((file_logger_duplicate.read_all().find(log_text_2) == std::string::npos), "log_text_2 was not found in duplicate log");

            utf8::Assert::IsFalse((moved_duplicate_logger.read_all().find(log_text_0) == std::string::npos), "log_text_0 was not found in moved log");
            utf8::Assert::IsFalse((moved_duplicate_logger.read_all().find(log_text_1) == std::string::npos), "log_text_1 was not found in moved log");
            utf8::Assert::IsFalse((moved_duplicate_logger.read_all().find(log_text_2) == std::string::npos), "log_text_2 was not found in moved log");
         }
         catch (const error::context & e)
         {
            utf8::Assert::IsTrue(false, e.full_what()); // something went wrong
         }
      }

      TEST_METHOD(TestFileLoggerIsThreadsafe)
      {
         try
         {
            // prepare for test...
            CREATE_LOGGER(logger_factory::type::file_logger, log_file_name, LogFilter::Full);
            const std::string FILTERED_TEXT = "DON'T show this in the log!";

            std::atomic<int> progress = 0;

            auto log_progress = [&progress, &FILTERED_TEXT]
            {
               std::function<std::string(int)> progress_bar = [](int percent)
               {
                  std::stringstream bar;
                  bar << /*"\r" << */ "[";
                  for (int i = 0; i < 20; i++)
                  {
                     bar << ((i < percent / 5) ? u8"█" : " ");
                  }
                  bar << "]" << std::setw(3) << percent << "%";
                  return bar.str();
               };

               while (progress < 100)
               {
                  // test design is to use a different handle to same file and set a different filter value (just to challenge the coding)
                  // if threads shared a single logger they would compete over the filter resource setting... and thats what happening now.
                  std::this_thread::sleep_for(1ms);
                  LOG_TRACE(progress_bar(progress));                 // log (A LOT) in the thread (uses TRACE level)

                  std::string thread_output_suppressed("separate thread said: ");
                  thread_output_suppressed.append(FILTERED_TEXT);
                  LOG_WARNING(thread_output_suppressed);                // issue something that should be filtered
               }
               std::cout << progress_bar(progress) << std::endl;
            };

            if (TEST_LOG_LEVEL(LogLevel::Warning))
               TOGGLE_LOG_LEVEL(LogLevel::Warning);

            LOG_INFO("Launch the progress bar in a separate thread.");
            thread_RAII separate_thread(std::thread(log_progress), thread_RAII::DtorAction::detach);

            LOG_INFO("Do some simultaneous logging from the main thread.");
            for (progress = 0; progress < 100; progress++)
            {
               LOG_INFO("Logging in the main thread.");                    // log (a little) in the main thread
               std::this_thread::sleep_for(10ms);
            }

            std::string main_output_suppressed("Main thread said : ");
            main_output_suppressed.append(FILTERED_TEXT);
            LOG_WARNING(main_output_suppressed);                             // issue something that should be filtered

            Expects(progress == 100);  // if not, program would deadlock at join

            LOG_INFO("Block until progress bar is finished.");
            separate_thread.get().join();

            std::string log_text = "this is a log entry "; log_text.append(utc_timestamp()); // unique every run
            LOG_INFO(log_text);
            utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_text) == std::string::npos), "logged text was not found in log");

            utf8::Assert::IsTrue((LOG_FILE_CONTENTS.find(FILTERED_TEXT) == std::string::npos), "filtered text was found in log");

            // full success assessment (currently) needs human intervention...
            std::cout << "Check '" << log_file_name << "' entries are interleaved (line by line) without any cross-talk/garbling." << std::endl;
         }
         catch (const error::context & e)
         {
            utf8::Assert::IsTrue(false, e.full_what()); // something went wrong
         }
      }
   };
}
