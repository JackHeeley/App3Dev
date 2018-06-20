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

#pragma warning (disable:4251 4290)

///<summary> wraps the system memory mapped file facility.</summary>
class EXTENDEDUNIVERSALCPPSUPPORT_API MemoryMappedFile
{

public:
   ///<summary> helper for working in kilobytes</summary>
   static constexpr inline uint64_t kilobytes(uint64_t n) noexcept { return n*1024; };
   
   ///<summary> helper for working in megabytes.</summary>
   static constexpr inline uint64_t megabytes(uint64_t n) noexcept { return n*kilobytes(1024); };

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
   const std::string get_file_path() const;

   ///<summary> get name of memory buffer.</summary>
   ///<returns> text string representing memory buffer name</returns>
   const std::string get_buffer_name() const;

   ///<summary> get buffer size.</summary>
   ///<returns> length memory buffer in bytes.</returns>
   const uint64_t get_buffer_size() const noexcept;

   ///<summary> get buffer as a gsl::span.</summary>
   ///<returns> a gsl span (with content in address space of the memory mapped file)</returns>
   gsl::span<unsigned char> get_span() const;

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
