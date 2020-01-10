//
// UnitTestDeviceTypeDirectory.cpp : a utf8 everywhere component unit test 
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

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace utf8;

namespace UnitTestExtendedUniversalCppSupport
{
   TEST_CLASS(UnitTestDeviceTypeDirectory)
   {
   public:

#pragma warning(disable: 26440 26477 26497)
      TEST_CLASS_INITIALIZE(InitializeUnitDeviceTypeDirectory) noexcept
#pragma warning(default: 26440 26477 26497)
      {
         try
         {
            CREATE_LOGGER(logger_factory::logger_type::file_logger, log_file_name, LogFilter::Full);
         }
         catch (...)
         {
            LOG_ERROR("Couldn't create logger.");       // No logger? This will emit on std::cerr
         }
      }

      TEST_METHOD(TestDeviceTypeDirectoryGetDeviceTypeAsString)
      {
         try
         {
            // perform the operation under test (happy flow)...
            const GUID disk_guid(utf8::guid_convert::to_guid(DeviceTypeDirectory::get_device_type_as_string(DeviceTypeDirectory::DeviceType::DISK_DEVICES)));
           
            // check result (happy flow)  
            utf8::Assert::IsTrue((disk_guid == GUID_DEVINTERFACE_DISK), "Value return was not as expected");

            // do some more stuff that shouldn't throw
            const GUID cdrom_guid(utf8::guid_convert::to_guid(DeviceTypeDirectory::get_device_type_as_string(DeviceTypeDirectory::DeviceType::CDROM_DEVICES)));
            const GUID partition_guid(utf8::guid_convert::to_guid(DeviceTypeDirectory::get_device_type_as_string(DeviceTypeDirectory::DeviceType::PARTITION_DEVICES)));
            const GUID writeOnceDisk_guid(utf8::guid_convert::to_guid(DeviceTypeDirectory::get_device_type_as_string(DeviceTypeDirectory::DeviceType::WRITEONCEDISK_DEVICES)));
            const GUID MediumChanger_guid(utf8::guid_convert::to_guid(DeviceTypeDirectory::get_device_type_as_string(DeviceTypeDirectory::DeviceType::MEDIUMCHANGER_DEVICES)));
            const GUID floppy_guid(utf8::guid_convert::to_guid(DeviceTypeDirectory::get_device_type_as_string(DeviceTypeDirectory::DeviceType::FLOPPY_DEVICES)));
            const GUID cdchanger_guid(utf8::guid_convert::to_guid(DeviceTypeDirectory::get_device_type_as_string(DeviceTypeDirectory::DeviceType::CDCHANGER_DEVICES)));
            const GUID storageport_guid(utf8::guid_convert::to_guid(DeviceTypeDirectory::get_device_type_as_string(DeviceTypeDirectory::DeviceType::STORAGEPORT_DEVICES)));
            const GUID vmlun_guid(utf8::guid_convert::to_guid(DeviceTypeDirectory::get_device_type_as_string(DeviceTypeDirectory::DeviceType::VMLUN_DEVICES)));
            const GUID ses_guid(utf8::guid_convert::to_guid(DeviceTypeDirectory::get_device_type_as_string(DeviceTypeDirectory::DeviceType::SES_DEVICES)));
         }
         catch (const std::exception & e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }
   };
}