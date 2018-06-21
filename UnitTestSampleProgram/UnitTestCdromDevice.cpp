#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::chrono_literals;
using namespace utf8;

namespace UnitTestAbstractDevice
{
   TEST_CLASS(UnitTestCdromDevice)
   {
   public:

      TEST_METHOD(TestCdromDeviceConstructor)
      {
         try
         {
            // prepare for test (ensure suitable test hardware exists on this system)...
            DeviceDiscoverer cdrom_interface(GUID_DEVINTERFACE_CDROM);
            utf8::Assert::IsFalse(cdrom_interface.device_path_map.get()[0] == "", "no system cdrom devices were discovered");

            // perform the operation under test (construct a device for the system's first enumerated cdrom)...
            CdromDevice cdrom(DeviceDiscoverer(GUID_DEVINTERFACE_CDROM).device_path_map.get()[0]);

            // test succeeds if construction doesn't throw
            cdrom.unlock();
         }
         catch (std::exception& e)
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
            ///<summary> default copy constructor.</summary>
            TrayDoorLock(TrayDoorLock& other) = default;

            ///<summary> deleted move constructor.</summary>
            TrayDoorLock(TrayDoorLock&& other) = delete;

            ///<summary> default copy assignment.</summary>
            TrayDoorLock& operator=(TrayDoorLock& other) = default;

            ///<summary> deleted move assignment.</summary>
            TrayDoorLock& operator=(TrayDoorLock&& other) = delete;

#pragma warning (disable:26447)
            ~TrayDoorLock()
            {
               try
               {
                  m_cdr.unlock();
               }
               catch (...)
               {
                 // dismiss is still least bad option here
               }
            }
#pragma warning (default:26447)
         };

         // issue IOCTL_STORAGE_EJECT_MEDIA while PREVENT_MEDIA_REMOVAL is/is not enacted...
         try
         {
            std::string device_path = DeviceDiscoverer(GUID_DEVINTERFACE_CDROM).device_path_map.get()[0];
            CdromDevice cdrom0(device_path);

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
               catch (std::exception& e)
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
            catch (std::exception& e)
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
         catch (std::exception& e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }

      TEST_METHOD(TestCdromGetImageSize)
      {

         try
         {
            // prepare for test (construct a device for the system's first enumerated cdrom)...
            CdromDevice cdrom(DeviceDiscoverer(GUID_DEVINTERFACE_CDROM).device_path_map.get()[0]);
                       
            int retries = 10;                          // cdrom drive may not be in ready state
            while (--retries > 0)
            {
               try
               {
                  // perform the operation under test (query device geometry and calculate media size)...
                  const auto size = cdrom.get_image_size();
                  break;
               }
               catch (std::exception& e)
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
         catch (std::exception& e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }

//      TEST_METHOD(TestCdromDeviceReadImage)
//      {
//         utf8::Assert::Fail("Test deprecated (takes too long). Use <testapp> (tbs) instead, or comment out this fail.");
//
//         // RAII door lock helper
//         class TrayDoorLock
//         {
//            CdromDevice& m_cdr;
//
//         public:
//            TrayDoorLock(CdromDevice& cdrom) : m_cdr(cdrom)
//            {
//               m_cdr.lock();
//            }
//            ///<summary> default copy constructor.</summary>
//            TrayDoorLock(TrayDoorLock& other) = default;
//
//            ///<summary> deleted move constructor.</summary>
//            TrayDoorLock(TrayDoorLock&& other) = delete;
//
//            ///<summary> default copy assignment.</summary>
//            TrayDoorLock& operator=(TrayDoorLock& other) = default;
//
//            ///<summary> deleted move assignment.</summary>
//            TrayDoorLock& operator=(TrayDoorLock&& other) = delete;
//
//#pragma warning (disable:26447)
//            ~TrayDoorLock()
//            {
//               try
//               {
//                  m_cdr.unlock();
//               }
//               catch (...)
//               {
//                  // dismiss is still least bad option here
//               }
//            }
//#pragma warning (default:26447)
//         };
//
//         try
//         {
//            // prepare for test (construct a device for the system's first enumerated cdrom)...
//            CdromDevice cdrom(DeviceDiscoverer(GUID_DEVINTERFACE_CDROM).device_path_map.get()[0]);
//
//            // ...create buffer
//            std::vector<unsigned char> buffer;            // x32 and x64 vector sizes differ dramatically in capacity
//            buffer.resize(cdrom.get_image_size());        // x32 will fail here with large CDROM or DVD images
//
//            // diable media eject (also operator initiated) during test
//            TrayDoorLock scoped_lock(cdrom);
//
//            bool test_done = false;    // cdrom drive may not be in ready state
//            while (!test_done)
//            {
//               try
//               {
//                  // perform the operation under test (read full device content)...
//                  cdrom.get_image(buffer);    // doesnt require an explicit gsl::as_span<unsignedchar>(buffer)
//                  test_done = true;
//               }
//               catch (std::exception& e)
//               {
//                  // other tests can appear to have 'swapped' media...
//                  if (SystemError().get_error_code() != ERROR_MEDIA_CHANGED) throw e;
//               }
//            };
//
//            // write to file (confirm by mounting iso)
//            std::ofstream file("test.iso", std::ios::out | std::ios::binary);
//            file.unsetf(std::ios::skipws); // Don't eat new lines in binary mode!!!
//            file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
//            file.close();
//
//            // check results (by hand) - image is good if file mounts and is readable.
//         }
//         catch (std::exception& e)
//         {
//            utf8::Assert::IsTrue(false, e.what()); // something went wrong
//         }
//      }
   };
}