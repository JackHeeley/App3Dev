//
// memory_mapped_file.hpp : implements a memory buffer backed by a file.
//
// This implementation uses windows memory mapped files. This is an easy way to provide
// a data object that can be shared between processes. By doing this, the code to
// fill the buffer can be in a seperate process from the buffer consumer.
//
// Memory mapped files are named shared objects which are efficiently backed to disk. 
//
// Copyright (c) 2003-2017 Jack Heeley, all rights reserved
//
#pragma once

#ifdef EXTENDEDUNIVERSALCPPSUPPORT_EXPORTS
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define EXTENDEDUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <memory>
#include <string>

#include <spimpl.hpp>

#include "mmf_vector.hpp"

#pragma warning (disable:4251 4290)

///<summary> provides support for custom allocator (see mmf_vector.hpp).</summary>
extern "C"
{
   ///<summary> static address of a MemoryMappedFile buffer.</summary>
   ///<remarks> c++14 limits allocators to be stateless, so this is as good as it gets.</remarks>
   static unsigned char* s_buffer_ptr;

   ///<summary> simple (thread unsafe) mmf_malloc.</summary>
   ///<returns> pointer to allocated memory (within memory mapped file buffer).</returns>
   inline void* __cdecl mmf_malloc(size_t _Size)
   {
      size_t s_buffer_initial = reinterpret_cast<size_t>(s_buffer_ptr);
      s_buffer_ptr = reinterpret_cast<unsigned char*>(s_buffer_initial + _Size);
      return reinterpret_cast<unsigned char*>(s_buffer_initial);
   };

   ///<summary> simple (thread unsafe) mmf_free.</summary>
   ///<remarks> actual delete is deferred (and done by memory mapped file).</remarks>
   inline void __cdecl mmf_free(void* _Block) { };
}

///<summary> wraps the system memory mapped file facility.</summary>
class EXTENDEDUNIVERSALCPPSUPPORT_API MemoryMappedFile
{

public:
   ///<summary> helper for working in kilobytes</summary>
   static inline uint32_t kilobytes(int n) { return n*1024; };
   
   ///<summary> helper for working in megabytes.</summary>
   static inline uint32_t megabytes(int n) { return n*kilobytes(1024); };

   ///<summary> construct a memory mapped file object</summary>
   ///<param name='file_path'> path name of file to be used.</param>
   ///<param name='buffer_name'> name of buffer in memory. Processes can share buffer if they know this name</param>
   ///<param name='buffer_size'> size of buffer in bytes. If zero the buffer size will be chosen to fit the 
   /// size of the actual contents of the swap space file specified by parameter 1. The value can be interrogated with get_buffer_size().</param>
   ///<remarks>The zero buffer size option supports re-construction from earlier buffer data created in a previous session
   /// (and persisted to the backing disk file when the buffer was released).</remarks>
   ///<exception cref='std::exception'>if the operation could not be completed.</exception>
   MemoryMappedFile(const std::string& file_path, const std::string& buffer_name, uint64_t buffer_size);

   ///<summary> get path name of disk file used as swap space for buffer</summary>
   ///<returns> text string representing file name</returns>
   const std::string get_file_path();

   ///<summary> get name of memory buffer.</summary>
   ///<returns> text string representing memory buffer name</returns>
   const std::string get_buffer_name();

   ///<summary> get base address of buffer (a user space virtual address).</summary>
   ///<returns> address of memory buffer</returns>
   void* get_buffer_address();

   ///<summary> get buffer as a vector of bytes.</summary>
   ///<returns> a vector of bytes (with content in address space of the memory mapped file)</returns>
   mmf_vector get_buffer();

   ///<summary> get buffer size.</summary>
   ///<returns> length memory buffer in bytes.</returns>
   uint64_t get_buffer_size();

   ///<summary> commit buffer to disk and release memory.</summary>
   void release();

protected:

private:
   ///<summary> forward reference to private implementation.</summary>
   class impl;

   ///<summary> smart unique pointer to private implementation.</summary>
   ///<remarks> with default copy, move and compare support.</remarks>
   spimpl::impl_ptr<impl> impl_;
};

#pragma warning (default:4251 4290)
