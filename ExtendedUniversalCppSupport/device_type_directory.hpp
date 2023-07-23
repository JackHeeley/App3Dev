//
// device_type_directory.hpp : a platform independent way to refer to device types
//
// allows portability (ANSI CPP compiler) checks on business logic by decoupling 
// system specific GUIDs from known device types.
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
#ifndef __DEVICE_TYPE_DIRECTORY_HPP__
#define __DEVICE_TYPE_DIRECTORY_HPP__

#ifdef EXTENDEDUNIVERSALCPPSUPPORT_EXPORTS
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <string>

#include <spimpl.hpp>

///<summary> wraps the system device interface.</summary>
class EXTENDEDUNIVERSALCPPSUPPORT_API DeviceTypeDirectory {

public:
   ///<summary> an enumeration providing platform agnostic names for all the device types recognized by this implementation.</summary>
   ///<remarks> this is just an arbitrary sample, chosen from a wider set available to windows platforms (winioctl.h)</remarks>
   enum class DeviceType : int
   {
      DISK_DEVICES = 0x00,
      CDROM_DEVICES = 0x01,
      PARTITION_DEVICES = 0x02,
      TAPE_DEVICES = 0x03,
      WRITEONCEDISK_DEVICES = 0x04,
      VOLUME_DEVICES = 0x05,
      MEDIUMCHANGER_DEVICES = 0x06,
      FLOPPY_DEVICES = 0x07,
      CDCHANGER_DEVICES = 0x08,
      STORAGEPORT_DEVICES = 0x09,
      VMLUN_DEVICES = 0x0a,
      SES_DEVICES = 0x0b
   };

   ///<summary> lookup the system representation of a device type.</summary>
   ///<param name='aDeviceType'> any of the supported device types.</param>
   ///<returns> an opaque string representing the device type to the system.</returns>
   static std::string get_device_type_as_string(DeviceType aDeviceType);

private:
   ///<summary> construct a singleton to map all supported DeviceTypes to corresponding system representations.</summary>
   DeviceTypeDirectory() = default;

   ///<summary> forward reference to a private (singleton) inner implementation class.</summary>
   ///<remarks> inner class decouples this header from the system types needed by the implementation.</remarks>
   class impl;

};

#endif // __DEVICE_TYPE_DIRECTORY_HPP__
