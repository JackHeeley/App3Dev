//
// device_discoverer.hpp : implements device driver interface mechanisms
//
// At construction time we enumerate the devices of a particular type 
// (defined by an interface class guid), and provide a mechanism to get
// the device path(s) needed when opening device(s) of this type.
//
// See: Microsoft Knowledge Base Article - 259695 
//
// Copyright (c) 2003-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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
#pragma once

#ifdef EXTENDEDUNIVERSALCPPSUPPORT_EXPORTS
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <map>
#include <string>
#include <spimpl.hpp>

#include "device_type_directory.hpp"

///<summary> wraps the system device interface.</summary>
class DeviceDiscoverer {

public:
   ///<summary> construct an interface object used to enumerate the devices of a particular device interface class.</summary>
   ///<param name = "aDeviceType"> the device type to be enumerated.</param>
   EXTENDEDUNIVERSALCPPSUPPORT_API DeviceDiscoverer(DeviceTypeDirectory::DeviceType aDeviceType) noexcept;

   ///<summary> equals comparison operator.</summary>
   ///<remarks> defines equals to mean identical device path content.</remarks>
   EXTENDEDUNIVERSALCPPSUPPORT_API const bool DeviceDiscoverer::operator==(const DeviceDiscoverer& other) const;

   ///<summary> not equals comparison operator.</summary>
   ///<remarks> defines not equals to mean differing device path content.</remarks>
   EXTENDEDUNIVERSALCPPSUPPORT_API const bool DeviceDiscoverer::operator!=(const DeviceDiscoverer& other) const;

private:
   ///<summary> forward reference to private implementation.</summary>
   class impl;

   ///<summary> unique pointer to private implementation.</summary>
   ///<remarks> with default copy, move and compare support.</remarks>
   spimpl::impl_ptr<impl> pimpl;

public:
   ///<summary> references a std::map listing the device paths for the system devices corresponding to 
   /// the device type supplied at construction time. The map contains entries for all device 
   /// instances discovered at construction time. Entries are std::strings containing utf8 encoded content.</summary>
   /// <remarks> unlike a raw reference, std::reference_wrapper is copyable (a feature we need to support DeviceDiscoverer
   /// move operations with the current implementation choices). </remarks>
   std::reference_wrapper<std::map<int, std::string>> device_path_map;
};
