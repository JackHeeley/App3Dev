//
// device_type_directory.cpp :  a platform independent way to refer to device types
//
// allows portability (ISO C++17 compiler) checks on business logic by decoupling 
// system specific GUIDs from known device types.
//
// Copyright (c) 2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
#include "stdafx.h"
#include "device_type_directory.hpp"
#include "utf8_win_convert.hpp"

#include <winioctl.h>

///<summary> the private implementation of DeviceTypeDirectory. A Singleton.</summary>
///<remarks> Motivation for using an inner class is to encapsulate/hide 'GUID' (a Windows specific type).</remarks>
class DeviceTypeDirectory::impl
{

private:
   ///<summary> a std::map relating all device types recognized by this implementation, to a recognised Windows platform specific representation.</summary>
   std::map<DeviceTypeDirectory::DeviceType, const GUID> device_type_map;

   ///<summary> constructs the singleton DeviceTypeDirectory::impl.</summary>
   impl() noexcept :
      device_type_map{}
   {
      try 
      {
         device_type_map.insert(std::make_pair(DeviceType::DISK_DEVICES, GUID_DEVINTERFACE_DISK));
         device_type_map.insert(std::make_pair(DeviceType::CDROM_DEVICES, GUID_DEVINTERFACE_CDROM));
         device_type_map.insert(std::make_pair(DeviceType::PARTITION_DEVICES, GUID_DEVINTERFACE_PARTITION));
         device_type_map.insert(std::make_pair(DeviceType::TAPE_DEVICES, GUID_DEVINTERFACE_TAPE));
         device_type_map.insert(std::make_pair(DeviceType::WRITEONCEDISK_DEVICES, GUID_DEVINTERFACE_WRITEONCEDISK));
         device_type_map.insert(std::make_pair(DeviceType::VOLUME_DEVICES, GUID_DEVINTERFACE_VOLUME));
         device_type_map.insert(std::make_pair(DeviceType::MEDIUMCHANGER_DEVICES, GUID_DEVINTERFACE_MEDIUMCHANGER));
         device_type_map.insert(std::make_pair(DeviceType::FLOPPY_DEVICES, GUID_DEVINTERFACE_FLOPPY));
         device_type_map.insert(std::make_pair(DeviceType::CDCHANGER_DEVICES, GUID_DEVINTERFACE_CDCHANGER));
         device_type_map.insert(std::make_pair(DeviceType::STORAGEPORT_DEVICES, GUID_DEVINTERFACE_STORAGEPORT));
         device_type_map.insert(std::make_pair(DeviceType::VMLUN_DEVICES, GUID_DEVINTERFACE_VMLUN));
         device_type_map.insert(std::make_pair(DeviceType::SES_DEVICES, GUID_DEVINTERFACE_SES));
      }
      catch (...)
      {
         //TODO: consider stderr here
      }
   }

public:

   ///<summary> static getInstance (singleton).</summary>
   static impl& getInstance() noexcept
   {
      static impl singleton;
      return singleton;
   }

   ///<summary> copy constructor deleted (singleton).</summary>
   impl(const impl& other) = delete;

   ///<summary> move constructor deleted (singleton).</summary>
   impl(impl&& other) noexcept = delete;

   ///<summary> no copy assignment operator (singleton).</summary>
   impl& impl::operator=(impl& other) = delete;

   ///<summary> no move assignment operator (singleton).</summary>
   impl& impl::operator=(impl&& other) = delete;

   ///<summary> explicit default destructor (rule of 5).</summary>
   ~impl() = default;

   std::string get_device_type_as_string(DeviceType aDeviceType)
   {
#pragma warning(disable: 26486)
      return utf8::win_convert::from_guid(device_type_map[aDeviceType]);
#pragma warning(default: 26486)
   }

};

/*
* ******************************************************************************
* Bridge to inner class - public interface to DeviceTypeDirectory implementation
* ******************************************************************************
*/

std::string DeviceTypeDirectory::get_device_type_as_string(DeviceType aDeviceType)
{
   return impl::getInstance().get_device_type_as_string(aDeviceType);
}