//
// cd_rom_device.cpp : implements the cdrom device object
//
// Here we provide a capability to create concrete objects for cdrom device(s).
// The default deviceNumber (=0), selects the first device found.
//
// Copyright (c) 2005-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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
#include "stdafx.h"
#include <winioctl.h>
#include <ntddcdrm.h>

#include "device.hpp"
#include "cd_rom_device.hpp"
#include "logger.hpp"

#include <limits>

///<summary>simulate_resource_limitation</summary>
///<remarks>can be used to force multiple smaller reads (which allows for progress tracking)</remarks>
static void simulate_resource_limitation(void)
{
   LOG_WARNING("Simulating a resource limitation"); // TODO: refactor. This works, but its a dirty trick
   SetLastError(ERROR_NO_SYSTEM_RESOURCES);
   throw std::exception("simulated resource limitation");
}

/*
* ***************************************************************************
* PIMPL idiom - private implementation of CdromDevice class
* ***************************************************************************
*/

///<summary> the private implementation of Device.</summary>
///<remarks> Windows types used internally, adheres to "utf8 everywhere" paradigm at public interface</remarks>
class CdromDevice::impl : Device
{

private:
   ///<summary> physical locked state of drive door</summary>
   bool locked;

public:
   ///<summary> construct a cdrom device.</summary> 
   ///<param name='device_path'>the path selecting the physical device instance</param>
   impl(std::string device_path) :
      Device(device_path)
   {
      locked = false;
   };

   // no copy constructor (unique ptr)
   impl(const impl &other) = delete;
   
   impl(impl&& other) = delete;

   // (unique ptr= NonCopyable)
   impl& impl::operator=(impl& other) = delete;

   // base class has const members (non-moveable after construction)
   impl& impl::operator=(impl&& other) = delete; 

   ///<summary> destructor maintains accessible device state at end of use.</summary> 
   ~impl(void)
   {
      if (locked)
      {
         unlock();
      }
   }
 
   ///<summary> get size of media image.</summary>
   ///<returns> size in bytes of image data.</returns>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   const uint64_t impl::get_image_size() const
   {
      // query current media for data needed for buffer size calculation
      const DISK_GEOMETRY diskGeometry = get_disk_geometry();

      if (diskGeometry.Cylinders.HighPart != 0)
      {
         SetLastError(ERROR_NOT_SUPPORTED);
         throw error_context("Unsupported media size");   // future proofing
      }

      // calculate size of buffer needed to read entire media content
      const uint64_t bufferSize =
         uint64_t{ diskGeometry.Cylinders.LowPart } *
         uint64_t{ diskGeometry.TracksPerCylinder } *
         uint64_t{ diskGeometry.SectorsPerTrack } *
         uint64_t{ diskGeometry.BytesPerSector };

      return bufferSize;
   }
 
   ///<summary>get image of media into span, while maintaining a progress indication as we go.</summary>
   ///<remarks> This is a synchronous operation that can be very time consuming with some media (eg DVD).</remarks>
   ///<param name ='span'> a gsl::span repesenting a memory location to receive the image.</param>
   ///<param name ='a_progress'> reference to the external location where get_image() %progress is maintained</param>
   ///<exception cref='std::exception'>if the operation could not be completed with m_progress==100.</exception>
   void impl::get_image(gsl::span<unsigned char> span, std::atomic<int>& a_progress) const
   {
      // initialize the low and high water marks (used in read, and resource limited exception handling)
      LPBYTE lpabyBufferMemoryBase = span.data();
      LPBYTE lpabyBufferMemoryAddress = lpabyBufferMemoryBase;

      try
      {
         a_progress = 0;

         // force multiple smaller reads (where a granular progress tracker can be maintained)
         simulate_resource_limitation();

         // reading full image in one go is most probable scenario (at least for CD's)
         read_blocks(lpabyBufferMemoryBase, lpabyBufferMemoryAddress, 1, span.size_bytes(), a_progress);
      }
      catch (std::exception&)
      {
         // manage resource limitations by attempting multiple smaller reads
         if (GetLastError() == ERROR_NO_SYSTEM_RESOURCES)
         {
            // query current media for details used in exception handling strategy (physical block alignment constraint)
            const DISK_GEOMETRY diskGeometry = get_disk_geometry();

            if (diskGeometry.Cylinders.HighPart != 0)
            {
               SetLastError(ERROR_NOT_SUPPORTED);
               throw error_context("Unsupported media size"); // future proofing
            }

            const uint64_t cbyCylinderSize =
               uint64_t{ diskGeometry.TracksPerCylinder } *
               uint64_t{ diskGeometry.SectorsPerTrack } *
               uint64_t{ diskGeometry.BytesPerSector };

            try
            {
               // read remainder of the image in cylinder sized chunks 
               // (not necessarily physical cylinders but guaranteed to be an exact multiple of physical block size)
               read_blocks(lpabyBufferMemoryBase, lpabyBufferMemoryAddress, diskGeometry.Cylinders.LowPart, cbyCylinderSize, a_progress);
            }
            catch (std::exception&)
            {
               if (GetLastError() == ERROR_NO_SYSTEM_RESOURCES)
               {
                  const uint64_t cTracksReadAsCylinders = (((uint64_t)lpabyBufferMemoryAddress - (uint64_t)lpabyBufferMemoryBase) / cbyCylinderSize)* diskGeometry.TracksPerCylinder;
                  const uint64_t cbyTrackSize = (uint64_t)(diskGeometry.SectorsPerTrack) * (uint64_t)(diskGeometry.BytesPerSector);
                  const uint64_t cTracksStillToRead = ((uint64_t)(diskGeometry.Cylinders.LowPart) * (uint64_t)(diskGeometry.TracksPerCylinder)) - cTracksReadAsCylinders;
                  try
                  {
                     // read remainder of the image in track sized sized chunks 
                     // (not necessarily physical tracks but guaranteed to be an exact multiple of physical block size)
                     read_blocks(lpabyBufferMemoryBase, lpabyBufferMemoryAddress, cTracksStillToRead, cbyTrackSize, a_progress);
                  }
                  catch (std::exception&)
                  {
                     const uint64_t cSectorsReadAsTracks = (((uint64_t)lpabyBufferMemoryAddress - (uint64_t)lpabyBufferMemoryBase) / cbyTrackSize)* diskGeometry.SectorsPerTrack;
                     const uint64_t cSectorsStillToRead = ((uint64_t)(diskGeometry.Cylinders.LowPart) * (uint64_t)(diskGeometry.TracksPerCylinder) * (uint64_t)(diskGeometry.SectorsPerTrack)) - cSectorsReadAsTracks;
                     if (GetLastError() == ERROR_NO_SYSTEM_RESOURCES)
                     {
                        // read remainder of the image in sector sized sized chunks 
                        // (not necessarily physical sectors but guaranteed to be an exact multiple of physical block size)
                        read_blocks(lpabyBufferMemoryBase, lpabyBufferMemoryAddress, cSectorsStillToRead, diskGeometry.BytesPerSector, a_progress);
                     }
                     else throw;
                  }
               }
               else throw;
            }
         }
         else throw;
      }

      Ensures(a_progress == 100);   // if not, program will deadlock
   }

   ///<summary> prevents media removal. If the cdrom is already in the locked state, then this method does nothing.</summary>
   void impl::lock(void) noexcept
   {
      try
      {
         locked = lock_control(true);
      }
      catch (const error::context & e)
      {
         LOG_WARNING(e.full_what());
      }
   }

   ///<summary> allows media removal. If the cdrom is already in the unlocked state, then this method does nothing.</summary>
   void impl::unlock(void) noexcept
   {
      try
      {
         locked = lock_control(false);
      }
      catch (const error::context& e)
      {
         LOG_WARNING(e.full_what());
      }
   }

   ///<summary>query the locked state of the cdrom.</summary>
   ///<returns>true if the cdrom is locked.</returns>
   bool impl::get_locked(void) const noexcept
   {
      return locked;
   }

   ///<summary> load the media (closes the door of the CD drive)</summary>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   void impl::load(void) const
   {
      const DWORD nBytesReturned =
         ioctl(IOCTL_STORAGE_LOAD_MEDIA, nullptr, 0, nullptr, 0);

      if (nBytesReturned != 0)
      {
         throw error_context("ioctl returned non-zero length");
      }
   }

   ///<summary> eject the media (opens the door of the CD drive)</summary>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   void impl::eject(void) const
   {
      const DWORD nBytesReturned =
         ioctl(IOCTL_STORAGE_EJECT_MEDIA, nullptr, 0, nullptr, 0);

      if (nBytesReturned != 0)
      {
         throw error_context("ioctl returned non-zero length");
      }
   }

private:
   ///<summary>apply the IOCTL to allow/disallow media removal</summary>
   ///<remarks>this physically locks the cdrom tray door</remarks>
   ///<exception cref='std::exception'>if the operation could not be completed</exception>
   bool impl::lock_control(bool aLockedValue) const
   {
      PREVENT_MEDIA_REMOVAL lpInBuffer;

      lpInBuffer.PreventMediaRemoval = aLockedValue;

      const DWORD nBytesReturned =
         ioctl(IOCTL_STORAGE_MEDIA_REMOVAL, &lpInBuffer, sizeof(PREVENT_MEDIA_REMOVAL), nullptr, 0);

      if (nBytesReturned != 0)
      {
         throw error_context("ioctl returned non-zero length");
      }
      return aLockedValue;
   }

   ///<summary>get shape and size of medium currently in cdrom drive</summary>
   ///<exception cref='std::exception'>if the operation could not be completed</exception>
   const DISK_GEOMETRY impl::get_disk_geometry(void) const
   {
      DISK_GEOMETRY disk_geometry;

      const DWORD nBytesReturned =
         ioctl(IOCTL_CDROM_GET_DRIVE_GEOMETRY, nullptr, 0, &disk_geometry, sizeof(DISK_GEOMETRY));    // (re-)fetch from device

      if (nBytesReturned != sizeof(DISK_GEOMETRY))
      {
         throw error_context("ioctl returned unexpected length");
      }

      return disk_geometry;
   }

   ///<summary> read the entire device as specified number of blocks of a specified size.</summary>
   ///<remarks> for block devices reads must be integral multiples of the physical block size, and block aligned.</remarks>
   ///<param name='lpabyBufferMemoryBase'>the address where the start of the data is stored</param>
   ///<param name='lpabyBufferMemoryAddress'>the address where the data will be stored after reading</param>
   ///<param name='cBlocks'>the number of blocks to read</param>
   ///<param name='cbyBlockSize'>the size in bytes of the blocks to read</param>
   ///<param name ='a_progress'> reference to the external location where get_image() %progress is maintained</param>
   ///<exception cref='std::exception'>if the operation could not be completed</exception>
   void impl::read_blocks(LPBYTE& lpabyBufferMemoryBase, LPBYTE& lpabyBufferMemoryAddress, uint64_t cBlocks, uint64_t cbyBlockSize, std::atomic<int>& a_progress) const
   {
      for (uint64_t nBlock = 0; nBlock < cBlocks; nBlock++)
      {
         seek(lpabyBufferMemoryAddress - lpabyBufferMemoryBase);
         
#pragma warning (disable:26486 26487)
         read(lpabyBufferMemoryAddress, gsl::narrow_cast<uint32_t>(cbyBlockSize)); // see remarks above
         lpabyBufferMemoryAddress += cbyBlockSize;
#pragma warning (default:26486 26487)

         a_progress = gsl::narrow<int>((100 * nBlock) / cBlocks);
      }
      a_progress = 100; // handle possible rounding error
   }
};


/*
* ***************************************************************************
* PIMPL idiom - public interface for CdromDevice implementation
* ***************************************************************************
*/

///<summary> constructs a user mode Device that can be used to access a particular system cdrom instance.</summary>
///<param name='device_path'> the system name of the cdrom device to use.</param>
///<exception cref='std::exception'>if construction fails.</exception>
CdromDevice::CdromDevice(std::string device_path) :
   pimpl(spimpl::make_unique_impl<impl>(device_path))
{
}

///<summary> get size of media image.</summary>
///<returns> size in bytes of image data.</returns>
///<exception cref='std::exception'>if the operation could not be completed.</exception>
const uint64_t CdromDevice::get_image_size(void) const 
{
   return pimpl->get_image_size();
}

///<summary>get image of media into span.</summary>
///<remarks> This is a synchronous operation that can be very time consuming with some media (eg DVD).</remarks>
///<param name ='span'> a gsl::span representing a memory location to receive the image.</param>
///<param name='a_progress'> reference to percentage progress used in get_image.</param>
///<exception cref='std::exception'>if the operation could not be completed.</exception>
void CdromDevice::get_image(gsl::span<unsigned char>span, std::atomic<int>& a_progress) const
{
   pimpl->get_image(span, a_progress);
}

///<summary> prevents media removal. If the cdrom is already in the locked state, then this method does nothing.</summary>
void CdromDevice::lock(void) noexcept
{
   pimpl->lock();
}

///<summary> allows media removal. If the toaster is already in the unlocked state, then this method does nothing.</summary>
void CdromDevice::unlock(void) noexcept
{
   pimpl->unlock();
}

///<summary>query the locked state of the cdrom.</summary>
///<returns>true if the cdrom is locked.</returns>
const bool CdromDevice::get_locked(void) const noexcept
{
   return pimpl->get_locked();
}

///<summary> load the media (closes the door of the CD drive)</summary>
///<exception cref='std::exception'>if the operation could not be completed.</exception>
void CdromDevice::load(void) const
{
   pimpl->load();
}

///<summary> eject the media (opens the door of the CD drive)</summary>
///<exception cref='std::exception'>if the operation could not be completed.</exception>
void CdromDevice::eject(void) const
{
   pimpl->eject();
}
