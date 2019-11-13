//
// ripper.cpp : implements the ripper helper class 
//
// supplies a functor for the console application.
//
// Copyright (c) 2018-2019 Jack Heeley, all rights reserved
//
#include "stdafx.h"

#include "tray_door_lock.hpp"
#include "ripper.hpp"

///<summary> the file logger in use.</summary>
extern file_logger Logger;


///<summary> construct a ripper.</summary>
///<param name='devicePath'> the utf8 name of a raw system cdrom device containing media.</param>
///<param name='filePath'> the utf8 name of a file to receive the (iso 9660) image.</param>
///<param name='a_progress'> reference to percentage progess of the rip</param>
Ripper::Ripper(std::string devicePath, std::string filePath, std::atomic<int>& a_progress) :
      m_cdr(devicePath, a_progress),
      m_mmf(filePath, "", m_cdr.get_image_size())
{
   LOG_INFO(std::string("Ripper Device ").append(devicePath));
   LOG_INFO(std::string("Ripping to ").append(filePath));
}

///<summary> functor to perform the rip operation.</summary>
void Ripper::operator()()
{
   TrayDoorLock lock(m_cdr);
   m_cdr.get_image(m_mmf.get_span());
}