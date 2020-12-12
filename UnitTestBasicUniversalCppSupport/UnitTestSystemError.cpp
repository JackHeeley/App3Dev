//
// UnitTestSystemError.cpp : a utf8 everywhere component unit test 
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

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace utf8;

namespace UnitTestBasicUniversalCppSupport
{
   TEST_CLASS(UnitTestSystemError)
   {
   public:

#pragma warning(disable: 26440 26497)
      TEST_CLASS_INITIALIZE(InitializeUnitTestSystemError) noexcept  // NOLINT(clang-diagnostic-missing-braces)
#pragma warning(default: 26440 26497)
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

      TEST_METHOD(TestSystemErrorDefaultConstructor)
      {
         try
         {
            // prepare for test (nothing to do)...

            // perform the operation under test (construct a SystemError with default constructor)...
            SystemError lastError;

            // test succeeds if construction doesn't throw and error_code is zero...
            utf8::Assert::AreEqual(lastError.get_error_code(), 0, "an error was detected when none were triggered");

         }
         catch (const std::exception& e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }

      TEST_METHOD(TestSystemErrorParameterizedConstructor)
      {
         try
         {
            // prepare for test (nothing to do)...

            // perform the operation under test (construct some SystemErrors with the parameterized constructor)...
            for (int i = 0; i < 256; i++)
            {
               SystemError lastError(i);
               
               // test succeeds if construction doesn't throw and error_code is i ...
               utf8::Assert::AreEqual(lastError.get_error_code(), i, "a different error_code was returned from the one constructed");
            }
         }
         catch (const std::exception& e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }

      TEST_METHOD(TestSystemErrorCopyConstructor)
      {
         try
         {
            // prepare for test 
            SystemError error_a(10);

            // perform the operation under test (copy construct)...
            SystemError error_b(error_a);

            // check that the target matches source...
            utf8::Assert::IsTrue((error_b == error_a), "Source or destination was corrupted by copy operation");
         }
         catch (const std::exception& e)
         {
            utf8::Assert::Fail(e.what()); // report what went wrong
         }
      }

      TEST_METHOD(TestSystemErrorCopyAndCompare)
      {
         try
         {
            // prepare for test (make some system error objects)
            SystemError error_a(5);    // initialize with some data
            SystemError error_b(8);    // initialize with some different data

            auto error_text_a_before = error_a.get_error_text();
            auto error_text_b_before = error_b.get_error_text();

            utf8::Assert::IsTrue((error_text_a_before != error_text_b_before), "equal error texts do not support this test");

            // check pre-condition
            utf8::Assert::IsTrue((error_a != error_b), "SystemErrors matched before copy");

            // perform the operation under test (copy assign)...
            error_a = error_b;

            // check result
            utf8::Assert::IsTrue((error_a == error_b), "SystemErrors didn't match after copy");
         }
         catch (const std::exception& e)
         {
            utf8::Assert::Fail(e.what()); // report what went wrong
         }
      }

      TEST_METHOD(TestSystemErrorMove)
      {
         try
         {
            // prepare for test (construct some data)
            SystemError error_a(5);    // initialize with some data
            SystemError error_b(8);    // initialize with some different data

            SystemError error_b_copy = error_b;                      // keep copy for comparison later

                                                                     // check pre-condition
            utf8::Assert::IsTrue((error_a != error_b), "SystemError objects already matched before copy");

            // perform the operation under test (move assign)...
            error_a = std::move(error_b);

            // check result
            utf8::Assert::IsTrue((error_a == error_b_copy), "move failed to create an identical object");

            // after moves source must be left in a valid (but unspecified) state. In this case we find source addressable (as required), and not equal (acceptable).
            utf8::Assert::IsTrue((error_b != error_b_copy), "moved from object is still equal after move (suspicious)");   // NOLINT(clang-analyzer-cplusplus.Move)
         }
         catch (const std::exception& e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }
      
      TEST_METHOD(TestSystemErrorGetErrorCode)
      {
         try
         {
            // prepare for test...
            constexpr int error_code = 1;
            constexpr int expected_value = 1;
            SystemError lastError(error_code);

            // perform the operation under test (get error_code from SystemError)...
            const auto actual_value = lastError.get_error_code();

            // test succeeds if error_code matches
            utf8::Assert::IsTrue((actual_value == expected_value), "SystemError returned unexpected error code");
         }
         catch (const std::exception& e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }

      TEST_METHOD(TestSystemErrorGetErrorText)
      {
         try
         {
            // prepare for test...
            constexpr int error_code = 1;
            const std::string expected_value("Incorrect function.\r\n");
            SystemError lastError(error_code);

            // perform the operation under test (construct some SystemErrors with the parameterized constructor)...
            utf8::Assert::AreEqual(lastError.get_error_code(), error_code, "a different error_code was returned from the one constructed");

            // perform the operation under test (call get_error_text)
            std::string actual_value = lastError.get_error_text();

            // test succeeds if construction doesn't throw and error_code is 1, and get_error_text() returns the expected string value
            utf8::Assert::IsTrue((actual_value == expected_value), "SystemError returned unexpected error text");
         }
         catch (const std::exception& e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }

      TEST_METHOD(TestSystemErrorOutOfRangeValues)
      {
         try
         {
            // prepare for test (select some 'bad' values)...
            std::vector<int> test_case { 0x23, 0x2000 };

            // perform the operation under test (construct some SystemErrors with the parameterized constructor)...
            for (const int i : test_case )
            {
               SystemError lastError(i);

               // test succeeds if construction doesn't throw and...
               
               // 1) error_code is i ...
               utf8::Assert::AreEqual(lastError.get_error_code(), i, "an error was detected when none were triggered");

               // 2) get_error_text() doesn't throw and...
               utf8::Assert::IsFalse(lastError.get_error_text().find(") out of range, or otherwise unformattable.\r\n") == std::string::npos, "Out of range system error reported some other error text");

               // interrogate why we failed
               SystemError cause;
               std::stringstream ss;
               ss << "Couldn't decode error_code(" << cause.get_error_code() << ") because of: " << cause.get_error_text();
               std::cout << ss.str().c_str();
            }
         }
         catch (const std::exception& e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }
   };
}