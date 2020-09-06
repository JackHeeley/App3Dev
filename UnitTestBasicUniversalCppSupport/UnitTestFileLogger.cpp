//
// UnitTestFileLogger.cpp : a utf8 everywhere component unit test 
//
// Copyright (c) 2019-2020 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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

#include <atomic>
#include <chrono>
#include <iomanip>
#include <thread>
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace utf8;

using namespace std::chrono_literals;

namespace UnitTestBasicUniversalCppSupport
{

   TEST_CLASS(UnitTestFileLogger)
   {

   private:
      // ensure log filter is restored on all test paths
      class RAII_Preserve_LogFilter
      {
      private:
         LogFilter m_saved_filter;

      public:
         RAII_Preserve_LogFilter() noexcept
         {
            try
            {
#pragma warning (disable: 26486)
               m_saved_filter = logger_factory::getInstance()->get_log_filter();
#pragma warning (default: 26486)
            }
            catch (...)
            {
               LOG_ERROR("RAII_Preserve_LogFilter constructor error!");
            }
         }

         RAII_Preserve_LogFilter(const RAII_Preserve_LogFilter& other) = delete;
         RAII_Preserve_LogFilter(RAII_Preserve_LogFilter&& other) noexcept = delete;
         RAII_Preserve_LogFilter& operator=(RAII_Preserve_LogFilter& other) = delete;
         RAII_Preserve_LogFilter& operator=(RAII_Preserve_LogFilter&& other) = delete;

         ~RAII_Preserve_LogFilter() noexcept
         {
            try
            {
#pragma warning (disable: 26486)
               logger_factory::getInstance()->set_log_filter(m_saved_filter);
#pragma warning (default: 26486)
            }
            catch (...)
            {
               LOG_ERROR("RAII_Preserve_LogFilter destructor error!");
            }
         }
      };

   public:

#pragma warning(disable: 26440 26477 26497)
      TEST_CLASS_INITIALIZE(InitializeUnitTestFileLogger) noexcept
#pragma warning(default: 26440 26477 26497)
      {
         try
         {
            CREATE_LOGGER(logger_factory::logger_type::file_logger, log_file_name, DEFAULT_LOG_FILTER);
         }
         catch (...)
         {
            LOG_ERROR("Couldn't create logger.");     // No logger? This will emit on std::cerr
         }
      }

      TEST_METHOD(TestGetShortFile)
      {
         // we expect consistent seperator characters (e.g. as __FILE__ macro will present to loggers) ...
         utf8::Assert::AreEqual("file_a.ext", get_short_file("c:\\dir\\subdir\\file_a.ext"));
         utf8::Assert::AreEqual("file_b.ext", get_short_file("c:/dir/subdir/file_b.ext"));
         utf8::Assert::AreEqual("file_c.ext", get_short_file("file_c.ext"));
         utf8::Assert::AreEqual("file_d.ext", get_short_file("c:\\dir\\subdir\\subsubdir\\subsubsubdir\\file_d.ext"));
       
         // and we support mixed separators for completeness (there is no real trade-off with constexpr)...
         utf8::Assert::AreEqual("file_e.ext", get_short_file("c:\\dir/subdir\\file_e.ext"));
         utf8::Assert::AreEqual("file_f.ext", get_short_file("c:/dir/subdir\\file_f.ext"));
         utf8::Assert::AreEqual("file_g.ext", get_short_file("c:/dir\\subdir/file_g.ext"));
         utf8::Assert::AreEqual("file_h.ext", get_short_file("c:\\dir\\subdir/file_h.ext"));

         // we aren't validating paths, and testing suspect edge- and special- cases is always good practice...
         utf8::Assert::AreEqual("file_i.ext", get_short_file("c:\\dir\\subdir////////////file_i.ext"));
         utf8::Assert::AreEqual("file_j.ext", get_short_file("////////////////file_j.ext"));
         utf8::Assert::AreEqual("file_k.ext", get_short_file("\\\\\\\\\\\\\\\\file_k.ext"));
         utf8::Assert::AreEqual(u8"φιλε_λ.εχτ", get_short_file(u8"\\\\\\\\\\\\\\\\φιλε_λ.εχτ"));
      }

      TEST_METHOD(TestErrorContext)
      {
         // Building 'line' prevents this test from breaking every time unit test changes affect the line number tested here
         std::string line = "";

         try
         {
            // perform the operation under test...
            line.insert(0, "(" TO_STRING_LITERAL(__LINE__) ")"); throw error_context("we blew it"); // prepare line number and throw (on same line)
         }
         catch (const error::context& e)
         {
#pragma warning(disable:26489) // warning C26489: Don't dereference a pointer that may be invalid
            const std::string what_string(e.full_what());
            utf8::Assert::IsTrue(what_string.find("UnitTestFileLogger.cpp") != std::string::npos, "Didn't find __FILE__ in exception what().");
            utf8::Assert::IsTrue(what_string.find(line) != std::string::npos, "Didn't find (throw) __LINE__ in exception what().");
            utf8::Assert::IsTrue(what_string.find(__FUNCTION__) != std::string::npos, "Didn't find __FUNCTION__ in exception what().");
#pragma warning(default:26489)
         }
      }

      TEST_METHOD(TestFileLoggerSimpleLogEntry)
      {
         try
         {
            // prepare for test...
            std::string log_text = "this is a log entry "; log_text.append(utc_timestamp()); // unique every run

            // perform the operation under test...
            LOG_TRACE(log_text);

            // test succeeds if log_text with this unique timestamp attached, is found in the log...
            utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_text) == std::string::npos), "logged text was not found in log");
         }
         catch (const error::context & e)
         {
#pragma warning(disable:26489) // warning C26489: Don't dereference a pointer that may be invalid
            utf8::Assert::Fail(e.full_what()); // something went wrong
#pragma warning(default:26489)
         }
      }


      TEST_METHOD(TestFileLoggerFilteringBasic)
      {
         try
         {
            // prepare for test...
            std::string stamp = utc_timestamp();
            std::string log_none_text = "TestFileLoggerFilteringBasic none entry "; log_none_text.append(stamp); // unique every run
            std::string log_trace_text = "TestFileLoggerFilteringBasic trace entry "; log_trace_text.append(stamp); // unique every run
            std::string log_debug_text = "TestFileLoggerFilteringBasic debug entry "; log_debug_text.append(stamp); // unique every run
            std::string log_info_text = "TestFileLoggerFilteringBasic info entry "; log_info_text.append(stamp); // unique every run
            std::string log_warning_text = "TestFileLoggerFilteringBasic warning entry "; log_warning_text.append(stamp); // unique every run
            std::string log_error_text = "TestFileLoggerFilteringBasic error entry "; log_error_text.append(stamp); // unique every run

            // perform the operation under test...
            LOG_NONE(log_none_text);
            LOG_TRACE(log_trace_text);
            LOG_DEBUG(log_debug_text);
            LOG_INFO(log_info_text);
            LOG_WARNING(log_warning_text);
            LOG_ERROR(log_error_text);

            // for ENABLED  LogLevels test succeeds if log_xxx_text with this unique timestamp attached, is found in the log...
            // for DISABLED LogLevels test succeeds if log_xxx_text with this unique timestamp attached, is not found in the log...

            if (TEST_LOG_LEVEL(LogLevel::None))
            {
               utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_none_text) == std::string::npos), "expected log_none_text was not found in log");
            }
            else
            {
               utf8::Assert::IsTrue((LOG_FILE_CONTENTS.find(log_none_text) == std::string::npos), "unexpected log_none_text was found in log");
            }

            if (TEST_LOG_LEVEL(LogLevel::Trace))
            {
               utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_trace_text) == std::string::npos), "expected log_trace_text was not found in log");
            }
            else
            {
               utf8::Assert::IsTrue((LOG_FILE_CONTENTS.find(log_trace_text) == std::string::npos), "unexpected log_trace_text was found in log");
            }


            if (TEST_LOG_LEVEL(LogLevel::Debug))
            {
               utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_debug_text) == std::string::npos), "expected log_debug_text was not found in log");
            }
            else
            {
               utf8::Assert::IsTrue((LOG_FILE_CONTENTS.find(log_debug_text) == std::string::npos), "unexpected log_debug_text was found in log");
            }

            if (TEST_LOG_LEVEL(LogLevel::Info))
            {
               utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_info_text) == std::string::npos), "expected log_info_text was not found in log");
            }
            else
            {
               utf8::Assert::IsTrue((LOG_FILE_CONTENTS.find(log_info_text) == std::string::npos), "unexpected log_info_text was found in log");
            }

            if (TEST_LOG_LEVEL(LogLevel::Warning))
            {
               utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_warning_text) == std::string::npos), "expected log_warning_text was not found in log");
            }
            else
            {
               utf8::Assert::IsTrue((LOG_FILE_CONTENTS.find(log_warning_text) == std::string::npos), "unexpected log_warning_text was found in log");
            }

            if (TEST_LOG_LEVEL(LogLevel::Error))
            {
               utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_error_text) == std::string::npos), "expected log_error_text was not found in log");
            }
            else
            {
               utf8::Assert::IsTrue((LOG_FILE_CONTENTS.find(log_error_text) == std::string::npos), "unexpected log_error_text was found in log");
            }

         }
         catch (const error::context & e)
         {
            utf8::Assert::Fail(e.full_what()); // something went wrong
         }
      }


#ifdef STATIC_LOG_FILTERING
#pragma warning(disable: 26477 26485)
      BEGIN_TEST_METHOD_ATTRIBUTE(TestFileLoggerFilteringAdvanced)
         TEST_IGNORE()        // TestFileLoggerFilteringAdvanced Static logging can't be changed at runtime (TOGGLE_LOG_LEVEL has no effect)
         END_TEST_METHOD_ATTRIBUTE()
#pragma warning(default: 26477 26485)
#endif
      TEST_METHOD(TestFileLoggerFilteringAdvanced)
      {
         try
         {
            // prepare for test...
            RAII_Preserve_LogFilter saveFilter;

            std::string stamp = utc_timestamp();
            std::string log_none_text = "TestFileLoggerFilteringAdvanced none entry "; log_none_text.append(stamp); // unique every run
            std::string log_trace_text = "TestFileLoggerFilteringAdvanced trace entry "; log_trace_text.append(stamp); // unique every run
            std::string log_debug_text = "TestFileLoggerFilteringAdvanced debug entry "; log_debug_text.append(stamp); // unique every run
            std::string log_info_text = "TestFileLoggerFilteringAdvanced info entry "; log_info_text.append(stamp); // unique every run
            std::string log_warning_text = "TestFileLoggerFilteringAdvanced warning entry "; log_warning_text.append(stamp); // unique every run
            std::string log_error_text = "TestFileLoggerFilteringAdvanced error entry "; log_error_text.append(stamp); // unique every run

            TOGGLE_LOG_LEVEL(LogLevel::None);
            TOGGLE_LOG_LEVEL(LogLevel::Trace);
            TOGGLE_LOG_LEVEL(LogLevel::Debug);
            TOGGLE_LOG_LEVEL(LogLevel::Info);
            TOGGLE_LOG_LEVEL(LogLevel::Warning);
            TOGGLE_LOG_LEVEL(LogLevel::Error);

            // perform the operation under test...
            LOG_NONE(log_none_text);
            LOG_TRACE(log_trace_text);
            LOG_DEBUG(log_debug_text);
            LOG_INFO(log_info_text);
            LOG_WARNING(log_warning_text);
            LOG_ERROR(log_error_text);

            // for ENABLED  LogLevels test succeeds if log_xxx_text with this unique timestamp attached, is found in the log...
            // for DISABLED LogLevels test succeeds if log_xxx_text with this unique timestamp attached, is not found in the log...

            std::string log_file_contents = LOG_FILE_CONTENTS;

            if (TEST_LOG_LEVEL(LogLevel::None))
            {
               utf8::Assert::IsFalse((log_file_contents.find(log_none_text) == std::string::npos), "expected log_none_text was not found in log");
            }
            else
            {
               utf8::Assert::IsTrue((log_file_contents.find(log_none_text) == std::string::npos), "unexpected log_none_text was found in log");
            }

            if (TEST_LOG_LEVEL(LogLevel::Trace))
            {
               utf8::Assert::IsFalse((log_file_contents.find(log_trace_text) == std::string::npos), "expected log_trace_text was not found in log");
            }
            else
            {
               utf8::Assert::IsTrue((log_file_contents.find(log_trace_text) == std::string::npos), "unexpected log_trace_text was found in log");
            }


            if (TEST_LOG_LEVEL(LogLevel::Debug))
            {
               utf8::Assert::IsFalse((log_file_contents.find(log_debug_text) == std::string::npos), "expected log_debug_text was not found in log");
            }
            else
            {
               utf8::Assert::IsTrue((log_file_contents.find(log_debug_text) == std::string::npos), "unexpected log_debug_text was found in log");
            }

            if (TEST_LOG_LEVEL(LogLevel::Info))
            {
               utf8::Assert::IsFalse((log_file_contents.find(log_info_text) == std::string::npos), "expected log_info_text was not found in log");
            }
            else
            {
               utf8::Assert::IsTrue((log_file_contents.find(log_info_text) == std::string::npos), "unexpected log_info_text was found in log");
            }

            if (TEST_LOG_LEVEL(LogLevel::Warning))
            {
               utf8::Assert::IsFalse((log_file_contents.find(log_warning_text) == std::string::npos), "expected log_warning_text was not found in log");
            }
            else
            {
               utf8::Assert::IsTrue((log_file_contents.find(log_warning_text) == std::string::npos), "unexpected log_warning_text was found in log");
            }

            if (TEST_LOG_LEVEL(LogLevel::Error))
            {
               utf8::Assert::IsFalse((log_file_contents.find(log_error_text) == std::string::npos), "expected log_error_text was not found in log");
            }
            else
            {
               utf8::Assert::IsTrue((log_file_contents.find(log_error_text) == std::string::npos), "unexpected log_error_text was found in log");
            }

         }
         catch (const error::context & e)
         {
#pragma warning(disable:26489)
            utf8::Assert::Fail(e.full_what()); // something went wrong
#pragma warning(default:26489)
         }
      }

      TEST_METHOD(TestFileLoggerIsComparable)
      {
         try
         {
            // create another logger - contains a handle to same log file as used by default logging macros, but with a log filter that could vary independently...
            file_logger file_logger_duplicate(log_file_name, LogFilter::Full);
            file_logger copied_duplicate_logger = file_logger_duplicate; // copy will NOT invalidate file_logger_duplicate.

            // test succeeds if duplicate and copy are considered equal...
            utf8::Assert::IsTrue((copied_duplicate_logger == file_logger_duplicate), "the two log copies were not considered equal");
            utf8::Assert::IsFalse((copied_duplicate_logger != file_logger_duplicate), "the two log copies were considered inequal");

            file_logger moved_duplicate_logger = std::move(file_logger_duplicate); // move will (correctly) invalidate file_logger_duplicate.

            //... and after a move, the copied duplicate and moved duplicate are still considered equal.
            utf8::Assert::IsTrue((copied_duplicate_logger == moved_duplicate_logger), "the copied and moved log objects were not considered equal");
            utf8::Assert::IsFalse((copied_duplicate_logger != moved_duplicate_logger), "the copied and moved log objects were considered inequal");
         }
         catch (const error::context& e)
         {
            utf8::Assert::Fail(e.full_what()); // something went wrong
         }
      }

      TEST_METHOD(TestFileLoggerIsCopyable)
      {
         try
         {
            // prepare for test...
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
            utf8::Assert::Fail(e.full_what()); // something went wrong
         }
      }

      TEST_METHOD(TestFileLoggerIsMovable)
      {
         try
         {
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
            RAII_Preserve_LogFilter saveFilter;  

#ifdef _DEBUG
            std::string FILTERED_TEXT = "EXPECT to see this in the (debug) logs!"; // if using debug build static default log filtering 
#else
            std::string FILTERED_TEXT = "DON'T show this in the (release) logs!"; // if using release build static default log filtering 
#endif
            FILTERED_TEXT.append(utc_timestamp());
            std::atomic<int> progress = 0;
            
            // 'borrow' a well known code snippet from main program (and add some logging)...
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
                  std::this_thread::sleep_for(1ms);
                  LOG_TRACE(progress_bar(progress));                 // log (A LOT) in the thread (uses TRACE level)

                  std::string thread_output_suppressed("separate thread said: ");
                  thread_output_suppressed.append(FILTERED_TEXT);
                  LOG_DEBUG(thread_output_suppressed);             // issue something that should be filtered out (if using release build static default log filter)
               }
               std::cout << progress_bar(progress) << std::endl;
            };

            // Logger is currently single instanced, so next block affects logging in both threads (not optimal)
#ifndef STATIC_LOG_FILTERING
            // Dynamic filtering test is inappropriate in this context
            if (TEST_LOG_LEVEL(LogLevel::Debug))
            {
               TOGGLE_LOG_LEVEL(LogLevel::Debug);
            }
#endif // !STATIC_LOG_FILTERING

            LOG_TRACE("Launch the progress bar in a separate thread.");
            RAII_thread separate_thread(std::thread(log_progress), RAII_thread::DtorAction::detach);

            LOG_TRACE("Do some simultaneous logging from the main thread.");
            for (progress = 0; progress < 100; progress++)
            {
               LOG_TRACE("Logging in the main thread.");                    // log (a little) in the main thread
               std::this_thread::sleep_for(10ms);
            }

            std::string main_output_suppressed("Main thread said : ");
            main_output_suppressed.append(FILTERED_TEXT);
            LOG_DEBUG(main_output_suppressed);                             // issue something that should be filtered

            Expects(progress == 100);  // if not, program would deadlock at join

            LOG_TRACE("Block until progress bar is finished.");
            separate_thread.get().join();

            std::string log_text = "this is a log entry "; log_text.append(utc_timestamp()); // unique every run
            LOG_TRACE(log_text);
            utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_text) == std::string::npos), "logged text was not found in log");

#ifndef STATIC_LOG_FILTERING
            // Dynamic filtering test is inappropriate in this context
            utf8::Assert::IsTrue((LOG_FILE_CONTENTS.find(FILTERED_TEXT) == std::string::npos), "filtered text was found in log");
#endif // !STATIC_LOG_FILTERING

            // full success assessment (currently) needs human intervention...
            std::cout << "Check '" << log_file_name << "' entries are interleaved (line by line) without any cross-talk/garbling." << std::endl;
         }
         catch (const error::context & e)
         {
            utf8::Assert::IsTrue(false, e.full_what()); // something went wrong
         }
      }

      TEST_METHOD(TestLogAndExceptionDecoration)
      {
         // check preconditions & prepare
         std::string file = (__SHORT_FILE__);

         utf8::Assert::AreEqual("UnitTestFileLogger.cpp", file.c_str(), "Unexpected filename."); // also confirms __SHORT_FILE__
         std::string expected_throw_line_number("");   // built (not hard coded) to provide resilience (source line numbers change frequently)

         std::string log_text = "This is a LOG_DEBUG test"; log_text.append(utc_timestamp()); // unique every run

         // perform the operation under test (First invoke all logging forms)...
         try
         {
            LOG_TRACE("This is a LOG_TRACE test");       
            LOG_DEBUG(log_text);                       // special case (LOG_DEBUG filtered by other tests) but should have been restored
            LOG_INFO("This is a LOG_INFO test");
            LOG_WARNING("This is a LOG_WARNING test");
            LOG_ERROR("This is a LOG_ERROR test");
         }
         catch (...)
         {
            // test fails if 'noexcept' code throws...
            utf8::Assert::IsTrue(false, "Exception thrown by logging.");
         }

#ifdef _DEBUG
         // test partially succeeds if log_text with this unique timestamp attached, is found in the log (true only in DEBUG builds)...
         utf8::Assert::IsFalse((LOG_FILE_CONTENTS.find(log_text) == std::string::npos), "DEBUG filtering is active (check other unit tests) ");
#endif // DEBUG

         // test FULLY succeeds if log file now has entries that are correctly decorated (according to the build rules for decoration).

         // We could automate a test here but we are satisfied to emit this reminder...
         std::cout << "Please manually inspect the decoration of log entries in the log." << std::endl;

         // perform the operation under test (also exercise error context)...
         try
         {
            expected_throw_line_number.insert(0, "(" TO_STRING_LITERAL(__LINE__) ")"); /* and throw on same line */ throw error_context("simulated test error");
         }
         catch (const error::context & e)
         {
            // test succeeds if the error_context has all the anticipated details present (when caught).
#pragma warning(disable:26489)
            const std::string what_string(e.full_what());
#pragma warning(default:26489)
            utf8::Assert::IsTrue(what_string.find(__SHORT_FILE__) != std::string::npos, "Didn't find __FILE__ in exception what().");
            utf8::Assert::IsTrue(what_string.find(expected_throw_line_number) != std::string::npos, "Didn't find (throw) __LINE__ in exception what().");
            utf8::Assert::IsTrue(what_string.find(__FUNCTION__) != std::string::npos, "Didn't find __FUNCTION__ in exception what().");
         }
      }
   };
}
