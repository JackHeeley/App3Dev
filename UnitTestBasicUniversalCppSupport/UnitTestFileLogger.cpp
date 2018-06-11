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
         catch (error::context& e)
         {
            std::string what_string(e.full_what());
            utf8::Assert::IsTrue(what_string.find("unittestfilelogger.cpp") != std::string::npos, "Didn't find __FILE__ in exception what().");
            //utf8::Assert::IsTrue(what_string.find("L26") != std::string::npos, "Didn't find __LINE__ in exception what().");
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
         catch (error::context& e)
         {
            utf8::Assert::IsTrue(false, e.full_what()); // something went wrong
         }
      }

   };
}