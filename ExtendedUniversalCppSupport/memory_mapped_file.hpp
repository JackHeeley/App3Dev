//
// memory_mapped_file.hpp : implements a memory buffer backed by a file.
//
// This implementation uses windows memory mapped files. This is an easy way to provide
// a data object that can be shared between processes. By doing this, the code to
// fill the buffer can be in a seperate process from the buffer consumer.
//
// Memory mapped files are named shared objects which are efficiently backed to disk. 
//
// Copyright (c) 2003-2020 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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
#ifndef __MEMORY_MAPPED_FILE_HPP__
#define __MEMORY_MAPPED_FILE_HPP__

#ifdef EXTENDEDUNIVERSALCPPSUPPORT_EXPORTS
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <string>

#include <gsl.hpp>
#include <spimpl.hpp>

///<summary> wraps the system memory mapped file facility.</summary>
class MemoryMappedFile
{

public:
   ///<summary> helper for working in kilobytes</summary>
   static constexpr inline uint64_t kilobytes(uint64_t n) noexcept { return n*1024; };
   
   ///<summary> helper for working in megabytes.</summary>
   static constexpr inline uint64_t megabytes(uint64_t n) noexcept { return n*kilobytes(1024); };

   ///<summary> construct a memory mapped file object</summary>
   ///<param name='file_path'> path name of file to be used.</param>
   ///<param name='buffer_name'> name of buffer in memory. Processes can share buffer if they know this name</param>
   ///<param name='buffer_size'> size of buffer in bytes.</param>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   EXTENDEDUNIVERSALCPPSUPPORT_API MemoryMappedFile(const std::string& file_path, const std::string& buffer_name, uint64_t buffer_size);

   ///<summary> get path name of disk file used as swap space for buffer</summary>
   ///<returns> text string representing file name</returns>
   EXTENDEDUNIVERSALCPPSUPPORT_API const std::string get_file_path() const;

   ///<summary> get name of memory buffer.</summary>
   ///<returns> text string representing memory buffer name</returns>
   EXTENDEDUNIVERSALCPPSUPPORT_API const std::string get_buffer_name() const;

   ///<summary> get buffer size.</summary>
   ///<returns> length memory buffer in bytes.</returns>
   EXTENDEDUNIVERSALCPPSUPPORT_API const uint64_t get_buffer_size() const noexcept;

   ///<summary> get buffer as a gsl::span.</summary>
   ///<returns> a gsl span (with content in address space of the memory mapped file)</returns>
   EXTENDEDUNIVERSALCPPSUPPORT_API gsl::span<unsigned char> get_span() const;

   ///<summary> commit buffer to disk and release memory.</summary>
   ///<remarks> exposed here for unit testing purposes only (no other use-cases will benefit)</remarks>
   EXTENDEDUNIVERSALCPPSUPPORT_API void release();

private:
   ///<summary> forward reference to private implementation.</summary>
   class impl;

   ///<summary> smart unique pointer to private implementation.</summary>
   ///<remarks> with default copy, move and compare support.</remarks>
   spimpl::impl_ptr<impl> pimpl;
};

#endif // __MEMORY_MAPPED_FILE_HPP_
