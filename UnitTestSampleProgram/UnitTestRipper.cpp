#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::chrono_literals;
using namespace utf8;

namespace UnitTestExtendedUniversalCppSupport
{
   TEST_CLASS(UnitTestRipper)
   {
   public:

      TEST_METHOD(TestFunctor)
      {
         try
         {
            // check test preconditions (at least one physical cdrom needed)
            DeviceDiscoverer cdromDevices(GUID_DEVINTERFACE_CDROM);
            utf8::Assert::IsFalse(cdromDevices.device_path_map.get().empty(), "no system cdrom devices were discovered");

            //prepare test 
            auto deviceName = cdromDevices.device_path_map.get()[0];
            const static std::string fileName("cdrom_image.iso");

            //perform operation under test
            Ripper(deviceName, fileName)();
         }
         catch (std::exception& e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }
   };
}