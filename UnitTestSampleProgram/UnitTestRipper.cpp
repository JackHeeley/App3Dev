//
// UnitTestRipper.cpp : a utf8 everywhere component unit test 
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
using namespace std::chrono_literals;
using namespace utf8;

namespace UnitTestSampleProgram
{

   TEST_CLASS(UnitTestRipper)
   {
   public:

#pragma warning(disable: 26440 26477 26497)
      TEST_CLASS_INITIALIZE(InitializeUnitTestRipper) noexcept
#pragma warning(default: 26440 26477 26497)
      {
         try
         {
            CREATE_LOGGER(logger_factory::type::file_logger, log_file_name, DEFAULT_LOG_FILTER);
         }
         catch (...)
         {
            LOG_ERROR("Couldn't create logger.");       // No logger? This will emit on std::cerr
         }
      }

#pragma warning(disable: 26477)
      BEGIN_TEST_METHOD_ATTRIBUTE(TestRipperMove)
         TEST_IGNORE()        // TestFunctor takes too long to run every time...
      END_TEST_METHOD_ATTRIBUTE()
#pragma warning(default: 26477)
      TEST_METHOD(TestRipperMove)
      {
         try
         {
            // check test preconditions (at least one physical cdrom needed)
            DeviceDiscoverer cdromDevices(DeviceTypeDirectory::DeviceType::CDROM_DEVICES);
            utf8::Assert::IsFalse(cdromDevices.device_path_map.get().empty(), "no system cdrom devices were discovered");
            
            //prepare test 
            auto deviceName = cdromDevices.device_path_map.get()[0];

            //perform operation under test
            Ripper rip = std::move(Ripper(deviceName));

            // check results (use the moved ripper)
            std::atomic<int> progress;
            const static std::string fileName("cdrom_image.iso");
            rip(fileName, progress);

            // Test succeeds if it doesn't throw
         }
         catch (const std::exception & e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }

#pragma warning(disable: 26477)
      BEGIN_TEST_METHOD_ATTRIBUTE(TestFunctor)
         TEST_IGNORE()        // TestFunctor takes too long to run every time...
      END_TEST_METHOD_ATTRIBUTE()
#pragma warning(default: 26477)

      TEST_METHOD(TestFunctor)
      {
         try
         {
            // check test preconditions (at least one physical cdrom needed)
            DeviceDiscoverer cdromDevices(DeviceTypeDirectory::DeviceType::CDROM_DEVICES);
            utf8::Assert::IsFalse(cdromDevices.device_path_map.get().empty(), "no system cdrom devices were discovered");

            //prepare test 
            auto deviceName = cdromDevices.device_path_map.get()[0];
            Ripper rip(deviceName);
            std::atomic<int> progress;
            const static std::string fileName("cdrom_image.iso");

            //perform operation under test
            rip(fileName, progress);
         }
         catch (const std::exception& e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }
   };
}