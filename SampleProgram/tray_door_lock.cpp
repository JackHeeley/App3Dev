//
// tray_door_lock.cpp : helper class for the console application.
//
// RAII object to manage cd rom tray door locking 
//
// Copyright (c) 2018 Jack Heeley, all rights reserved
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
