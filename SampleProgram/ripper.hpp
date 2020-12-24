//
// ripper.hpp : implements the ripper helper class
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

#include <atomic>

#include "logger.hpp"
#include "RAII_cd_physical_lock.hpp"
#include "RAII_cd_exclusive_access_lock.hpp"

///<summary> functor class to rip image.</summary>
class Ripper
{
private:
   ///<summary> the cdrom to be ripped.</summary>
   CdromDevice m_cdr;

public:
   ///<summary> construct a ripper.</summary>
   ///<param name='devicePath'> the utf8 name of a raw system cdrom device containing media.</param>
   Ripper(const std::string& devicePath) :
      m_cdr(devicePath)
   {
      LOG_INFO(std::string("Ripper Device ").append(devicePath));
   }

   ///<summary> functor to perform the rip operation. This copies the cdrom image to a disk file.</summary>
   ///<remarks> 
   /// During the rip operation we take steps to avoid mission failure happening as a side-effect of some other (ill advised) user action.
   ///   1) we make sure (assuming hardware supports tray door locking) that the media can't be prematurely removed (during copy).
   ///   2) we make sure that another program instance or another program can't simultaneously access the device (being used for copy). 
   /// Externally callers can permit the user to choose to abort the mission (via CTRL+C, CTRL+BREAK and close console window). 
   /// In that case the caller process should issue a pre-emptive tray door unlock during abort signal handling.
   /// </remarks>
   ///<param name='filePath'> the utf8 name of a file to receive the (iso 9660) image.</param>
   ///<param name='a_progress'> reference to where percentage read progress will be maintained (during the rip operation).</param>
   void operator()(const std::string& filePath, std::atomic<int>& a_progress)
   {
      RAII_cd_physical_lock lock(m_cdr);                                      // disables the cd eject button
      RAII_cd_exclusive_access_lock ea_lock(m_cdr, "Rip_" + utc_timestamp()); // disallow other instances/programs (potential simultaneous writes)

      LOG_INFO(std::string("Ripping to ").append(filePath));

      // get image into the buffer of a suitably named memory mapped file, (and keep track of progress)
      m_cdr.get_image(MemoryMappedFile(filePath, "", m_cdr.get_image_size()).get_span(), a_progress);
   }
};
