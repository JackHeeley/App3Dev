//
// device_discoverer.cpp : implements device driver interface mechanisms
//
// Here we enumerate the devices of a particular type (defined by
// an interface class) with the SetupDi interface, and provide a 
// method to get the device path(s) needed when opening device(s).
//
// These paths can be used to instantiate device objects.
//
// See: Microsoft Knowledge Base Article - 259695 
//
// Copyright (c) 2003-2020 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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
#include <setupapi.h>

#include "device_discoverer.hpp"
#include "utf8_guid.hpp"


/*
* ***************************************************************************
* PIMPL idiom - private implementation of DeviceDiscoverer class
* ***************************************************************************
*/

///<summary> the private implementation of DeviceDiscoverer.</summary>
///<remarks> Windows types used internally, adheres to "utf8 everywhere" paradigm at public interface</remarks>
class DeviceDiscoverer::impl
{
private:
   ///<summary> handle to the DEVINFO structure used.</summary>
   ///<remarks> used when enumerating devices via DeviceDiscoverer interface.</remarks>
   HDEVINFO m_hDevInfo;

public:
   ///<summary> class GUID that identifies the device interface (for standard devices see wioctl.h)</summary>
   ///<remarks> This is the same GUID as used by a device driver when calling IoRegisterDeviceInterface.</remarks>
   const GUID theGuid;

   ///<summary> pointer to class GUID as required by legacy api's</summary>
   LPCGUID INTERFACE_CLASS_GUID;

   ///<summary> the device path data member.</summary>
   std::map<int, std::string> device_path_data;

   ///<summary> construct a DeviceDiscoverer::private_impl object used to enumerate the devices of a particular device interface class.</summary>
   ///<param name = "aDeviceType"> the device type for the interface class to be enumerated.</param>
   impl(DeviceTypeDirectory::DeviceType aDeviceType) /*noexcept*/ :
      device_path_data(),
      theGuid(utf8::guid_convert::to_guid(DeviceTypeDirectory::get_device_type_as_string(aDeviceType))),
      INTERFACE_CLASS_GUID(&theGuid)
   {
      m_hDevInfo = getDevInfoHandle(INTERFACE_CLASS_GUID);

      SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
      deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

      int device_number = -1;
      while (getDeviceInterfaceData(++device_number, deviceInterfaceData))
      {
         device_path_data.insert(std::pair<int, std::string>(device_number, utf8::convert::from_utf16(getDevicePath(deviceInterfaceData))));
      }
   }

   ///<summary> equals comparison operator.</summary>
   ///<remarks> defines equals to mean accessible data is identical.</remarks>
   const bool impl::operator==(const impl& other) const
   {
      if ((this == nullptr) || (&other == nullptr)) return false;       // moved object
      if (this == &other) return true; // same object
      return (
         (INTERFACE_CLASS_GUID == other.INTERFACE_CLASS_GUID) &&
         (device_path_data == other.device_path_data)    
         // considered equal if public data is identical
      );
   }

   ///<summary> not equals comparison operator.</summary>
   ///<remarks> defines not equals to mean accessible data differs.</remarks>
   const bool impl::operator!=(const impl& other) const
   {
      return (!((*this) == other));
   }
      
   ///<summary> copy constructor.</summary>
   impl(const impl& other) = default;
      
   ///<summary> move constructor.</summary>
   impl(impl&& other) = default;

   ///<summary> copy assignment operator</summary>
   impl& impl::operator=(impl& other) = default;

   ///<summary> move assignment operator </summary>
   impl& impl::operator=(impl&& other) = default;
   
   ///<summary> dtor release resources used by the class.</summary>
   ~impl()
   {
      SetupDiDestroyDeviceInfoList(m_hDevInfo); //TODO : after moves, there is a redundant extra call (which is harmless in this case)
   }

private:
   ///<summary> get handle to device information set corresponding to the interface class guid supplied.</summary>
   ///<param name ="anInterfaceClassGuid"> pointer to setup interface class guid or device class guid to interrogate.</param>
   const HDEVINFO getDevInfoHandle(LPCGUID anInterfaceClassGuid) const
   {
      constexpr DWORD Flags = DIGCF_PRESENT | DIGCF_DEVICEINTERFACE;

      HDEVINFO hDI = SetupDiGetClassDevs(anInterfaceClassGuid,
         nullptr,
         nullptr,
         Flags
      );

      if (hDI == INVALID_HANDLE_VALUE)
      {
         throw error_context("SetUpDiGetClassDevs failed");
      }
      return hDI;
   }

   ///<summary> Get deviceInterfaceData for this interface class.</summary>
   ///<param name ="memberIndex"> zero based device number for the device instance.</param>
   ///<returns> true if device interface data is found for this device.</returns>
   const bool getDeviceInterfaceData(int memberIndex, SP_DEVICE_INTERFACE_DATA& deviceInterfaceData) const
   {
      const BOOLEAN result =
         SetupDiEnumDeviceInterfaces(m_hDevInfo,
            nullptr,
            INTERFACE_CLASS_GUID,
            memberIndex,
            &deviceInterfaceData
         );

      if (!result)
      {
         const int errorCode = GetLastError();
         if (errorCode != ERROR_NO_MORE_ITEMS)
         {
            throw error_context("SetupDiEnumDeviceInterfaces failed");
         }
      }

      return (result == TRUE);
   }

   ///<summary> Get deviceInterfaceDetailData for a particular instance of a device of this interface class.</summary>
   ///<returns> std::wstring form of the device path (the specific device interface detail data fetched from DeviceDiscoverer).</returns>
   const std::wstring getDevicePath(SP_DEVICE_INTERFACE_DATA deviceInterfaceData) const
   {
      ULONG predictedLength = 0;
      ULONG requiredLength = 0;

      //
      // First find out required length of the buffer
      //
      const BOOLEAN result =
         SetupDiGetInterfaceDeviceDetail(m_hDevInfo,
            &deviceInterfaceData,
            NULL,    // probing so no output buffer yet
            0,       // probing so output buffer length of zero
            &requiredLength,
            NULL     // not interested in the specific dev-node
         );

      if (!result) 
      {
         const int errorCode = GetLastError();
         if (errorCode != ERROR_INSUFFICIENT_BUFFER) 
         {
            throw error_context("Probing SetupDiGetInterfaceDeviceDetail failed");
         }
      }

      predictedLength = requiredLength;

      //
      // allocate and initialize the buffer
      //
      auto buffer = std::vector<unsigned char>(predictedLength);
#pragma warning(disable : 26490)
      PSP_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(buffer.data());
#pragma warning(default : 26490)

      pDeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

      if(!SetupDiGetInterfaceDeviceDetail(m_hDevInfo,
         &deviceInterfaceData,
         pDeviceInterfaceDetailData,
         predictedLength,
         &requiredLength,
         NULL )
         )
      {
         throw error_context("SetupDiGetInterfaceDeviceDetail failed");
      }
#pragma warning(disable : 26485)
      return std::wstring(pDeviceInterfaceDetailData->DevicePath); // copies buffer content before its destroyed
#pragma warning(default : 26485)
   }
};


/*
* ***************************************************************************
* PIMPL idiom - public interface for DeviceDiscoverer implementation 
* ***************************************************************************
*/

///<summary> constructs a DeviceDiscoverer for the chosen device interface class.</summary>
///<param name='aDeviceType'>value representing a chosen device type</param>
DeviceDiscoverer::DeviceDiscoverer(DeviceTypeDirectory::DeviceType aDeviceType) noexcept :
#pragma warning (disable:26447)
   pimpl(spimpl::make_impl<impl>(aDeviceType)),
#pragma warning (default:26447)
   device_path_map(std::ref<std::map<int,std::string>>(pimpl->device_path_data))
{
}

///<summary> equals comparison operator.</summary>
///<remarks> objects considered equal if private_impl's are equal.</remarks>
const bool DeviceDiscoverer::operator==(const DeviceDiscoverer& other) const
{
   return *pimpl.get() == *other.pimpl.get();
}

///<summary> not equals comparison operator.</summary>
///<remarks> objects considered not equal if private_impl's differ.</remarks>
const bool DeviceDiscoverer::operator!=(const DeviceDiscoverer& other) const
{
   return !(*this == other);
}