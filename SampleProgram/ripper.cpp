//
// ripper.cpp : implements the ripper helper class 
//
// supplies a functor for the console application.
//
// Copyright (c) 2018 Jack Heeley, all rights reserved
//
#include "stdafx.h"

#include "tray_door_lock.hpp"
#include "ripper.hpp"

///<summary> the file logger in use.</summary>
extern file_logger Logger;


///<summary> construct a ripper.</summary>
///<param name='devicePath'> the utf8 name of a raw system cdrom device containing media.</param>
///<param name='filePath'> the utf8 name of a file to receive the (iso 9660) image.</param>
Ripper::Ripper(std::string devicePath, std::string filePath) :
      m_cdr(devicePath),
      m_mmf(filePath, "", m_cdr.get_image_size())
{
   LOG_INFO(std::string("Ripper Device ").append(devicePath));
   LOG_INFO(std::string("Ripping to ").append(filePath));
}

///<summary> functor to perform the rip operation.</summary>
void Ripper::operator()()
{
   TrayDoorLock lock(m_cdr);
   const auto span = m_mmf.get_span();
   m_cdr.get_image(span);
}