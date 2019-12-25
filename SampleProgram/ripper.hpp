//
// ripper.hpp : implements the ripper helper class
//
// Copyright (c) 2005-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
#pragma once

#include <atomic>

///<summary> functor class to rip image.</summary>
class Ripper
{
private:
   ///<summary> the cdrom to be ripped.</summary>
   CdromDevice m_cdr;

public:
   ///<summary> construct a ripper.</summary>
   ///<param name='devicePath'> the utf8 name of a raw system cdrom device containing optical media disk.</param>
   Ripper(std::string devicePath);

   ///<summary> functor to perform the rip operation.</summary>
   ///<param name='filePath'> the utf8 name of a file to receive the (iso 9660) image.</param>
   ///<param name='a_progress'> reference to where percentage read progress will be maintained (during the rip operation).</param>
   void operator()(std::string filePath, std::atomic<int>& a_progress);
};
