//
// cd_rom_device.hpp : implements the cdrom device object
//
// Here we provide a capability to create concrete objects for cd rom device(s).
//
// Copyright (c) 2005-2017 Jack Heeley, all rights reserved
//
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <spimpl.hpp>

#include <memory_mapped_file.hpp>     // mmf_vector type (and dependent custom allocator support functions)

#pragma warning (disable:4251 4290)

///<summary> CdromDevice offers a limited feature set specific to CD ROM Devices</summary>
///<remarks> raw device access (without CDFS) allows us to build a one time rip feature 
/// based on low level serial reads. We don't consider seek and read position which 
/// likely means limitations and exceptions if extending the current scope. We don't 
/// attempt write support, and this would pose some big challenges.</remarks> 
class CdromDevice 
{
public:
   ///<summary> constructs a user mode Device that can be used to access a particular system cdrom instance.</summary>
   ///<param name='device_path'> the system name of the cdrom device to use.</param>
   ///<exception cref='std::exception'>if construction fails.</exception>
   CdromDevice(std::string device_path);

   ///<summary> get size of media image.</summary>
   ///<returns> size in bytes of image data.</returns>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   uint64_t get_image_size(void);

   ///<summary>get image of media into normal byte vector.</summary>
   ///<remarks> This is a synchronous operation that can be very time consuming with some media (eg DVD).
   /// For this reason caller may wish to consider using lock/unlock.</remarks>
   ///<param name ='buffer'> a vector to receive the image.
   /// The vector will be adjusted in size to accommodate the data.</param>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   void get_image(std::vector<unsigned char>& buffer);

   ///<summary>get image of media into mmf byte vector.</summary>
   ///<remarks> This is a synchronous operation that can be very time consuming with some media (eg DVD).
   /// For this reason caller may wish to consider using lock/unlock.</remarks>
   ///<param name ='buffer'> a mmf_vector to receive the image. 
   /// The vector will be adjusted in size to accommodate the data.</param>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   void get_image(mmf_vector& buffer);

   ///<summary>get image of media to memory buffer.</summary>
   ///<remarks> This is a synchronous operation that can be very time consuming with some media (eg DVD).</remarks>
   ///<param name ='ptr'> a raw pointer to a memory location to receive the image.</param>
   ///<param name ='buffer_size'> length in bytes of the memory location to receive the image.
   /// The supplied contiguous virtual memory block should be large enough to accomodate the data to be returned.</param>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   void CdromDevice::get_image(void* ptr, uint64_t buffer_size);

   ///<summary> prevents media removal.</summary>
   void lock(void);

   ///<summary> allows media removal after lock.</summary>
   void unlock(void);

   ///<summary>query the locked state of the cdrom.</summary>
   ///<returns>true if the cdrom is locked.</returns>
   bool get_locked(void);

   ///<summary> load the media (closes the door of the CD drive)</summary>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   void load(void);

   ///<summary> eject the media (opens the door of the CD drive)</summary>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   void eject(void);

protected:

private:
   ///<summary> forward reference to private implementation.</summary>
   class impl;

   ///<summary> unique pointer to private implementation.</summary>
   ///<remarks> supports default move semantics.</remarks>
   spimpl::unique_impl_ptr<impl> impl_;
};

#pragma warning (default:4251 4290)