//
// UnitTestUtf8Convert.cpp : a utf8 everywhere component unit test 
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

#include "utf8_guid.hpp" 

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace utf8;

namespace UnitTestBasicUniversalCppSupport
{
   TEST_CLASS(UnitTestUtf8Convert)
   {
   public:

#pragma warning(disable: 26440)
      TEST_CLASS_INITIALIZE(InitializeUnitTestUtf8Convert) noexcept  // NOLINT(clang-diagnostic-missing-braces)
#pragma warning(default: 26440)
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

      TEST_METHOD(TestUtf8CountCodepoints)
      {
         try
         {
            // This test provides a demonstration of what a code point is. 
            // It shows how string length can differ from the number of separately readable characters 
            // and shows that it DOES in fact differ when you need to use an extended character set.
            // 
            // UTF8 represents a code point (e.g. a printing character or white space etc.) using anything between 1 and 4 bytes of storage
            // The convention we adopt here is to store all strings in utf8 encoded format as classical zero terminated char strings, and
            // to manipulate these in code using modern std::string objects. Where this convention is expensive or inconvenient to apply
            // (such as with the device access API's in the extended library) we encapsulate and isolate the unconventional elements using
            // pointer to implementation (PIMPL) constructs. This generally means string conversion is a one time (construction) overhead
            // and limits the runtime impact fairly effectively in most practical situations. In some cases (such as with Microsoft CPP unit
            // test framework Assert) it is more convenient to apply a just in time intercept and convert strategy, and although this is 
            // theoretically expensive, in the context of unit testing it has negligible performance impact.
            //
            // The benefit of utf8 everywhere is evident looking at this unit test. It shows that "naive" assignment of string literals can 
            // be applied completely naturally in the bulk of the code, without complexities arising from wrong encoding assumptions.
            //
            // As this unit test demonstrates, when programmers are actually working with an extended character set, they need to appreciate 
            // the variable length character encoding aspect of UTF8. We provide a codepoint_count function for convenience in this respect.
         
            std::string s(U8("Γειά σας Κόσμε!\n"));
            utf8::Assert::IsTrue(s.length() == 28, "UTF8 unexpected data length");
            utf8::Assert::IsTrue(utf8::count_codepoints(s) == 16, "UTF8 unexpected codepoint count");

            s = (U8("Γειά σας Κόσμε!\n"));
            utf8::Assert::IsTrue(s.length() == 28, "Test 2 UTF8 unexpected data length");
            utf8::Assert::IsTrue(utf8::count_codepoints(s) == 16, "Test 2 UTF8 unexpected codepoint count");

#pragma warning(disable: 4566)                   // we override the compiler warning to demonstrate that an
            s = "Γειά σας Κόσμε!\n";             // ill formed utf8 string literal causes a character encoding error
#pragma warning(default: 4566)
            utf8::Assert::IsFalse(s.length() == 28, "programmer's anticipated value (28), is unexpected in this bug scenario");
            utf8::Assert::IsTrue(s.length() == 16, "test designer's expected value (16), was not found");
            utf8::Assert::IsTrue(utf8::count_codepoints(s) == 16, "Test 3 UTF8 unexpected codepoint count");

            s = U8("Hello World!\n");
            utf8::Assert::IsTrue(s.length() == 13, "UTF8 ansi compatibility unexpected data length");
            utf8::Assert::IsTrue(utf8::count_codepoints(s) == 13, "UTF8 ansi compatibility unexpected codepoint count");

            s = "Hello World!\n";
            utf8::Assert::IsTrue(s.length() == 13, "ANSI Unexpected data length");
            utf8::Assert::IsTrue(utf8::count_codepoints(s) == 13, "ANSI Unexpected codepoint count");

            s.clear();
            utf8::Assert::IsTrue(utf8::count_codepoints(s) == 0, "codepoint count failed with empty string");
         }
         catch (const std::exception& e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }

      TEST_METHOD(TestUtf8ConvertFromGuid)
      {
         try
         {
            // prepare for test -  express GUID_DEVINTERFACE_CDROM as a string. Format matches lower case DEFINE_GUID text style (see "winioctl.h")...
            const std::string expected("0x53f56308L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b");

            // perform the operation under test (convert actual guid to string)...
            std::string actual = utf8::guid_convert::from_guid(GUID_DEVINTERFACE_CDROM);

            // test succeeds if values match...
            utf8::Assert::AreEqual(expected, actual, "converted GUID does not match expected value");
         }
         catch (const std::exception& e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }

      TEST_METHOD(TestUtf8ConvertToGuid)
      {
         try
         {
            // prepare for test -  express GUID_DEVINTERFACE_CDROM as a string. Format matches lower case DEFINE_GUID text style (see "winioctl.h")...
            const std::string initial_guid_string("0x53f56308L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b");

            // outcome after round trip should be the same...
            const std::string expected_guid_string(initial_guid_string);

            const GUID expected_guid = GUID_DEVINTERFACE_CDROM; // this IS "0x53f56308L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b"

            // perform the operation under test (convert string to GUID)...
            const GUID actual_guid = utf8::guid_convert::to_guid(initial_guid_string);

            // test succeeds if GUID values match (template specialization is required here, see UnitTestBasicUniversalCppSupport.hpp)...
            utf8::Assert::AreEqual(expected_guid, actual_guid, "converted GUID does not match expected value");

            // round trip to get back to a string (earlier we had no utf8::Assert::AreEqual for GUID type)
            std::string actual_guid_string = utf8::guid_convert::from_guid(actual_guid);

            // test succeeds if string values match...
            utf8::Assert::AreEqual(expected_guid_string, actual_guid_string, "converted GUID string does not match expected value");
         }
         catch (const std::exception & e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }      
 
      TEST_METHOD(TestGuidAsserts)
      {
         // a template specialization (see UnitTestBasicUniversalCppSupport.hpp) is required before AreEqual will support _GUID parameter
         // here we confirm that this measure is sufficient and has the desired effect before we rely on it in other tests

         try
         {
            // prepare for test -  express GUID_DEVINTERFACE_CDROM as a string. Format matches lower case DEFINE_GUID text style (see "winioctl.h")...
            const std::string cdrom_string("0x53f56308L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b");
            const std::string mismatch("0x53f56309L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b");

            const GUID GUID_MY_CDROM = utf8::guid_convert::to_guid(cdrom_string);
            const GUID GUID_MISMATCH = utf8::guid_convert::to_guid(mismatch);

            // test succeeds if AreEqual identifies matching GUID objects and AreNotEqual identifies mismatches...
            utf8::Assert::AreEqual(GUID_MY_CDROM, GUID_DEVINTERFACE_CDROM, "Assert::AreEqual fails to identify equal GUIDs");
            utf8::Assert::AreNotEqual(GUID_MISMATCH, GUID_DEVINTERFACE_CDROM, "Assert::AreNotEqual fails to identify not equal GUIDs");

            //// we have also confirmed the corollory, i.e. correct behaviour when the asserts fail. (This test can't be automated)
            //// uncommenting here also offers an opportunity to do a visual check on the _GUID template specialization supplied 
            //// (see: UnitTestBasicUniversalCppSupport.hpp)
            //try
            //{
            //   utf8::Assert::AreNotEqual(GUID_MY_CDROM, GUID_DEVINTERFACE_CDROM, "Assert::AreNotEqual correctly aborted with equal GUIDs");
            //   utf8::Assert::AreEqual(GUID_MISMATCH, GUID_DEVINTERFACE_CDROM, "Assert::AreEqual correctly aborted with not equal GUIDs");
            //}
            //catch (...)
            //{
            //   // we wish this would do the trick, but you can't catch an assert this way.
            //}

         }
         catch (const std::exception& e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }

      TEST_METHOD(TestUtf8ConvertToGuidSpecials)
      {
         try
         {
            // prepare for test -  express GUID_DEVINTERFACE_CDROM as a string. Format matches lower case DEFINE_GUID text style (see "winioctl.h")...
            const std::string initial_guid_string("0x53F56308L, 0xB6BF, 0x11D0, 0x94, 0xF2, 0x00, 0xa0, 0xC9, 0x1E, 0xFB, 0x8B");

            // outcome after round trip should be lower case...
            const std::string expected_guid_string("0x53f56308L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b");

            const GUID expected_guid = GUID_DEVINTERFACE_CDROM; // this IS "0x53f56308L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b"

            // perform the operation under test (convert string to GUID)...
            const GUID actual_guid = utf8::guid_convert::to_guid(initial_guid_string);

            // test succeeds if GUID values match (template specialization is required here, see UnitTestBasicUniversalCppSupport.hpp)...
            utf8::Assert::AreEqual(expected_guid, actual_guid, "converted GUID does not match expected value");

            // round trip to get back to a string (earlier we had no utf8::Assert::AreEqual for GUID type)
            std::string actual_guid_string = utf8::guid_convert::from_guid(actual_guid);

            // test succeeds if string values match...
            utf8::Assert::AreEqual(expected_guid_string, actual_guid_string, "converted GUID string does not match expected value");
         }
         catch (const std::exception & e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }
   };
}