//
// RAII_physical_lock.hpp : implements an optical drive physical lock helper class.
//
// Note: Many (but not all) optical media devices support physical locking and unlocking
// to prevent media removal at critical times.
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
#pragma once

#include <cd_rom_device.hpp>

///<summary> RAII object used to lock the physical media tray door on CDROM.</summary>
///<remarks> Be aware that signalled process termination (Eg CRTL+C or CTRL+BREAK) will bypass these destructors!</remarks>
class RAII_physical_lock
{
private:
   ///<summary> the referenced cdrom.</summary>
   CdromDevice & m_cdr;

public:
   ///<summary> lock tray door at construction time.</summary>
   ///<param name='cdr'> a reference to the cdrom to lock.</param>
   RAII_physical_lock::RAII_physical_lock(CdromDevice& cdr) noexcept 
      : m_cdr(cdr)
   {
      m_cdr.lock();
   }

   ///<summary> deleted copy constructor.</summary>
   ///<remarks> avoids premature unlocks via inactive object.</remarks>
   RAII_physical_lock(RAII_physical_lock& other) = delete;

   ///<summary> deleted move constructor.</summary>
   ///<remarks> avoids premature unlocks via inactive object.</remarks>
   RAII_physical_lock(RAII_physical_lock&& other) = delete;

   ///<summary> deleted copy assignment.</summary>
   ///<remarks> avoids premature unlocks via inactive object.</remarks>
   RAII_physical_lock& operator=(RAII_physical_lock& other) = delete;

   ///<summary> deleted move assignment.</summary>
   ///<remarks> avoids premature unlocks via inactive object.</remarks>
   RAII_physical_lock& operator=(RAII_physical_lock&& other) = delete;

   ///<summary>unlock tray door in all return paths</summary>
   ~RAII_physical_lock() noexcept
   {
      LOG_INFO("~RAII_physical_lock invoked");
      m_cdr.unlock();
   }
};