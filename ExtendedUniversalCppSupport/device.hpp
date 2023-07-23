//
// device.hpp : implements device driver access methods.
//
// Here we implement methods wrapping system calls. This offloads the
// parameterization of these functions from clients, for ease of use.
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
#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#ifdef EXTENDEDUNIVERSALCPPSUPPORT_EXPORTS
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <string>

#include <spimpl.hpp>

///<summary> represents a movable abstract physical system device.</summary>  
///<remarks> we explicitly disallow copy, and compare of devices as these operations have no great value.</remarks>
class Device {

public:

   ///<summary> constructs a movable user mode Device that can be used to access a particular system device instance.</summary>
   ///<param name='device_path'> the system name of the device to use.</param>
   ///<exception cref='std::exception'>if construction fails.</exception>
   EXTENDEDUNIVERSALCPPSUPPORT_API Device(const std::string& device_path);

   /// <summary> issue a synchronous device i/o control message. The thread is suspended until this request completes.</summary>
   ///<param name ='dwIoControlCode'> Specifies the IOCTL_XXX to be set up. For more information about system specific device-type-specific I/O codes, 
   /// see the appropriate system reference. Eg for Windows this is the Windows DDK Kernel Mode Driver Design Guide Reference Part II.</param>
   ///<param name='lpInBuffer'> Points to an input buffer to be passed to the driver or nullptr if the request does not pass input data.</param>
   ///<param name='nInBufferSize'> Specifies the length in bytes of the input buffer. If InputBuffer is nullptr, this value must be zero.</param>
   ///<param name='lpOutBuffer'> Points to an output buffer in which the driver is to return data or NULL if the request does not require driver to return data.</param>
   ///<param name='nOutBufferSize'> Specifies the length in bytes of the output buffer. If OutputBuffer is NULL, this value must be zero.</param>
   ///<returns> actual number of bytes transferred in the operation.</returns>
   ///<exception cref='std::exception'> if the operation could not be completed.</exception>
   EXTENDEDUNIVERSALCPPSUPPORT_API const std::uint32_t ioctl(std::uint32_t dwIoControlCode, void* lpInBuffer, std::uint32_t nInBufferSize, void* lpOutBuffer, std::uint32_t nOutBufferSize) const;

   ///<summary> seek in the read/write space of the device (set the file pointer).</summary>
   ///<param name='cbyOffsetFromStart'> byte offset from start of device.</param>
   ///<exception cref='std::exception'> if the operation could not be completed.</exception>
   EXTENDEDUNIVERSALCPPSUPPORT_API const void seek(std::uint64_t cbyOffsetFromStart) const;

   ///<summary> issue a synchronous read. The thread is suspended pending completion of the read.</summary>
   ///<param name='lpBuffer'> pointer to buffer which will receive read data.</param>
   ///<param name='nBytesToRead'> number of bytes to read. this must be less than or equal to the available memory at lpBuffer.</param>
   ///<returns> actual number of bytes transferred/read.</returns>
   ///<exception cref='std::exception'> if the operation could not be completed.</exception>
   EXTENDEDUNIVERSALCPPSUPPORT_API const std::uint32_t read(void* lpBuffer, std::uint32_t nBytesToRead) const;

   ///<summary> issue a synchronous write. The thread is suspended pending completion of the write.</summary>
   ///<param name='lpBuffer'> pointer to buffer containing data to write.</param>
   ///<param name='nBytesToWrite'> number of bytes to write from the buffer.</param>
   ///<returns> actual number of bytes transferred/written.</returns>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   EXTENDEDUNIVERSALCPPSUPPORT_API const std::uint32_t write(void* lpBuffer, std::uint32_t nBytesToWrite) const;

   ///<summary> reset the device.</summary>
   ///<remarks> this is implemented as a close then open sequence and relies on the system device
   /// performing a "reset on open" semantics. This condition is not guaranteed for all devices, although a
   /// close and open will be an effective way to discard context anyway for most purposes.
   /// Devices that do not support a full reset on open may provide an ioctl to perform reset (see ioctl)</remarks>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   EXTENDEDUNIVERSALCPPSUPPORT_API void reset();

private:
   ///<summary> forward reference to private implementation.</summary>
   class impl;

   ///<summary> smart unique pointer to private implementation.</summary>
   ///<remarks> Non copyable, with move limitations.</remarks>
   spimpl::unique_impl_ptr<impl> pimpl;
};

#endif // __DEVICE_HPP__
