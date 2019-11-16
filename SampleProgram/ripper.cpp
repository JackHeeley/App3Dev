//
// ripper.cpp : implements the ripper helper class 
//
// supplies a functor to rip the CD image for the console application. 
//
// Choosing a memory mapped file for the read buffer means that immediately the get_image() from
// CdRomDevice has completed, the data is instantly available as a file in the filesystem (cache).

// The operating system will automatically take care of keeping a permanent copy (on disk), in a 
// later timeframe, and without us seeing any loss of service, or having to explicitly code any 
// write-side instructions. Persistence of data on disk happens at a time of system choosing, and
// at some point after the memory_mapped_file destructor closes the handles on the view and file.
//
// This approach can be anything up to twice as responsive as coding explict write-side data saving
// to a normal disk file, as that would mean waiting for another step before the data is externally 
// available. This is an important design consideration, due to the time/data quantities involved.
//
// Copyright (c) 2018-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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