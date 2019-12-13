//
// tray_door_lock.cpp : helper class for the console application.
//
// RAII object to manage cd rom tray door locking 
//
// Copyright (c) 2003-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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

#include "tray_door_lock.hpp"

///<summary> the file logger in use.</summary>
extern file_logger Logger;

///<summary> lock tray door at construction time.</summary>
///<param name='cdr'> a reference to the cdrom to lock.</param>
TrayDoorLock::TrayDoorLock(CdromDevice& cdr) : m_cdr(cdr)
{
   m_cdr.lock();
}

///<summary>unlock tray door in all return paths</summary>
TrayDoorLock::~TrayDoorLock() noexcept
{
   try
   {
      LOG_INFO("~TrayDoorLock invoked");
      m_cdr.unlock();
   }
   catch (...)
   {
      try
      {
         LOG_ERROR("Exception in ~TrayDoorLock.");
      }
      catch (...)
      {
         // dismiss is still least bad option here
      }
   }
}
