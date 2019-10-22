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

      TEST_METHOD(TestDeviceConstructor)
      {
         try
         {
            // prepare for test (ensure suitable test hardware exists on this system)...
            DeviceDiscoverer cdrom_interface(GUID_DEVINTERFACE_CDROM);
            utf8::Assert::IsFalse(cdrom_interface.device_path_map.get().empty(), "no system cdrom devices were discovered");

            // perform the operation under test (construct a device for the system's first enumerated cdrom)...
            Device cdrom(DeviceDiscoverer(GUID_DEVINTERFACE_CDROM).device_path_map.get()[0]);

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
            Device cdrom(DeviceDiscoverer(GUID_DEVINTERFACE_CDROM).device_path_map.get()[0]);

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
            Device cdrom(DeviceDiscoverer(GUID_DEVINTERFACE_CDROM).device_path_map.get()[0]);

            // cdroms are only readable in exact block size units that depend on disk geometry
            // this test hard codes a safe size (doesn't query actual geometry of the disk)
            constexpr int BLOCK_SIZE = 4096;     
            std::vector<unsigned char>buffer;
            buffer.reserve(BLOCK_SIZE);

            uint32_t nBytesReturned = 0;

            // this test may start before CD is ready (after another test potentially changed the cd)
            int retries = 10;                          // cdrom drive may not be in ready state
            while (--retries > 0)
            {
               try
               {
                  // perform the operation under test (issue a read on the device)...
                  nBytesReturned = cdrom.read(buffer.data(), BLOCK_SIZE);
                  break;
               }
               catch (const std::exception& e)
               {
                  switch (SystemError().get_error_code())
                  {
                  case ERROR_NOT_READY:
                  case ERROR_MEDIA_CHANGED:
                  case ERROR_IO_DEVICE:
                     std::this_thread::sleep_for(2s); // not ready conditions, expected in some test sequences
                     break;

                  default: throw e;
                  }
               }
            }

            // check results...
            SystemError lastError;

            //FIXME: lastError persists after successes?
            if (lastError.get_error_code() == ERROR_SUCCESS)
            {
               utf8::Assert::IsTrue(nBytesReturned == BLOCK_SIZE, "read returned an unexpected byte count");
            }
            else
            {
               utf8::Assert::IsTrue(false, lastError.get_error_text().c_str());
            }

         }
         catch (const std::exception& e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }
   };
}