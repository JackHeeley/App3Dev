//
// UnitTestUtf8Convert.cpp : a utf8 everywhere component unit test 
//
// Copyright (c) 2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace utf8;

namespace UnitTestBasicUniversalCppSupport
{
   TEST_CLASS(UnitTestUtf8Convert)
   {
   public:

      TEST_METHOD(TestUtf8ConvertFromGuid)
      {
         try
         {
            // prepare for test -  express GUID_DEVINTERFACE_CDROM as a string. Format matches lower case DEFINE_GUID text style (see "winioctl.h")...
            const std::string expected("0x53f56308L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b");

            // perform the operation under test (convert actual guid to string)...
            std::string actual = utf8::convert::from_guid(GUID_DEVINTERFACE_CDROM);

            // test succeeds if values match...
            utf8::Assert::AreEqual(expected, actual, "converted GUID does not match expected value");
         }
         catch (const std::exception& e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }
      
   };
}