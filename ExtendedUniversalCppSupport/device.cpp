//
// device.cpp : implements device driver access methods.
//
// Here we implement methods which wrap the Win32 calls. This offloads the
// parameterization of these functions from clients, for ease of use.
//
// Copyright (c) 2003-2016 Jack Heeley, all rights reserved
//
#include "stdafx.h"
#include "device.hpp"

/*
* ***************************************************************************
* PIMPL idiom - private implementation of Device class
* ***************************************************************************
*/

///<summary> the private implementation of Device.</summary>
///<remarks> Windows types used internally, adheres to "utf8 everywhere" paradigm at public interface</remarks>
class Device::impl
{

private:
   ///<summary>the wstring form of the device path and the memory</summary>
   const std::wstring device_path_w;

   ///<summary> handle to the (open) device.</summary>
   HANDLE hDevice;

public:
   ///<summary> constructs a user mode Device that can be used to access a particular system device instance.</summary>
   ///<param name='a_device_path'> the system name of the device to use.</param>
   ///<exception cref='std::exception'>if construction fails.</exception>
   impl(std::string a_device_path) :
      device_path_w(utf8::convert::to_utf16(a_device_path)),
      hDevice(INVALID_HANDLE_VALUE)
   {
      open();
   }

   ///<summary> copy constructor.</summary>
   ///<remarks>copies device name, and opens another handle on device</remarks>
   impl(const impl& other) :
      device_path_w(other.device_path_w),
      hDevice(INVALID_HANDLE_VALUE)
   {
      open();
   }

   ///<summary> move constructor.</summary>
   ///<remarks>copies device name, and takes ownership of other.hDevice</remarks>
#pragma warning (disable:26439)
   impl(impl&& other) /*noexcept*/  :
      device_path_w(other.device_path_w),
      hDevice(other.hDevice)
   {
      other.hDevice = INVALID_HANDLE_VALUE; // avoid double or premature CloseHandle
   }
#pragma warning (default:26439)

   ///<summary> no copy assignment operator (class has const member)</summary>
   impl& impl::operator=(impl& other) = delete;
 
   ///<summary> no move assignment operator (class has const member)</summary>
   impl& impl::operator=(impl&& other) = delete;
   
   ///<summary> destructor</summary> 
   ///<remarks>If the handle to the device is open, it will be closed here.</remarks>
   ~impl() 
   {
      close();
   }

   ///<summary> open device for win32 device control and file i/o access.</summary>
   ///<exception cref='std::exception'>if the operation cannot be completed.</exception>
   void open()
   {
      const DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
      const DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
      //LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;
      const DWORD dwCreateDisposition = OPEN_EXISTING;
      const DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
      HANDLE hTemplateFile = NULL;

      hDevice = CreateFile(device_path_w.c_str(),
         dwDesiredAccess,
         dwShareMode,
         NULL, //lpSecurityAttributes,
         dwCreateDisposition,
         dwFlagsAndAttributes,
         hTemplateFile
      );

      if (hDevice == INVALID_HANDLE_VALUE) 
      {
         std::stringstream create_file_failed; create_file_failed << "CreateFile(\"" << utf8::convert::from_utf16(device_path_w) << "\", ...) failed";
         throw error_context(create_file_failed.str().c_str());
      }
   }

   /// <summary> issue a synchronous device i/o control message. The thread is suspended until this request completes.</summary>
   ///<param name ='dwIoControlCode'> Specifies the IOCTL_XXX to be set up. For more information about system specific device-type-specific I/O codes, 
   /// see the appropriate system reference. Eg for Windows this is the Windows DDK Kernel Mode Driver Design Guide Reference Part II.</param>
   ///<param name='lpInBuffer'> Points to an input buffer to be passed to the driver or nullptr if the request does not pass input data.</param>
   ///<param name='nInBufferSize'> Specifies the length in bytes of the input buffer. If InputBuffer is nullptr, this value must be zero.</param>
   ///<param name='lpOutBuffer'> Points to an output buffer in which the driver is to return data or NULL if the request does not require driver to return data.</param>
   ///<param name='nOutBufferSize'> Specifies the length in bytes of the output buffer. If OutputBuffer is NULL, this value must be zero.</param>
   ///<returns> actual number of bytes transferred in the operation.</returns>
   ///<exception cref='std::exception'> if the operation could not be completed.</exception>
   const DWORD ioctl(DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize) const
   {

      if (hDevice == INVALID_HANDLE_VALUE) 
      {
         throw error_context("Invalid handle");
      }

      DWORD nBytesReturned = 0;

      if (!DeviceIoControl(hDevice,
         dwIoControlCode,
         lpInBuffer,
         nInBufferSize,
         lpOutBuffer,
         nOutBufferSize,
         &nBytesReturned,
         NULL
      ))
      {
         throw error_context("DeviceIoControl failed");
      }

      return nBytesReturned;
   }

   ///<summary> issue a synchronous read. The thread is suspended pending completion of the read.</summary>
   ///<param name='lpBuffer'> pointer to buffer which will receive read data.</param>
   ///<param name='nBytesToRead'> number of bytes to read. this must be less than or equal to the available memory at lpBuffer.</param>
   ///<returns> actual number of bytes transferred/read.</returns>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   const DWORD read(LPVOID lpBuffer, DWORD numberOfBytesToRead) const
   {

      if (hDevice == INVALID_HANDLE_VALUE) 
      {
         throw error_context("Invalid handle");
      }

      DWORD numberOfBytesRead = 0;

      if (!ReadFile(hDevice,
         lpBuffer,
         numberOfBytesToRead,
         &numberOfBytesRead,
         NULL
      ))
      {
         throw error_context("ReadFile failed");
      }
      return numberOfBytesRead;
   }

   ///<summary> issue a synchronous write. The thread is suspended pending completion of the write.</summary>
   ///<param name='lpBuffer'> pointer to buffer containing data to write.</param>
   ///<param name='nBytesToWrite'> number of bytes to write from the buffer.</param>
   ///<returns> actual number of bytes transferred/written.</returns>
   ///<exception cref='std::exception'> if the operation could not be completed.</exception>
   const DWORD write(LPVOID lpBuffer, DWORD numberOfBytesToWrite) const
   {
 
      if (hDevice == INVALID_HANDLE_VALUE) 
      {
         throw error_context("Invalid handle");
      }

      DWORD numberOfBytesWritten = 0;

      if (!WriteFile(hDevice,
         lpBuffer,
         numberOfBytesToWrite,
         &numberOfBytesWritten,
         NULL
      ))
      {
         throw error_context("WriteFile failed");
      }
      return numberOfBytesWritten;
   }

   ///<summary> reset the device.</summary>
   ///<remarks> this is equivalent to close/open sequence and is predicated on the system 
   /// device implementing a "reset on open" semantics. This condition is not guaranteed.
   /// Devices that do not support this semantics may provide an ioctl to perform reset (see ioctl)</remarks>
   ///<exception cref='std::exception'> if the operation could not be completed.</exception>
   void reset() 
   {
      close();
      open();  // implies we implement reset on open semantics in driver
   }

   ///<summary> close the device for device control and file i/o operations.</summary>
   void close() noexcept
   {
      if (hDevice != INVALID_HANDLE_VALUE) 
      {
         CloseHandle(hDevice);
         hDevice = INVALID_HANDLE_VALUE;
      }
   }
};

/*
* ***************************************************************************
* PIMPL idiom - public interface for Device implementation
* ***************************************************************************
*/

///<summary> constructs a movable user mode Device that can be used to access a particular system device instance.</summary>
///<param name='a_device_path'> the system name of the device to use.</param>
///<exception cref='std::exception'> if construction fails.</exception>
///<remarks>opens the device</remarks>
Device::Device(std::string a_device_path) : 
   impl_(spimpl::make_unique_impl<impl>(a_device_path))
{
}

const std::uint32_t Device::ioctl(std::uint32_t dwIoControlCode, void* lpInBuffer, std::uint32_t nInBufferSize, void* lpOutBuffer, std::uint32_t nOutBufferSize) const throw()
{
   return impl_->ioctl(dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize);
}

const std::uint32_t Device::read(void* lpBuffer, std::uint32_t nBytesToRead) const throw()
{
   return impl_->read(lpBuffer, nBytesToRead);
}

const std::uint32_t Device::write(void* lpBuffer, std::uint32_t nBytesToWrite) const throw()
{
   return impl_->write(lpBuffer, nBytesToWrite);
}

void Device::reset()
{
   impl_->reset();
}