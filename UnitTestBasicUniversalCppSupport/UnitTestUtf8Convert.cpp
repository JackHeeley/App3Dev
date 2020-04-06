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

#pragma warning(disable: 26440 26477 26497)
      TEST_CLASS_INITIALIZE(InitializeUnitTestUtf8Convert) noexcept
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

            //round trip to get back to a string (since we have no utf8::Assert::AreEqual for GUID type)
            std::string actual_guid_string = utf8::guid_convert::from_guid(actual_guid);

            // test succeeds if string values match...
            utf8::Assert::AreEqual(expected_guid_string, actual_guid_string, "converted GUID string does not match expected value");
         }
         catch (const std::exception & e)
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

            utf8::Assert::AreEqual(expected_guid, actual_guid, "converted GUID does not match expected value");

            //round trip to get back to a string (since we have no utf8::Assert::AreEqual for GUID type)
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