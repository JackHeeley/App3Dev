//
// UnitTestDeviceDiscoverer.cpp : a utf8 everywhere component unit test 
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

namespace UnitTestExtendedUniversalCppSupport
{
   TEST_CLASS(UnitTestDeviceDiscoverer)
	{
	public:
		
#pragma warning(disable: 26440 26477 26497)
      TEST_CLASS_INITIALIZE(InitializeUnitDeviceDiscoverer) noexcept
#pragma warning(default: 26440 26477 26497)
      {
         try
         {
            CREATE_LOGGER(logger_factory::type::file_logger, log_file_name, LogFilter::Full);
         }
         catch (...)
         {
            LOG_ERROR("Couldn't create logger.");     // should fallback and emit on std::cerr
         }
      }

      TEST_METHOD(TestDeviceDiscovererConstructor)
      {
         try
         {
            // perform the operation under test (happy flow)...
            DeviceDiscoverer cdrom(DeviceTypeDirectory::DeviceType::CDROM_DEVICES);

            // check result (happy flow)  
            utf8::Assert::IsFalse((cdrom.device_path_map.get().empty()), "unexpected that there are no CD ROM devices on this system. If this is true, ignore this test 'failure'.");
            std::string cdrom_path0 = cdrom.device_path_map.get()[0];
            utf8::Assert::IsTrue((strlen(cdrom_path0.c_str()) > 0), "the path returned was empty");

            // perform the operation under test (unhappy flow)...
            DeviceDiscoverer bad_device(DeviceTypeDirectory::DeviceType::FLOPPY_DEVICES);
            utf8::Assert::IsTrue(bad_device.device_path_map.get().empty(), "number of FLOPPY devices was unexpected");

            // check result (unhappy flow)  
            std::string bad_device0_path = bad_device.device_path_map.get()[-1]; // try as we might we don't throw here
            utf8::Assert::IsTrue((bad_device0_path == ""), "we retrieved a name for a non-existing enumeration of a device identified using an invalid guid");
         }
         catch (const std::exception& e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }

      }
      
      TEST_METHOD(TestDeviceDiscovererCopyConstructor)
      {
         try
         {
            // prepare for test (fetch cdrom data)
            DeviceDiscoverer cdrom(DeviceTypeDirectory::DeviceType::CDROM_DEVICES);

            // perform the operation under test...
            DeviceDiscoverer cdrom1(cdrom);

            // check that the target matches source...
            utf8::Assert::IsTrue((cdrom1 == cdrom), "Source or destination was corrupted by copy operation");
         }
         catch (std::exception e)
         {
            utf8::Assert::IsTrue(false, e.what()); // report what went wrong
         }
      }

      TEST_METHOD(TestDeviceDiscovererMoveConstructor)
      {
         try
         {
            // prepare for test (fetch cdrom data)
            DeviceDiscoverer cdrom(DeviceTypeDirectory::DeviceType::CDROM_DEVICES);

            const auto n_cdrom_devices_before = cdrom.device_path_map.get().size();
            const auto cdrom_device0_path = cdrom.device_path_map.get()[0];

            // perform the operation under test...
            DeviceDiscoverer cdrom1(std::move(cdrom));

            // check that target is well formed...
            utf8::Assert::IsTrue((cdrom1.device_path_map.get().size() == n_cdrom_devices_before), "Source data was corrupted during move");
            utf8::Assert::IsTrue((cdrom1.device_path_map.get()[0] == cdrom_device0_path), "device path was not transferred by move");

            // after moves source must be left in a valid (but unspecified) state. In this case we find source addressable (as required), and empty (not required).
            utf8::Assert::IsTrue((cdrom1 != cdrom), "moved from object (unexpectedly) preserved during move");

         }
         catch (std::exception e)
         {
            utf8::Assert::IsTrue(false, e.what()); // report what went wrong
         }
      }

      TEST_METHOD(TestDeviceDiscovererCopyAndCompare)
      {
         try
         {
            // prepare for test (fetch disk and cdrom data)
            DeviceDiscoverer disk(DeviceTypeDirectory::DeviceType::DISK_DEVICES);   // initialize disk with some "irrelevent" data
            DeviceDiscoverer cdrom(DeviceTypeDirectory::DeviceType::CDROM_DEVICES); // fetch the cd_rom data (but to a short-lived local scope object)

            const auto n_disk_devices_before = disk.device_path_map.get().size();
            const auto n_cdrom_devices_before = cdrom.device_path_map.get().size();

            // check pre-condition
            utf8::Assert::IsTrue((disk != cdrom), "DeviceDiscoverer objects matched before copy");

            // perform the operation under test (copy construct)...
            DeviceDiscoverer copy = DeviceDiscoverer(cdrom);

            // check result
            utf8::Assert::IsTrue((copy == cdrom), "DeviceDiscoverer objects didn't match after copy construct");
            
            // perform the operation under test (copy assign)...
            disk = cdrom;

            // check result
            utf8::Assert::IsTrue((disk == cdrom), "DeviceDiscoverer objects didn't match after copy assign");
         }
         catch (std::exception e)
         {
            utf8::Assert::IsTrue(false, e.what()); // report what went wrong
         }
      }

      TEST_METHOD(TestDeviceDiscovererMove)
      {
         try
         {
            // prepare for test (fetch disk and cdrom data)
            DeviceDiscoverer disk(DeviceTypeDirectory::DeviceType::DISK_DEVICES);   // initialize disk with some "irrelevent" data
            DeviceDiscoverer cdrom(DeviceTypeDirectory::DeviceType::CDROM_DEVICES); // fetch the cd_rom data

            DeviceDiscoverer cdrom_copy = cdrom;                      // keep copy for comparison later
            
            // check pre-condition
            utf8::Assert::IsTrue((disk != cdrom), "DeviceDiscoverer objects matched before copy");

            // perform the operation under test (move assign)...
            disk = std::move(cdrom);       

            // check result
            utf8::Assert::IsTrue((disk == cdrom_copy), "move failed to create an identical object");

            // after moves source must be left in a valid (but unspecified) state. In this case we find source addressable (as required), and different (not required).
            utf8::Assert::IsFalse((cdrom == cdrom_copy), "moved from object is equal after move");
         }
         catch (std::exception e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }

      TEST_METHOD(TestDeviceDiscovererDevicePathIsValid)
      {
         try
         {
            // prepare the test 
            DeviceDiscoverer cdrom(DeviceTypeDirectory::DeviceType::CDROM_DEVICES);    
            utf8::Assert::IsFalse(cdrom.device_path_map.get().empty(), "a CDROM device is not detected/attached (can't test");

            // perform the test (use the retrieved data)...
            std::wstring devicePathW = utf8::convert::to_utf16(cdrom.device_path_map.get()[0]);       

            // open the zero'th enumerated cdrom device (currently) attached to the system...
            HANDLE cd_rom_device_handle = CreateFile(
               devicePathW.c_str(),
               GENERIC_READ,
               FILE_SHARE_READ,
               NULL,
               OPEN_EXISTING,
               0,
               0
            );

            // check result 
            utf8::Assert::IsFalse((cd_rom_device_handle == INVALID_HANDLE_VALUE), "Program failed to open the device");

            // clean up
            if (!CloseHandle(cd_rom_device_handle))
            {
               utf8::Assert::IsTrue(false, "Program opened, but failed to close the device");
            }

         }
         catch (std::exception e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }
	};
}