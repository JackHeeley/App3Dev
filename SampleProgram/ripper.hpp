//
// ripper.hpp : implements the ripper helper class
//
// Copyright (c) 2005-2018 Jack Heeley, all rights reserved
//
#pragma once

///<summary> functor class to rip image.</summary>
class Ripper
{
private:
   ///<summary> the cdrom to be ripped.</summary>
   CdromDevice m_cdr;

   ///<summary> the (memory mapped) file to receive the (iso 9660) image.</summary>
   MemoryMappedFile m_mmf;

public:
   ///<summary> construct a ripper.</summary>
   ///<param name='devicePath'> the utf8 name of a raw system cdrom device containing media.</param>
   ///<param name='filePath'> the utf8 name of a file to receive the (iso 9660) image.</param>
   Ripper(std::string devicePath, std::string filePath);

   ///<summary> functor to perform the rip operation.</summary>
   void operator()();
};
