//
// RAII_exclusive_access_lock.hpp : implements an optical drive physical lock helper class.
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

///<summary> RAII object used to claim exclusive access to a specific physical CDROM.</summary>
///<remarks> Be aware that signalled process termination (Eg CRTL+C or CTRL+BREAK) will bypass these destructors!</remarks>
class RAII_exclusive_access_lock
{
private:
   ///<summary> the referenced cdrom.</summary>
   CdromDevice& m_cdr;
   std::string m_callerName;

public:
   ///<summary> claim exclusive access at construction time.</summary>
   ///<param name='cdr'> a reference to the cdrom to claim.</param>
   RAII_exclusive_access_lock::RAII_exclusive_access_lock(CdromDevice& cdr, const std::string& callerName) noexcept
      : m_cdr(cdr)
   {
      try
      {
         m_callerName = callerName;
         m_cdr.claim_exclusive_access(m_callerName);
      }
      catch (...)
      {
         LOG_WARNING("Couldn't construct an RAII_excliusive_access_lock");
      }
   }

   ///<summary> deleted copy constructor.</summary>
   ///<remarks> avoids premature unlocks via inactive object.</remarks>
   RAII_exclusive_access_lock(RAII_exclusive_access_lock& other) = delete;

   ///<summary> deleted move constructor.</summary>
   ///<remarks> avoids premature unlocks via inactive object.</remarks>
   RAII_exclusive_access_lock(RAII_exclusive_access_lock&& other) = delete;

   ///<summary> deleted copy assignment.</summary>
   ///<remarks> avoids premature unlocks via inactive object.</remarks>
   RAII_exclusive_access_lock& operator=(RAII_exclusive_access_lock& other) = delete;

   ///<summary> deleted move assignment.</summary>
   ///<remarks> avoids premature unlocks via inactive object.</remarks>
   RAII_exclusive_access_lock& operator=(RAII_exclusive_access_lock&& other) = delete;

   ///<summary>release the exclusive access all return paths</summary>
   ~RAII_exclusive_access_lock() noexcept
   {
      LOG_INFO("~RAII_exclusive_access_lock invoked");
      m_cdr.release_exclusive_access(m_callerName);
   }
};

