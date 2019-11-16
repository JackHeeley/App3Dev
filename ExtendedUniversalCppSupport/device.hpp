//
// device.hpp : implements device driver access methods.
//
// Here we implement methods wrapping system calls. This offloads the
// parameterization of these functions from clients, for ease of use.
//
// Copyright (c) 2003-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
#pragma once
#pragma warning (disable:4251 4290)

#ifdef EXTENDEDUNIVERSALCPPSUPPORT_EXPORTS
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <string>
#include <memory>
#include <exception>
#include <spimpl.hpp>

///<summary> represents a moveable abstract physical system device.</summary>  
///<remarks> we explicitly disallow copy, and compare of devices as these operations have no great value.</remarks>
class EXTENDEDUNIVERSALCPPSUPPORT_API Device {

public:

   ///<summary> constructs a movable user mode Device that can be used to access a particular system device instance.</summary>
   ///<param name='device_path'> the system name of the device to use.</param>
   ///<exception cref='std::exception'>if construction fails.</exception>
   Device(std::string device_path) throw (std::exception);

   /// <summary> issue a synchronous device i/o control message. The thread is suspended until this request completes.</summary>
   ///<param name ='dwIoControlCode'> Specifies the IOCTL_XXX to be set up. For more information about system specific device-type-specific I/O codes, 
   /// see the appropriate system reference. Eg for Windows this is the Windows DDK Kernel Mode Driver Design Guide Reference Part II.</param>
   ///<param name='lpInBuffer'> Points to an input buffer to be passed to the driver or nullptr if the request does not pass input data.</param>
   ///<param name='nInBufferSize'> Specifies the length in bytes of the input buffer. If InputBuffer is nullptr, this value must be zero.</param>
   ///<param name='lpOutBuffer'> Points to an output buffer in which the driver is to return data or NULL if the request does not require driver to return data.</param>
   ///<param name='nOutBufferSize'> Specifies the length in bytes of the output buffer. If OutputBuffer is NULL, this value must be zero.</param>
   ///<returns> actual number of bytes transferred in the operation.</returns>
   ///<exception cref='std::exception'> if the operation could not be completed.</exception>
   const std::uint32_t ioctl(std::uint32_t dwIoControlCode, void* lpInBuffer, std::uint32_t nInBufferSize, void* lpOutBuffer, std::uint32_t nOutBufferSize) const throw (std::exception);

   ///<summary> issue a synchronous read. The thread is suspended pending completion of the read.</summary>
   ///<param name='lpBuffer'> pointer to buffer which will receive read data.</param>
   ///<param name='nBytesToRead'> number of bytes to read. this must be less than or equal to the available memory at lpBuffer.</param>
   ///<returns> actual number of bytes transferred/read.</returns>
   ///<exception cref='std::exception'> if the operation could not be completed.</exception>
   const std::uint32_t read(void* lpBuffer, std::uint32_t nBytesToRead) const throw (std::exception);

   ///<summary> issue a synchronous write. The thread is suspended pending completion of the write.</summary>
   ///<param name='lpBuffer'> pointer to buffer containing data to write.</param>
   ///<param name='nBytesToWrite'> number of bytes to write from the buffer.</param>
   ///<returns> actual number of bytes transferred/written.</returns>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   const std::uint32_t write(void* lpBuffer, std::uint32_t nBytesToWrite) const throw (std::exception);

   ///<summary> reset the device.</summary>
   ///<remarks> this is equivalent to close/open sequence and is predicated on the system 
   /// device implementing a "reset on open" semantics. This condition is not guaranteed.
   /// Devices that do not support this semantics may provide an ioctl to perform reset (see ioctl)</remarks>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   void reset() throw (std::exception);

protected:

private:
   ///<summary> forward reference to private implementation.</summary>
   class impl;

   ///<summary> smart unique pointer to private implementation.</summary>
   ///<remarks> Non copyable, with move limitations.</remarks>
   spimpl::unique_impl_ptr<impl> pimpl;
};

#pragma warning (default:4251 4290)