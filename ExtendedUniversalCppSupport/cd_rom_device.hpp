//
// cd_rom_device.hpp : implements the cdrom device object
//
// Here we provide a capability to create concrete objects for cd rom device(s).
//
// Copyright (c) 2005-2020 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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
#pragma once

#ifdef EXTENDEDUNIVERSALCPPSUPPORT_EXPORTS
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <atomic>
#include <string>

#include <gsl.hpp>
#include <spimpl.hpp>

#include "memory_mapped_file.hpp"

///<summary> CdromDevice offers a limited feature set specific to CD ROM Devices</summary>
///<remarks> raw device access (without CDFS) allows us to build a one time rip feature 
/// based on low level serial reads. We don't attempt write support, and this would pose
/// some big challenges.</remarks> 
class CdromDevice
{
public:
   ///<summary> constructs a user mode Device that can be used to access a particular system cdrom instance.</summary>
   ///<param name='device_path'> the system name of the cdrom device to use.</param>
   ///<exception cref='std::exception'>if construction fails.</exception>
   EXTENDEDUNIVERSALCPPSUPPORT_API CdromDevice(const std::string& device_path);

   ///<summary> get size of media image.</summary>
   ///<returns> size in bytes of image data.</returns>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   EXTENDEDUNIVERSALCPPSUPPORT_API const uint64_t get_image_size(void) const;

   ///<summary> check for presence or absence of media in this CD drive.</summary>
   ///<returns> true if a compatible compact disk is recognized as being present in the drive, otherwise false.</returns> 
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   EXTENDEDUNIVERSALCPPSUPPORT_API const bool check_for_media_present(void) const;

   ///<summary>get image of media into span.</summary>
   ///<remarks> This is a synchronous operation that can be very time consuming with some media (eg DVD).
   /// The progress indicator is maintained (as a percentage) during this operation.</remarks>
   ///<param name ='span'> a gsl::span representing a memory location to receive the image.</param>
   ///<param name='a_progress'> reference to percentage progress used in get_image.</param>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   EXTENDEDUNIVERSALCPPSUPPORT_API void get_image(gsl::span<unsigned char> span, std::atomic<int>& a_progress) const;

   ///<summary> claims exclusive access to device.</summary>
   ///<remarks> by sending IOCTL. If successful, the filesystem that overlays the physical device will be inaccessible 
   /// until a call to release_exclusive_access() is made</remarks>
   ///<param name='aCallerName'> A NULL-terminated string that identifies the application or system component that has a lock on the CD-ROM device. 
   /// The length of the string must be less than or equal to CDROM_EXCLUSIVE_CALLER_LENGTH bytes, including the NULL character at the end of the string.
   /// The string must contain alphanumerics (A - Z, a - z, 0 - 9), spaces, periods, commas, colons (:), semi-colons (;), hyphens (-), and underscores (_).</param> 
   ///<exception cref='std::exception'>if the operation could not be completed</exception>
   EXTENDEDUNIVERSALCPPSUPPORT_API void claim_exclusive_access(const std::string& aCallerName) noexcept;

   ///<summary> releases previously claimed exclusive access to device.</summary>
   ///<remarks> Use after an earlier successful call to request_exclusive_acccess().</remarks>
   ///<param name='aCallerName'> A NULL-terminated string that identifies the application or system component that has a lock on the CD-ROM device. 
   /// The length of the string must be less than or equal to CDROM_EXCLUSIVE_CALLER_LENGTH bytes, including the NULL character at the end of the string.
   /// The string must contain alphanumerics (A - Z, a - z, 0 - 9), spaces, periods, commas, colons (:), semi-colons (;), hyphens (-), and underscores (_).</param> 
   ///<exception cref='std::exception'>if the operation could not be completed</exception>
   EXTENDEDUNIVERSALCPPSUPPORT_API void release_exclusive_access(const std::string& aCallerName) noexcept;

   ///<summary> prevents media removal.</summary>
   ///<remarks> by locking the door of the optical drive. (This feature is not supported by all optical drives).
   /// lock() also claims exclusive access to the device, which makes ioctl and reads from other processes fail 
   /// until the next subsequent call to unlock().</remarks>
   EXTENDEDUNIVERSALCPPSUPPORT_API void lock(void) noexcept;

   ///<summary> allows media removal again (after a lock).</summary>
   EXTENDEDUNIVERSALCPPSUPPORT_API void unlock(void) noexcept;

   ///<summary> load the media (closes the door of the CD drive) This feature is not supported by all optical drives.</summary>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   EXTENDEDUNIVERSALCPPSUPPORT_API void load(void) const;

   ///<summary> eject the media (opens the door of the CD drive)</summary>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   EXTENDEDUNIVERSALCPPSUPPORT_API void eject(void) const;

   ///<summary> static version of check for media.</summary>
   ///<param name='device_path'> the system name of the cdrom device to use.</param>
   ///<returns> true if a compatible compact disk is recognized as being present in the drive, otherwise false.</returns> 
   static EXTENDEDUNIVERSALCPPSUPPORT_API const bool check_for_media(const std::string& device_path)
   {
      return CdromDevice(device_path).check_for_media_present();
   }

private:
   ///<summary> forward reference to private implementation.</summary>
   class impl;

   ///<summary> unique pointer to private implementation.</summary>
   ///<remarks> Non copyable, with move limitations.</remarks>
   spimpl::unique_impl_ptr<impl> pimpl;
};
