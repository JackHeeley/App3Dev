//
// device_discoverer.cpp : implements device driver interface mechanisms
//
// Here we enumerate the devices of a particular type (defined by
// an interface class) using the SetupDi interface, and provide a 
// method to get the device path(s) needed when opening device(s).
//
// These paths can be used to instantiate device objects.
//
// See: Microsoft Knowledge Base Article - 259695 
//
// Copyright (c) 2003-2016 Jack Heeley, all rights reserved
//
#include "stdafx.h"
#include "device_discoverer.hpp"

#include <setupapi.h>

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
   LPGUID INTERFACE_CLASS_GUID;

   ///<summary> the device path data member.</summary>
   std::map<int, std::string> device_path_data;

   ///<summary> construct a DeviceDiscoverer::private_impl object used to enumerate the devices of a particular device interface class.</summary>
   ///<param name = "anInterfaceClassGuid"> the device interface class to be enumerated.</param>
   impl(const GUID& anInterfaceClassGuid) :
      device_path_data()
   {
      INTERFACE_CLASS_GUID = (LPGUID)&anInterfaceClassGuid;
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
   bool impl::operator==(const impl& other) const
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
   bool impl::operator!=(const impl& other) const
   {
      return (!((*this) == other));
   }

   ///<summary> dtor release resources used by the class.</summary>
   ~impl()
   {
      SetupDiDestroyDeviceInfoList(m_hDevInfo);
   }

private:
   ///<summary> get handle to device information set corresponding to the interface class guid supplied.</summary>
   ///<param name ="anInterfaceClassGuid"> pointer to setup interface class guid or device class guid to interrogate.</param>
   HDEVINFO getDevInfoHandle(LPGUID anInterfaceClassGuid)
   {
      DWORD Flags = DIGCF_PRESENT | DIGCF_DEVICEINTERFACE;

      HDEVINFO hDI = SetupDiGetClassDevs(anInterfaceClassGuid,
         NULL,
         NULL,
         Flags
      );

      if (hDI == INVALID_HANDLE_VALUE)
      {
         throw fox_exception("SetUpDiGetClassDevs failed");
      }
      return hDI;
   }

   ///<summary> Get deviceInterfaceData for this interface class.</summary>
   ///<param name ="memberIndex"> zero based device number for the device instance.</param>
   ///<returns> true if device interface data is found for this device.</returns>
   bool getDeviceInterfaceData(int memberIndex, SP_DEVICE_INTERFACE_DATA& deviceInterfaceData)
   {
      BOOLEAN result =
         SetupDiEnumDeviceInterfaces(m_hDevInfo,
            0,
            INTERFACE_CLASS_GUID,
            memberIndex,
            &deviceInterfaceData
         );

      if (!result)
      {
         int errorCode = GetLastError();
         if (errorCode != ERROR_NO_MORE_ITEMS)
         {
            throw fox_exception("SetupDiEnumDeviceInterfaces failed");
         }
      }

      return (result == TRUE);
   }

   ///<summary> Get deviceInterfaceDetailData for a particular instance of a device of this interface class.</summary>
   ///<returns> std::wstring form of the device path (the specific device interface detail data fetched from DeviceDiscoverer).</returns>
   std::wstring getDevicePath(SP_DEVICE_INTERFACE_DATA deviceInterfaceData)
   {
      ULONG predictedLength = 0;
      ULONG requiredLength = 0;

      //
      // First find out required length of the buffer
      //
      BOOLEAN result =
         SetupDiGetInterfaceDeviceDetail(m_hDevInfo,
            &deviceInterfaceData,
            NULL,    // probing so no output buffer yet
            0,       // probing so output buffer length of zero
            &requiredLength,
            NULL     // not interested in the specific dev-node
         );

      if (!result) 
      {
         int errorCode = GetLastError();
         if (errorCode != ERROR_INSUFFICIENT_BUFFER) 
         {
            throw fox_exception("Probing SetupDiGetInterfaceDeviceDetail failed");
         }
      }

      predictedLength = requiredLength;

      //
      // allocate and initialize the buffer
      //
      auto buffer = std::vector<unsigned char>(predictedLength);

      PSP_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)buffer.data();
      pDeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

      if(!SetupDiGetInterfaceDeviceDetail(m_hDevInfo,
         &deviceInterfaceData,
         pDeviceInterfaceDetailData,
         predictedLength,
         &requiredLength,
         NULL )
         )
      {
         throw fox_exception("SetupDiGetInterfaceDeviceDetail failed");
      }

      return std::wstring(pDeviceInterfaceDetailData->DevicePath); // copies buffer content before its destroyed
   }
};


/*
* ***************************************************************************
* PIMPL idiom - public interface for DeviceDiscoverer implementation 
* ***************************************************************************
*/

///<summary> constructs a DeviceDiscoverer for the chosen device interface class.</summary>
///<param name='anInterfaceClassGuid'>guid representing the system interface class for a chosen device type</param>
DeviceDiscoverer::DeviceDiscoverer(const GUID& anInterfaceClassGuid) :
   impl_(spimpl::make_impl<impl>(anInterfaceClassGuid)),
   device_path_map(std::ref<std::map<int,std::string>>(impl_->device_path_data))
{
}

///<summary> equals comparison operator.</summary>
///<remarks> objects considered equal if private_impl's are equal.</remarks>
bool DeviceDiscoverer::operator==(const DeviceDiscoverer& other) const
{
   return *impl_.get() == *other.impl_.get();
}

///<summary> not equals comparison operator.</summary>
///<remarks> objects considered not equal if private_impl's differ.</remarks>
bool DeviceDiscoverer::operator!=(const DeviceDiscoverer& other) const
{
   return !(*this == other);
}