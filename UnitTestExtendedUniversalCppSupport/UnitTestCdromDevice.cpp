//
// UnitTestCdromDevice.cpp : a utf8 everywhere component unit test 
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
   CREATE_LOGGER(logger_factory::type::file_logger, "MyUnitTestExtendedUniversalCppSupport.log", LogFilter::Full);

   TEST_CLASS(UnitTestCdromDevice)
   {
   public:

      TEST_METHOD(TestCdromDeviceConstructor)
      {
         try
         {
            // prepare for test (ensure suitable test hardware exists on this system)...
            DeviceDiscoverer cdrom_interface(DeviceTypeDirectory::DeviceType::CDROM_DEVICES);
            utf8::Assert::IsFalse(cdrom_interface.device_path_map.get()[0] == "", "no system cdrom devices were discovered");

            std::atomic<int> progress;

            // perform the operation under test (construct a device for the system's first enumerated cdrom)...
            CdromDevice cdrom(DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0], progress);

            // test succeeds if construction doesn't throw
            cdrom.unlock();
         }
         catch (const std::exception & e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }

      TEST_METHOD(TestCdromTrayLocking)
      {
         // RAII door lock helper
         class TrayDoorLock
         {
            CdromDevice& m_cdr;

         public:
            TrayDoorLock(CdromDevice& cdrom) : m_cdr(cdrom)
            {
               m_cdr.lock();
            }
            ///<summary> deleted copy constructor.</summary>
            TrayDoorLock(TrayDoorLock& other) = delete;

            ///<summary> deleted move constructor.</summary>
            TrayDoorLock(TrayDoorLock&& other) = delete;

            ///<summary> deleted copy assignment.</summary>
            TrayDoorLock& operator=(TrayDoorLock& other) = delete;

            ///<summary> deleted move assignment.</summary>
            TrayDoorLock& operator=(TrayDoorLock&& other) = delete;

            ~TrayDoorLock()
            {
               try
               {
                  m_cdr.unlock();
               }
               catch (const std::exception& e)
               {
                  USES(e);
                  LOG_WARNING(e.what());
               }
            }
         };

         // issue IOCTL_STORAGE_EJECT_MEDIA while PREVENT_MEDIA_REMOVAL is/is not enacted...
         try
         {
            std::atomic<int> progress;
            std::string device_path = DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0];
            CdromDevice cdrom0(device_path, progress);

            int retries = 10;

            while (retries-- > 0)
            {
               try
               {
                  // perpare for test (lock the tray door)...
                  TrayDoorLock scoped_lock(cdrom0);

                  utf8::Assert::IsTrue(cdrom0.get_locked(), "cdrom0 reports unlocked when expected state is locked");

                  // perform the operation under test...
                  cdrom0.eject();

                  // check results (expect fail with invalid function)
                  utf8::Assert::Fail("tray door could be opened despite being locked");
               }
               catch (const std::exception& e)
               {
                  switch (SystemError().get_error_code())
                  {
                  case ERROR_INVALID_FUNCTION:        // good (eject is invalid if device is locked)
                  case ERROR_SUCCESS:                 // since WINDOWS 10 1803
                     break;

                  case ERROR_NOT_READY:
                  case ERROR_MEDIA_CHANGED:
                  case ERROR_IO_DEVICE:
                     std::this_thread::sleep_for(1s); // not ready conditions, expected in some test sequences
                     break;

                  default: throw e;                   // freaky stuff
                  }
               }
            }

            // tray door is unlocked as scoped_lock goes out of scope (both normal and exception path)
            utf8::Assert::IsFalse(cdrom0.get_locked(), "cdrom0 reports locked when expected state is unlocked");

            // continue perform the operation under test (expect success now)...
            cdrom0.eject();            // check results (visually confirm that tray opens)

            // restore the system state (close the tray - if possible)...
            try
            {
               cdrom0.load();            // check results (visually confirm that tray closes)
            }
            catch (const std::exception& e)
            {
               switch (SystemError().get_error_code())
               {
               case ERROR_INVALID_FUNCTION:
                  break;               // some devices don't support load (tester will have to shut the door by hand)
               default:
                  throw e;
               }
            }
         }
         catch (const std::exception& e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }

      TEST_METHOD(TestCdromGetImageSize)
      {

         try
         {
            std::atomic<int> progress;

            // prepare for test (construct a device for the system's first enumerated cdrom)...
            CdromDevice cdrom(DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0], progress);
                       
            int retries = 10;                          // cdrom drive may not be in ready state
            while (--retries > 0)
            {
               try
               {
                  // perform the operation under test (query device geometry and calculate media size)...
                  const auto size = cdrom.get_image_size();
                  break;
               }
               catch (const std::exception& e)
               {
                  switch (SystemError().get_error_code())
                  {
                  case ERROR_NOT_READY:
                  case ERROR_MEDIA_CHANGED:
                  case ERROR_IO_DEVICE:
                     std::this_thread::sleep_for(1s); // not ready conditions, expected in some test sequences
                     break;

                  default: throw e;
                  }
               }
            };

            // considered success if a size is returned.
         }
         catch (const std::exception& e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }

      //TEST_METHOD(TestCdromDeviceReadImage)
      //{
      //   utf8::Assert::Fail("Test deprecated (takes too long). Use <testapp> (tbs) instead, or comment out this fail.");

      //   // RAII door lock helper
      //   class TrayDoorLock
      //   {
      //      CdromDevice& m_cdr;

      //   public:
      //      TrayDoorLock(CdromDevice& cdrom) : m_cdr(cdrom)
      //      {
      //         m_cdr.lock();
      //      }
      //      ///<summary> deleted copy constructor.</summary>
      //      TrayDoorLock(TrayDoorLock& other) = delete;

      //      ///<summary> deleted move constructor.</summary>
      //      TrayDoorLock(TrayDoorLock&& other) = delete;

      //      ///<summary> deleted copy assignment.</summary>
      //      TrayDoorLock& operator=(TrayDoorLock& other) = delete;

      //      ///<summary> deleted move assignment.</summary>
      //      TrayDoorLock& operator=(TrayDoorLock&& other) = delete;

      //      ~TrayDoorLock()
      //      {
      //         try
      //         {
      //            m_cdr.unlock();
      //         }
      //         catch (std::exception& e)
      //         {
      //            LOG_WARNING(e.what());
      //         }
      //      }
      //   };

      //   try
      //   {
      //      // prepare for test (construct a device for the system's first enumerated cdrom)...
      //      std::atomic<int> progress;

      //      CdromDevice cdrom(DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0], progress);

      //      // ...create buffer. On x32 resize will throw std::exception("vector<T> too long) if CDROM/DVD image is too large for a byte vector
      //      std::vector<unsigned char> buffer;
      //      buffer.resize(gsl::narrow<std::vector<unsigned char>::size_type>(cdrom.get_image_size()));    

      //      // disable media eject (also operator initiated) during test
      //      TrayDoorLock scoped_lock(cdrom);

      //      bool test_done = false;    // cdrom drive may not be in ready state
      //      while (!test_done)
      //      {
      //         try
      //         {
      //            // perform the operation under test (read full device content)...
      //            cdrom.get_image(buffer);    // doesn't require an explicit gsl::as_span<unsigned char>(buffer)
      //            test_done = true;
      //         }
      //         catch (const std::exception& e)
      //         {
      //            // other tests can appear to have 'swapped' media...
      //            if (SystemError().get_error_code() != ERROR_MEDIA_CHANGED) throw e;
      //         }
      //      };

      //      // write to file (confirm by mounting iso)
      //      std::ofstream file("test.iso", std::ios::out | std::ios::binary);
      //      file.unsetf(std::ios::skipws); // Don't eat new lines in binary mode!!!
      //      file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
      //      file.close();

      //      // check results (by hand) - image is good if file mounts and is readable.
      //   }
      //   catch (const std::exception& e)
      //   {
      //      utf8::Assert::IsTrue(false, e.what()); // something went wrong
      //   }
      //}
   };
}