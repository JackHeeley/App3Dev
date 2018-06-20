//
// device_discoverer.hpp : implements device driver interface mechanisms
//
// At construction time we enumerate the devices of a particular type 
// (defined by an interface class guid), and provide a mechanism to get
// the device path(s) needed when opening device(s) of this type.
//
// See: Microsoft Knowledge Base Article - 259695 
//
// Copyright (c) 2003-2017 Jack Heeley, all rights reserved
//
#pragma once

#ifdef EXTENDEDUNIVERSALCPPSUPPORT_EXPORTS
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <string>
#include <exception>
#include <vector>
#include <memory>
#include <map>
#include <spimpl.hpp>

#pragma warning (disable:4251 4290)

///<summary> wraps the system device interface.</summary>
class EXTENDEDUNIVERSALCPPSUPPORT_API DeviceDiscoverer {

public:
   ///<summary> construct an interface object used to enumerate the devices of a particular device interface class.</summary>
   ///<param name = "anInterfaceClassGuid"> the device interface class to be enumerated.</param>
   DeviceDiscoverer(const GUID& anInterfaceClassGuid) throw (std::exception);

   ///<summary> equals comparison operator.</summary>
   ///<remarks> defines equals to mean identical device path content.</remarks>
   const bool DeviceDiscoverer::operator==(const DeviceDiscoverer& other) const;

   ///<summary> not equals comparison operator.</summary>
   ///<remarks> defines not equals to mean differing device path content.</remarks>
   const bool DeviceDiscoverer::operator!=(const DeviceDiscoverer& other) const;

private:
   ///<summary> forward reference to private implementation.</summary>
   class impl;

   ///<summary> unique pointer to private implementation.</summary>
   ///<remarks> with default copy, move and compare support.</remarks>
   spimpl::impl_ptr<impl> impl_;

public:
   ///<summary> references a std::map listing the device paths for the system devices corresponding to 
   /// the interface class GUID supplied at construction time. The map contains entries for all device 
   /// instances discovered at construction time. Entries are std::strings containing utf8 encoded content.</summary>
   /// <remarks> unlike a raw reference, std::reference_wrapper is copyable (a feature we need to support DeviceDiscoverer
   /// move operations with the current implementation choices).</remarks>
   std::reference_wrapper<std::map<int, std::string>> device_path_map;

};

#pragma warning (default:4251 4290)


