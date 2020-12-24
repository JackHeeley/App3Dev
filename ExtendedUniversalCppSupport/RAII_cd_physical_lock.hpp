//
// RAII_cd_physical_lock.hpp : implements an optical drive physical lock helper class.
//
// Note: Many (but not all) optical media devices support physical locking and unlocking
// to prevent media removal at critical times.
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

#include <cd_rom_device.hpp>
#include <logger.hpp>

///<summary> RAII object used to lock the physical media tray door on CDROM.</summary>
///<remarks> Be aware that signalled process termination (Eg CRTL+C or CTRL+BREAK) can bypass RAII destructors!</remarks>
class RAII_cd_physical_lock
{
private:
   ///<summary> the referenced cdrom.</summary>
   CdromDevice& m_cdr;

public:
   ///<summary> lock tray door at construction time.</summary>
   ///<param name='cdr'> a reference to the cdrom to lock.</param>
   RAII_cd_physical_lock::RAII_cd_physical_lock(CdromDevice& cdr) noexcept
      : m_cdr(cdr)
   {
      m_cdr.lock();
   }

   RAII_cd_physical_lock(RAII_cd_physical_lock& other) = delete;
   RAII_cd_physical_lock(RAII_cd_physical_lock&& other) = delete;
   RAII_cd_physical_lock& operator=(RAII_cd_physical_lock& other) = delete;
   RAII_cd_physical_lock& operator=(RAII_cd_physical_lock&& other) = delete;

   ///<summary>unlock tray door in all return paths</summary>
   /// <remarks> 
   ///   Be aware that signalled process termination (Eg CRTL+C or CTRL+BREAK) can bypass RAII destructors!
   ///   In this case a signal handler is ALSO required (to perform a speculative unlock).
   ///   The signal handler ensures that the program can never leave a (permanently) locked cdrom drive door behind.
   ///   (see SampleProgram for an example of such a signal handler).
   /// </remarks>
   ~RAII_cd_physical_lock() noexcept
   {
      LOG_INFO("~RAII_cd_physical_lock invoked");
      m_cdr.unlock();                                    // unlock immediately after use
   }
};
