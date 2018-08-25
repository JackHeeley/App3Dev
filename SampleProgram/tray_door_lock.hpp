//
// tray_door_lock.hpp : implements the tray door lock helper class
//
// Copyright (c) 2005-2018 Jack Heeley, all rights reserved
//
#pragma once

#include <cd_rom_device.hpp>

///<summary> RAII object used to lock the physical media tray door on CDROM.</summary>
///<remarks> Be aware that signalled process termination (Eg CRTL+C or CTRL+BREAK) won't invoke destructors!</remarks>
class TrayDoorLock
{
private:
   ///<summary> the referenced cdrom.</summary>
   CdromDevice & m_cdr;

public:
   ///<summary> lock tray door at construction time.</summary>
   ///<param name='cdr'> a reference to the cdrom to lock.</param>
   TrayDoorLock(CdromDevice& cdr);

   ///<summary> deleted copy constructor.</summary>
   ///<remarks> avoids premature unlocks via inactive object.</remarks>
   TrayDoorLock(TrayDoorLock& other) = delete;

   ///<summary> deleted move constructor.</summary>
   ///<remarks> avoids premature unlocks via inactive object.</remarks>
   TrayDoorLock(TrayDoorLock&& other) = delete;

   ///<summary> deleted copy assignment.</summary>
   ///<remarks> avoids premature unlocks via inactive object.</remarks>
   TrayDoorLock& operator=(TrayDoorLock& other) = delete;

   ///<summary> deleted move assignment.</summary>
   ///<remarks> avoids premature unlocks via inactive object.</remarks>
   TrayDoorLock& operator=(TrayDoorLock&& other) = delete;

   ///<summary>unlock tray door in all return paths</summary>
#pragma warning (disable:26447)
   ~TrayDoorLock() noexcept;
#pragma warning (default:26447)

};