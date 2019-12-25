//
// UnitTestDevice.cpp : a utf8 everywhere component unit test 
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

namespace UnitTestExtendedUniversalCppSupport
{

#define IOCTL_STORAGE_BAD_IOCTL CTL_CODE(IOCTL_STORAGE_BASE, 0x02FF, METHOD_BUFFERED, FILE_READ_ACCESS)

   TEST_CLASS(UnitTestDevice)
   {
   public:

#pragma warning(disable: 26440 26477)
      TEST_CLASS_INITIALIZE(InitializeUnitTestDevice) noexcept
#pragma warning(default: 26440 26477)
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

      TEST_METHOD(TestDeviceConstructor)
      {
         try
         {
            // prepare for test (ensure suitable test hardware exists on this system)...
            DeviceDiscoverer cdrom_interface(DeviceTypeDirectory::DeviceType::CDROM_DEVICES);
            utf8::Assert::IsFalse(cdrom_interface.device_path_map.get().empty(), "no system cdrom devices were discovered");

            // perform the operation under test (construct a device for the system's first enumerated cdrom)...
            Device cdrom(DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0]);

            // test succeeds if construction doesn't throw
         }
         catch (const std::exception& e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }

      TEST_METHOD(TestDeviceIoCtl)
      {
         //utf8::Assert::Fail("Test deprecated (takes too long). Use <testapp> (tbs) instead, or comment out this fail.");

         try
         {
            // prepare for test (construct a device for the system's first enumerated cdrom)...
            Device cdrom(DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0]);

            // perform the operation under test (issue valid ioctl to the device)...
            auto nBytesReturned = cdrom.ioctl(IOCTL_STORAGE_EJECT_MEDIA, nullptr, 0, nullptr, 0);

            // check results (the cdrom tray should open) and programatically...
            utf8::Assert::IsTrue(nBytesReturned == 0, "ioctl returned an out-of-spec byte count");
            
            // restore the system state (close the tray)...
            try
            {
               nBytesReturned = cdrom.ioctl(IOCTL_STORAGE_LOAD_MEDIA, nullptr, 0, nullptr, 0);
               utf8::Assert::IsTrue(nBytesReturned == 0, "ioctl returned an out-of-spec byte count");
            }
            catch (const std::exception& e)
            {
               switch (SystemError().get_error_code())
               {
               case ERROR_INVALID_FUNCTION:  
                  // some cdrom devices don't have this physical capability (tester should close tray door by hand)
                  break;

               default:
                  utf8::Assert::Fail((std::string(e.what()).append(" was not expected.")).c_str());
               }
            }
            
            // continue with an exception path test (issue a bad ioctl to the device)...
            try
            {
               nBytesReturned = cdrom.ioctl(IOCTL_STORAGE_BAD_IOCTL, nullptr, 0, nullptr, 0);
               utf8::Assert::IsTrue(false,"ioctl didn't reject a foreign ctl_code");
            }
            catch (const std::exception& e)
            {
               switch (SystemError().get_error_code())
               {
               case ERROR_SUCCESS:                 // since Win10 1803
               case ERROR_INVALID_FUNCTION:
               case ERROR_NOT_SUPPORTED:
                  break;

               default:
                  utf8::Assert::Fail((std::string(e.what()).append(" was not expected.")).c_str());
               }
            }
         }
         catch (const std::exception& e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }

      TEST_METHOD(TestDeviceRead)
      {
         try
         {
            // prepare for test (construct a device for the system's first enumerated cdrom)...
            Device cdrom(DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0]);

            // cdroms are only readable in exact block size units that depend on disk geometry
            // this test hard codes a safe size (doesn't query actual geometry of the disk)
            constexpr int BLOCK_SIZE = 4096;
            std::vector<unsigned char>buffer;
            buffer.reserve(BLOCK_SIZE);

            uint32_t nBytesReturned = 0;

            int retries = 60;
            while (--retries >= 0 && nBytesReturned != BLOCK_SIZE)
            {
               try
               {
                  // dismiss possible errors from previous pass
                  SystemError().clear_error_code();

                  // perform the operation under test (issue a read on the device)...
                  nBytesReturned = cdrom.read(buffer.data(), BLOCK_SIZE);
                  break;
               }
               catch (const std::exception & e)
               {
                  switch (SystemError().get_error_code())
                  {
                  // some cases are worth retrying
                  case ERROR_NOT_READY:
                  case ERROR_MEDIA_CHANGED:
                  case ERROR_IO_DEVICE:
                     std::this_thread::sleep_for(1s);
                     break; 

                  default: throw e;
                  }
               }
            }

            // check results...
            SystemError lastError;
            if (lastError.get_error_code() == ERROR_SUCCESS)
            {
               utf8::Assert::IsTrue(nBytesReturned == BLOCK_SIZE, "read returned an unexpected byte count");
            }
            else
            {
               utf8::Assert::IsTrue(false, lastError.get_error_text().c_str());
            }
            
         }
         catch (const std::exception & e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }
   };
}