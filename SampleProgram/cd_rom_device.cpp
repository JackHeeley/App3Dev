//
// CdromDevice.cpp : implements the cdrom device object
//
// Here we provide a capability to create concrete objects for cdrom device(s).
// The default deviceNumber (=0), selects the first device found.
//
// Copyright (c) 2005-2017 Jack Heeley, all rights reserved
//
#include "stdafx.h"
#include "cd_rom_device.hpp"

#include <limits>
#include <ntddcdrm.h>

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

   ///<summary> destructor maintains accessible device state at end of use.</summary> 
   ~impl(void)
   {
      if (locked)
      {
         try
         {
            unlock();
         }
         catch (std::exception&)
         {
            // mustn't throw. TODO: could log here
         }
      }
   }
 
   ///<summary> get size of media image.</summary>
   ///<returns> size in bytes of image data.</returns>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   uint64_t impl::get_image_size()
   {
      // query current media for data needed for buffer size calculation
      DISK_GEOMETRY diskGeometry = get_disk_geometry();

      if (diskGeometry.Cylinders.HighPart != 0)
      {
         SetLastError(ERROR_NOT_SUPPORTED);
         throw fox_exception("Unsupported media size");   // future proofing
      }

      // calculate size of buffer needed to read entire media content
      uint64_t bufferSize =
         static_cast<uint64_t>(diskGeometry.Cylinders.LowPart) *
         static_cast<uint64_t>(diskGeometry.TracksPerCylinder) *
         static_cast<uint64_t>(diskGeometry.SectorsPerTrack) *
         static_cast<uint64_t>(diskGeometry.BytesPerSector);

      return bufferSize;
   }

   ///<summary>get image of media into normal byte vector.</summary>
   ///<remarks> This is a synchronous operation that can be very time consuming with some media (eg DVD).</remarks>
   ///<param name ='buffer'> a vector to receive the image. The vector will be adjusted in size to accommodate the data.</param>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   void impl::get_image(std::vector<unsigned char>& buffer)
   {
      uint64_t buffer_size = get_image_size();

      // check vector size limitation (on win32)
      if (buffer_size > reinterpret_cast<uint64_t>(std::numeric_limits<std::size_t>::max))
      {
         SetLastError(ERROR_NOT_SUPPORTED);
         throw fox_exception("Vector cannot accommodate this media size with x86 architecture");
      }

      // adjust buffer vector to accommodate the image data
      buffer.resize(static_cast<size_t>(buffer_size));

      get_image(buffer.data(), buffer.size());
   }

   ///<summary>get image of media into mmf byte vector.</summary>
   ///<remarks> This is a synchronous operation that can be very time consuming with some media (eg DVD).</remarks>
   ///<param name ='buffer'> a mmf_vector to receive the image. The vector will be adjusted in size to accommodate the data.</param>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   void impl::get_image(mmf_vector& buffer)
   {
      uint64_t buffer_size = get_image_size();

      // check vector size limitation (on win32)
      if (buffer_size > reinterpret_cast<uint64_t>(std::numeric_limits<std::size_t>::max))
      {
         SetLastError(ERROR_NOT_SUPPORTED);
         throw fox_exception("Vector cannot accommodate this media size with x86 architecture");
      }

      // adjust buffer vector to accommodate the image data
      buffer.resize(static_cast<size_t>(buffer_size));

      get_image(buffer.data(), buffer.size());
   }

   ///<summary>get image of media to memory buffer.</summary>
   ///<remarks> This is a synchronous operation that can be very time consuming with some media (eg DVD).</remarks>
   ///<param name ='ptr'> a raw pointer to a memory location to receive the image.</param>
   ///<param name ='buffer_size'> length in bytes of the memory location to receive the image.
   /// The supplied contiguous virtual memory block should be large enough to accomodate the data to be returned.</param>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   void impl::get_image(void* ptr, uint64_t buffer_size)
   {

      // initialize the low and high water marks (used in read, and resource limited exception handling)
      LPBYTE lpabyBufferMemoryBase = (LPBYTE)ptr;
      LPBYTE lpabyBufferMemoryAddress = lpabyBufferMemoryBase;

      try
      {
         // reading full image in one go is most probable scenario (at least for CD's)
         read_blocks(lpabyBufferMemoryAddress, 1, buffer_size); 
      }
      catch (std::exception&)
      {
         // manage resource limitations by attempting multiple smaller reads
         if (GetLastError() == ERROR_NO_SYSTEM_RESOURCES)
         {
            // query current media for details used in exception handling strategy (physical block alignment constraint)
            DISK_GEOMETRY diskGeometry = get_disk_geometry();

            if (diskGeometry.Cylinders.HighPart != 0)
            {
               SetLastError(ERROR_NOT_SUPPORTED);
               throw fox_exception("Unsupported media size"); // future proofing
            }
             
            uint64_t cbyCylinderSize =
              static_cast<uint64_t>(diskGeometry.TracksPerCylinder) *
              static_cast<uint64_t>(diskGeometry.SectorsPerTrack) *
              static_cast<uint64_t>(diskGeometry.BytesPerSector);
            
            try
            {
               // read remainder of the image in cylinder sized chunks 
               // (not necessarily physical cylinders but guaranteed to be an exact multiple of physical block size)
               read_blocks(lpabyBufferMemoryAddress, diskGeometry.Cylinders.LowPart, cbyCylinderSize);
            }
            catch (std::exception&)
            {
               if (GetLastError() == ERROR_NO_SYSTEM_RESOURCES)
               {
                  uint64_t cTracksReadAsCylinders = (((uint64_t)lpabyBufferMemoryAddress - (uint64_t)lpabyBufferMemoryBase) / cbyCylinderSize)* diskGeometry.TracksPerCylinder;
                  uint64_t cbyTrackSize = diskGeometry.SectorsPerTrack * diskGeometry.BytesPerSector;
                  uint64_t cTracksStillToRead = (diskGeometry.Cylinders.LowPart * diskGeometry.TracksPerCylinder) - cTracksReadAsCylinders;
                  try
                  {
                     // read remainder of the image in track sized sized chunks 
                     // (not necessarily physical tracks but guaranteed to be an exact multiple of physical block size)
                     read_blocks(lpabyBufferMemoryAddress, cTracksStillToRead, cbyTrackSize);
                  }
                  catch (std::exception&)
                  {
                     uint64_t cSectorsReadAsTracks = (((uint64_t)lpabyBufferMemoryAddress - (uint64_t)lpabyBufferMemoryBase) / cbyTrackSize)* diskGeometry.SectorsPerTrack;
                     uint64_t cSectorsStillToRead = (diskGeometry.Cylinders.LowPart * diskGeometry.TracksPerCylinder * diskGeometry.SectorsPerTrack) - cSectorsReadAsTracks;
                     if (GetLastError() == ERROR_NO_SYSTEM_RESOURCES)
                     {
                        // read remainder of the image in sector sized sized chunks 
                        // (not necessarily physical sectors but guaranteed to be an exact multiple of physical block size)
                        read_blocks(lpabyBufferMemoryAddress, cSectorsStillToRead, diskGeometry.BytesPerSector);
                     }
                     else throw;
                  }
               }
               else throw;
            }
         }
         else throw;
      }
   }

   ///<summary> prevents media removal. If the cdrom is already in the locked state, then this method does nothing.</summary>
   ///<exception cref='std::exception'>if the operation could not be completed</exception>
   void impl::lock(void)
   {
      locked = lock_control(true);
   }

   ///<summary> allows media removal. If the toaster is already in the unlocked state, then this method does nothing.</summary>
   ///<exception cref='std::exception'>if the operation could not be completed</exception>
   void impl::unlock(void)
   {
      locked = lock_control(false);
   }

   ///<summary>query the locked state of the cdrom.</summary>
   ///<returns>true if the cdrom is locked.</returns>
   bool impl::get_locked(void)
   {
      return locked;
   }

   ///<summary> load the media (closes the door of the CD drive)</summary>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   void impl::load(void)
   {
      DWORD nBytesReturned =
         ioctl(IOCTL_STORAGE_LOAD_MEDIA, NULL, 0, NULL, 0);

      if (nBytesReturned != 0)
      {
         throw fox_exception("ioctl returned non-zero length");
      }
   }

   ///<summary> eject the media (opens the door of the CD drive)</summary>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   void impl::eject(void)
   {
      DWORD nBytesReturned =
         ioctl(IOCTL_STORAGE_EJECT_MEDIA, NULL, 0, NULL, 0);

      if (nBytesReturned != 0)
      {
         throw fox_exception("ioctl returned non-zero length");
      }
   }

private:
   ///<summary>apply the IOCTL to allow/disallow media removal</summary>
   ///<remarks>this physically locks the cdrom tray door</remarks>
   ///<exception cref='std::exception'>if the operation could not be completed</exception>
   bool impl::lock_control(bool aLockedValue)
   {
      PREVENT_MEDIA_REMOVAL lpInBuffer;

      lpInBuffer.PreventMediaRemoval = aLockedValue;

      DWORD nBytesReturned =
         ioctl(IOCTL_STORAGE_MEDIA_REMOVAL, &lpInBuffer, sizeof(PREVENT_MEDIA_REMOVAL), NULL, 0);

      if (nBytesReturned != 0)
      {
         throw fox_exception("ioctl returned non-zero length");
      }
      return aLockedValue;
   }

   ///<summary>get shape and size of medium currently in cdrom drive</summary>
   ///<exception cref='std::exception'>if the operation could not be completed</exception>
   DISK_GEOMETRY impl::get_disk_geometry(void)
   {
      DISK_GEOMETRY disk_geometry;

      DWORD nBytesReturned =
         ioctl(IOCTL_CDROM_GET_DRIVE_GEOMETRY, NULL, 0, &disk_geometry, sizeof(DISK_GEOMETRY));    // (re-)fetch from device

      if (nBytesReturned != sizeof(DISK_GEOMETRY))
      {
         throw fox_exception("ioctl returned unexpected length");
      }

      return disk_geometry;
   }

   ///<summary> read the entire device as specified number of blocks of a specified size.</summary>
   ///<remarks> for block devices reads must be integral multiples of the physical block size, and block aligned.</remarks>
   ///<param name='lpabyBufferMemoryAddress'>the address where the data will be stored after reading</param>
   ///<param name='cBlocks'>the number of blocks to read</param>
   ///<param name='cbyBlockSize'>the size in bytes of the blocks to read</param>
   ///<exception cref='std::exception'>if the operation could not be completed</exception>
   void impl::read_blocks(LPBYTE &lpabyBufferMemoryAddress, uint64_t cBlocks, uint64_t cbyBlockSize)
   {
      for (uint64_t nBlock = 0; nBlock < cBlocks; nBlock++)
      {
         read(lpabyBufferMemoryAddress, static_cast<uint32_t>(cbyBlockSize)); // see remarks above
         lpabyBufferMemoryAddress += cbyBlockSize;
      }
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
   impl_(spimpl::make_unique_impl<impl>(device_path))
{
}

///<summary> get size of media image.</summary>
///<returns> size in bytes of image data.</returns>
///<exception cref='std::exception'>if the operation could not be completed.</exception>
uint64_t CdromDevice::get_image_size(void)
{
   return impl_->get_image_size();
}

///<summary>get image of media into normal byte vector.</summary>
///<remarks> This is a synchronous operation that can be very time consuming with some media (eg DVD).</remarks>
///<param name ='buffer'> a vector to receive the image.
/// The vector will be adjusted in size to accommodate the data.</param>
///<exception cref='std::exception'>if the operation could not be completed.</exception>
void CdromDevice::get_image(std::vector<unsigned char>& buffer)
{
   impl_->get_image(buffer);
}

///<summary>get image of media into mmf byte vector.</summary>
///<remarks> This is a synchronous operation that can be very time consuming with some media (eg DVD).</remarks>
///<param name ='buffer'> a mmf_vector to receive the image. The vector will be adjusted in size to accommodate the data.</param>
///<exception cref='std::exception'>if the operation could not be completed.</exception>
void CdromDevice::get_image(mmf_vector& buffer)
{
   impl_->get_image(buffer);
}

///<summary>get image of media to memory buffer.</summary>
///<remarks> This is a synchronous operation that can be very time consuming with some media (eg DVD).</remarks>
///<param name ='ptr'> a raw pointer to a memory location to receive the image.</param>
///<param name ='buffer_size'> length in bytes of the memory location to receive the image.
/// The supplied contiguous virtual memory block should be large enough to accomodate the data to be returned.</param>
///<exception cref='std::exception'>if the operation could not be completed.</exception>
void CdromDevice::get_image(void* ptr, uint64_t buffer_size)
{
   impl_->get_image(ptr, buffer_size);
}

///<summary> prevents media removal. If the cdrom is already in the locked state, then this method does nothing.</summary>
void CdromDevice::lock(void)
{
   impl_->lock();
}

///<summary> allows media removal. If the toaster is already in the unlocked state, then this method does nothing.</summary>
void CdromDevice::unlock(void)
{
   impl_->unlock();
}

///<summary>query the locked state of the cdrom.</summary>
///<returns>true if the cdrom is locked.</returns>
bool CdromDevice::get_locked(void)
{
   return impl_->get_locked();
}

///<summary> load the media (closes the door of the CD drive)</summary>
///<exception cref='std::exception'>if the operation could not be completed.</exception>
void CdromDevice::load(void)
{
   impl_->load();
}

///<summary> eject the media (opens the door of the CD drive)</summary>
///<exception cref='std::exception'>if the operation could not be completed.</exception>
void CdromDevice::eject(void)
{
   impl_->eject();
}
