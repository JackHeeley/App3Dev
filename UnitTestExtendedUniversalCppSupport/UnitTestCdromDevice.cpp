//
// UnitTestCdromDevice.cpp : a utf8 everywhere component unit test 
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

#include <fstream>
#include <future>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::chrono_literals;
using namespace utf8;

namespace UnitTestExtendedUniversalCppSupport
{
   TEST_CLASS(UnitTestCdromDevice)
   {
   public:

#pragma warning(disable: 26440 26497)
      TEST_CLASS_INITIALIZE(InitializeUnitTestCdRomDevice) noexcept  // NOLINT(clang-diagnostic-missing-braces)
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

      TEST_METHOD(TestCdromDeviceConstructor)
      {
         try
         {
            // prepare for test (ensure suitable test hardware exists on this system)...
            DeviceDiscoverer cdrom_interface(DeviceTypeDirectory::DeviceType::CDROM_DEVICES);
            utf8::Assert::IsFalse(cdrom_interface.device_path_map.get()[0] == "", "no system cdrom devices were discovered");

            // perform the operation under test (construct a device for the system's first enumerated cdrom)...
            CdromDevice cdrom(DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0]);

            // test succeeds if construction doesn't throw
            cdrom.unlock();
         }
         catch (const std::exception & e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }

      TEST_METHOD(TestCdromTrayLocking)
      {
         // RAII door lock helper. We could pull in header from SampleProgram (but thats a bit unstructured)
         class RAII_physical_lock
         {
            CdromDevice& m_cdr;

         public:
            RAII_physical_lock(CdromDevice& cdrom) noexcept : m_cdr(cdrom)
            {
               m_cdr.lock();
            }
            ///<summary> deleted copy constructor.</summary>
            RAII_physical_lock(RAII_physical_lock& other) = delete;

            ///<summary> deleted move constructor.</summary>
            RAII_physical_lock(RAII_physical_lock&& other) = delete;

            ///<summary> deleted copy assignment.</summary>
            RAII_physical_lock& operator=(RAII_physical_lock& other) = delete;

            ///<summary> deleted move assignment.</summary>
            RAII_physical_lock& operator=(RAII_physical_lock&& other) = delete;

            ~RAII_physical_lock()
            {
               m_cdr.unlock();
            }
         };

         // issue IOCTL_STORAGE_EJECT_MEDIA while PREVENT_MEDIA_REMOVAL is/is not enacted...
         try
         {
            std::string device_path = DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0];
            CdromDevice cdrom0(device_path);

            int retries = 10;

            while (retries-- > 0)
            {
               try
               {
                  // perpare for test (lock the tray door)...
                  RAII_physical_lock scoped_lock(cdrom0);

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
                  case ERROR_ACCESS_DENIED:           // since we claimed exclusive access
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

      //TODO: TIDY UP THESE CUT AND PASTE VARIANTS ON TestCdromTrayLocking
      TEST_METHOD(TestCdromTrayLockingAdvanced)
      {
         // RAII door lock helper. We could pull in header from SampleProgram (but thats a bit unstructured)
         class RAII_physical_lock
         {
            CdromDevice& m_cdr;

         public:
            RAII_physical_lock(CdromDevice& cdrom) noexcept : m_cdr(cdrom)
            {
               m_cdr.lock();
            }
            ///<summary> deleted copy constructor.</summary>
            RAII_physical_lock(RAII_physical_lock& other) = delete;

            ///<summary> deleted move constructor.</summary>
            RAII_physical_lock(RAII_physical_lock&& other) = delete;

            ///<summary> deleted copy assignment.</summary>
            RAII_physical_lock& operator=(RAII_physical_lock& other) = delete;

            ///<summary> deleted move assignment.</summary>
            RAII_physical_lock& operator=(RAII_physical_lock&& other) = delete;

            ~RAII_physical_lock()
            {
               m_cdr.unlock();
            }
         };

         // issue IOCTL_STORAGE_EJECT_MEDIA while PREVENT_MEDIA_REMOVAL is/is not enacted...
         try
         {
            std::string device_path = DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0];
            CdromDevice cdrom0(device_path);
            CdromDevice cdrom1(device_path);

            int retries = 10;

            while (retries-- > 0)
            {
               try
               {
                  // perpare for test (lock the tray door)...
                  RAII_physical_lock scoped_lock(cdrom0);

                  // perform the operation under test USING A DIFERENT OBJECT INSTANCE THIS TIME...
                  cdrom1.eject();

                  // check results (expect fail with invalid function)
                  utf8::Assert::Fail("tray door could be opened despite being locked");
               }
               catch (const std::exception& e)
               {
                  switch (SystemError().get_error_code())
                  {
                  case ERROR_INVALID_FUNCTION:        // good (eject is invalid if device is locked)
                  case ERROR_SUCCESS:                 // since WINDOWS 10 1803
                  case ERROR_ACCESS_DENIED:           // since we claimed exclusive access
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

      TEST_METHOD(TestCdromTrayLockingMultiThreaded)
      {
         // RAII door lock helper. We could pull in header from SampleProgram (but thats a bit unstructured)
         class RAII_physical_lock
         {
            CdromDevice& m_cdr;

         public:
            RAII_physical_lock(CdromDevice& cdrom) noexcept : m_cdr(cdrom)
            {
               m_cdr.lock();
            }
            ///<summary> deleted copy constructor.</summary>
            RAII_physical_lock(RAII_physical_lock& other) = delete;

            ///<summary> deleted move constructor.</summary>
            RAII_physical_lock(RAII_physical_lock&& other) = delete;

            ///<summary> deleted copy assignment.</summary>
            RAII_physical_lock& operator=(RAII_physical_lock& other) = delete;

            ///<summary> deleted move assignment.</summary>
            RAII_physical_lock& operator=(RAII_physical_lock&& other) = delete;

            ~RAII_physical_lock()
            {
               m_cdr.unlock();
            }
         };

         // issue IOCTL_STORAGE_EJECT_MEDIA while PREVENT_MEDIA_REMOVAL is/is not enacted...
         try
         {
            std::string device_path = DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0];
            CdromDevice cdrom0(device_path);
            CdromDevice cdrom1(device_path);

            int retries = 10;

            while (retries-- > 0)
            {
               try
               {
                  // perpare for test (lock the tray door)...
                  RAII_physical_lock scoped_lock(cdrom0);

                  // perform the operation under test USING A DIFERENT THREAD THIS TIME...
                  auto future = std::async(std::launch::async, [&cdrom1] { cdrom1.eject(); return true; });

                  if (!future.get())
                  {
                     //future should return true or get() should rethrow the exception (so we should never end up here)
                     utf8::Assert::Fail("test coding issue: the std::async predicate didn't operate as per design intent.");
                  }

                  // check results (expect fail with invalid function)
                  utf8::Assert::Fail("tray door could be opened despite being locked");

               }
               catch (const std::exception& e)
               {
                  switch (SystemError().get_error_code())
                  {
                  case ERROR_INVALID_FUNCTION:        // good (eject is invalid if device is locked)
                  case ERROR_SUCCESS:                 // since WINDOWS 10 1803
                  case ERROR_ACCESS_DENIED:           // since we claimed exclusive access
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


      TEST_METHOD(TestCdromTrayLockingMultiUser)
      {
         // RAII door lock helper. We could pull in header from SampleProgram (but thats a bit unstructured)
         class RAII_physical_lock
         {
            CdromDevice& m_cdr;

         public:
            RAII_physical_lock(CdromDevice& cdrom) noexcept : m_cdr(cdrom)
            {
               m_cdr.lock();
            }
            ///<summary> deleted copy constructor.</summary>
            RAII_physical_lock(RAII_physical_lock& other) = delete;

            ///<summary> deleted move constructor.</summary>
            RAII_physical_lock(RAII_physical_lock&& other) = delete;

            ///<summary> deleted copy assignment.</summary>
            RAII_physical_lock& operator=(RAII_physical_lock& other) = delete;

            ///<summary> deleted move assignment.</summary>
            RAII_physical_lock& operator=(RAII_physical_lock&& other) = delete;

            ~RAII_physical_lock()
            {
               m_cdr.unlock();
            }
         };

         class RAII_impersonate
         {

         public:
            RAII_impersonate() noexcept
            {
               if (!ImpersonateSelf(SecurityAnonymous))
               {
                  LOG_ERROR("RAII_impersonate ImpersonateSelf failed");
               }
            }

            ///<summary> deleted copy constructor.</summary>
            RAII_impersonate(RAII_impersonate& other) = delete;

            ///<summary> deleted move constructor.</summary>
            RAII_impersonate(RAII_impersonate&& other) = delete;

            ///<summary> deleted copy assignment.</summary>
            RAII_impersonate& operator=(RAII_impersonate& other) = delete;

            ///<summary> deleted move assignment.</summary>
            RAII_impersonate& operator=(RAII_impersonate&& other) = delete;

            ~RAII_impersonate()
            {
               RevertToSelf();
            }
         };

         // issue IOCTL_STORAGE_EJECT_MEDIA while PREVENT_MEDIA_REMOVAL is/is not enacted...
         try
         {
            std::string device_path = DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0];
            CdromDevice cdrom0(device_path);
            CdromDevice cdrom1(device_path);

            int retries = 10;

            while (retries-- > 0)
            {
               try
               {
                  // perpare for test (lock the tray door)...
                  RAII_physical_lock scoped_lock(cdrom0);

                  // TODO: perform the operation under test impersonating another user...
                  auto future = std::async(std::launch::async, [&cdrom1]
                     { 
                        RAII_impersonate I_am_my_own_imposter;
                        cdrom1.eject(); 
                        return true;
                     }
                  );

                  if (!future.get())
                  {
                     //future should return true or get() should rethrow the exception (so we should never end up here)
                     utf8::Assert::Fail("test coding issue: the std::async predicate didn't operate as per design intent.");
                  }

                  // check results (expect fail with invalid function)
                  utf8::Assert::Fail("tray door could be opened despite being locked");

               }
               catch (const std::exception& e)
               {
                  switch (SystemError().get_error_code())
                  {
                  case ERROR_INVALID_FUNCTION:        // good (eject is invalid if device is locked)
                  case ERROR_SUCCESS:                 // since WINDOWS 10 1803
                  case ERROR_ACCESS_DENIED:           // since we claimed exclusive access
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
            // prepare for test (construct a device for the system's first enumerated cdrom)...
            CdromDevice cdrom(DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0]);
                       
            int retries = 10;                          // cdrom drive may not be in ready state
            while (--retries > 0)
            {
               try
               {
                  // perform the operation under test (query device geometry and calculate media size)...
                  std::ignore = cdrom.get_image_size();
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
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }

#pragma warning(disable: 26485)
      BEGIN_TEST_METHOD_ATTRIBUTE(TestCdromDeviceReadImage)
         TEST_IGNORE()        // TestFunctor takes too long to run every time...
      END_TEST_METHOD_ATTRIBUTE()
#pragma warning(default: 26485)
      TEST_METHOD(TestCdromDeviceReadImage)
      {
         // RAII door lock helper
         class TrayDoorLock
         {
            CdromDevice& m_cdr;

         public:
            TrayDoorLock(CdromDevice& cdrom) noexcept : m_cdr(cdrom)
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
                  LOG_WARNING(e.what());
               }
            }
         };

         try
         {
            // prepare for test (construct a device for the system's first enumerated cdrom)...
            CdromDevice cdrom(DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0]);

            // ...create buffer. On x32 resize will throw std::exception("vector<T> too long) if CDROM/DVD image is too large for a byte vector
            std::vector<unsigned char> buffer;
            buffer.resize(gsl::narrow<std::vector<unsigned char>::size_type>(cdrom.get_image_size()));    

            // disable media eject (also operator initiated) during test
            TrayDoorLock scoped_lock(cdrom);

            bool test_done = false;    // cdrom drive may not be in ready state
            while (!test_done)
            {
               try
               {
                  // perform the operation under test (read full device content)...
                  std::atomic<int> progress;
                  cdrom.get_image(buffer, progress);    // doesn't require an explicit gsl::make_span<unsigned char>(buffer)
                  test_done = true;
               }
               catch (const std::exception& e)
               {
                  // other tests can appear to have 'swapped' media...
                  if (SystemError().get_error_code() != ERROR_MEDIA_CHANGED) throw e;
               }
            };

            // write to file (confirm by mounting iso)
            std::ofstream file("test.iso", std::ios::out | std::ios::binary);
            file.unsetf(std::ios::skipws); // Don't eat new lines in binary mode!!!
#pragma warning(disable : 26490)
            file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
#pragma warning(default : 26490)
            file.close();

            // check results (by hand) - image is good if file mounts and is readable.
         }
         catch (const std::exception& e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }
   };
}