//
// UnitTestTrayDoorLock.cpp : a utf8 everywhere component unit test 
//
// Copyright (c) 2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::chrono_literals;
using namespace utf8;

namespace UnitTestSampleProgram
{
   TEST_CLASS(UnitTestTrayDoorLock)
   {
   public:

      TEST_METHOD(TestTrayDoorLock)
      {
         try
         {
            // check test preconditions (at least one physical cdrom needed)
            DeviceDiscoverer cdrom_interface(GUID_DEVINTERFACE_CDROM);
            utf8::Assert::IsFalse(cdrom_interface.device_path_map.get().empty(), "no system cdrom devices were discovered");
            std::atomic<int> progress;

            //prepare test 
            CdromDevice m_cdr(cdrom_interface.device_path_map.get()[0], progress);

            //perform operation under test
            TrayDoorLock lock(m_cdr);
         }
         catch (const std::exception& e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }
   };
}