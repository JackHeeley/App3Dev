//
// memory_mapped_file.cpp : implements memory mapped file mechanisms
//
// see https://docs.microsoft.com/en-gb/windows/win32/memory/file-mapping
//
// Copyright (c) 2003-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
//    This program is free software : you can redistribute itand /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.If not, see < http://www.gnu.org/licenses/>.
//
#include "stdafx.h"
#include "memory_mapped_file.hpp"

/*
* ***************************************************************************
* PIMPL idiom - private implementation of MemoryMappedFile class
* ***************************************************************************
*/

///<summary> the private implementation of MemoryMappedFile.</summary>
///<remarks> Windows types used internally, adheres to "utf8 everywhere" paradigm at public interface</remarks>
class MemoryMappedFile::impl
{
private:
   ///<summary> text (unicode/ascii) representation of file name.</summary> 
   std::wstring filePathW;

   ///<summary> text (unicode/ascii) representation of buffer name.</summary>
   std::wstring bufferNameW;

   ///<summary> size of buffer in bytes.</summary>
   LARGE_INTEGER bufferSize;

   ///<summary> buffer address</summary>
   LPVOID buffer_ptr;

   ///<summary> handle to disk file backing the memory buffer.</summary>
   HANDLE hFile;

   ///<summary> handle to file map object representing the memory buffer.</summary>
   HANDLE hFileMap;

public:

   ///<summary> construct a memory mapped file object.</summary>
   ///<param name='aFilePath'> path name of file to be used.</param>
   ///<param name='aBufferName'> name of buffer in memory. Processes can share buffer if they know this name.</param>
   ///<param name='aBufferSize'> reference to a DWORD containing size of buffer in bytes.
   /// If this DWORD contains zero the buffer size will be chosen to fit the size of the actual contents of 
   /// the swap space file specified by parameter 1. The value can be interrogated with get_buffer_size().</param>
   ///<remarks> This option supports re-construction from earlier buffer data created in a previous session
   /// (and persisted to the backing disk file when the buffer is released).</remarks>
   ///<exception cref='std::exception'> if the operation could not be completed.</exception>
   impl(const std::string& aFilePath, const std::string& aBufferName, uint64_t aBufferSize) :
      filePathW(utf8::convert::to_utf16(aFilePath)),
      bufferNameW(utf8::convert::to_utf16(aBufferName)),

      buffer_ptr(nullptr),
      hFile(INVALID_HANDLE_VALUE),
      hFileMap(nullptr)
   {
      bufferSize.QuadPart = aBufferSize;
      openFile();

      if (aBufferSize == 0) 
      {
         sizeBufferToBackingFile();
      }

      createFileMapping();
      mapViewOfFile();
   }
   
   ///<summary> copy constructor.</summary>
   impl(const impl& other) :
      filePathW(other.filePathW),
      bufferNameW(other.bufferNameW),

      buffer_ptr(nullptr),
      hFile(INVALID_HANDLE_VALUE),
      hFileMap(nullptr)
   {
      bufferSize.QuadPart = other.bufferSize.QuadPart;
      openFile();

      createFileMapping();
      mapViewOfFile();
   }

   ///<summary> move constructor.</summary>
   impl(impl&& other) noexcept :
#pragma warning(disable:26447)
      filePathW(other.filePathW),
      bufferNameW(other.bufferNameW),
#pragma warning(default:26447)

      buffer_ptr(other.buffer_ptr),
      hFile(other.hFile),
      hFileMap(other.hFileMap)
   {
      bufferSize.QuadPart = other.bufferSize.QuadPart;
      other.hFile = INVALID_HANDLE_VALUE;
      other.hFileMap = nullptr;
   }

   ///<summary> copy assignment operator.</summary>
   impl& impl::operator=(impl& other)
   {
      if (this != &other)
      {
         filePathW = other.filePathW;
         bufferNameW = other.bufferNameW;

         buffer_ptr = nullptr;
         hFile = INVALID_HANDLE_VALUE;
         hFileMap = nullptr;
         
         bufferSize.QuadPart = other.bufferSize.QuadPart;
         openFile();

         createFileMapping();
         mapViewOfFile();
      }
      return (*this);
   }

   ///<summary> move assignment operator.</summary>
   impl& impl::operator=(impl&& other) noexcept
   {
      if (this != &other)
      {
         try
         {
            filePathW = other.filePathW;
            bufferNameW = other.bufferNameW;
         }
         catch (const std::exception& e)
         {
            LOG_WARNING(e.what());
         }

         buffer_ptr = other.buffer_ptr;
         hFile = other.hFile;
         hFileMap = other.hFileMap;
         bufferSize.QuadPart = other.bufferSize.QuadPart;
      
         ((impl&)other).hFile = INVALID_HANDLE_VALUE;
         ((impl&)other).hFileMap = nullptr;
      }
      return (*this);
   }

   ///<summary> destructor.</summary>
   impl::~impl()
   {
      try
      {
         release();
      }
      catch (const std::exception& e)
      {
         LOG_WARNING(e.what());
      }
   }

   ///<summary> get path name of disk file used as swap space for buffer</summary>
   ///<returns> text string representing file name</returns>
   const std::string get_file_path() const
   {
      return utf8::convert::from_utf16(filePathW);
   };

   ///<summary> get name of memory buffer.</summary>
   ///<returns> text string representing memory buffer name</returns>
   const std::string get_buffer_name() const
   {
      return utf8::convert::from_utf16(bufferNameW);
   };

   ///<summary> get buffer as a gsl::span.</summary>
   ///<returns> gsl::span (in mmf).</returns>
   gsl::span<unsigned char> get_span() const
   {
      return gsl::as_span<unsigned char>(static_cast<unsigned char*>(buffer_ptr), gsl::narrow<ptrdiff_t>(bufferSize.QuadPart));
   };
   
   ///<summary> get size of memory buffer.</summary>
   ///<returns> size of memory buffer in bytes</returns>
   const uint64_t get_buffer_size() const noexcept
   {
       return bufferSize.QuadPart;
   };

   ///<summary> create disk file to back memory buffer</summary>
   ///<exception cref='std::exception'> if the operation could not be completed.</exception>
   void impl::openFile() 
   {
      hFile = CreateFile(filePathW.c_str(),
         GENERIC_READ | GENERIC_WRITE,
         FILE_SHARE_READ | FILE_SHARE_WRITE,
         NULL,
         OPEN_ALWAYS,
         0,
         NULL
      );

      if (hFile == INVALID_HANDLE_VALUE) 
      {
         std::stringstream create_file_failed; create_file_failed << "CreateFile(\"" << utf8::convert::from_utf16(filePathW) << "\", ...) failed";
         throw error_context(create_file_failed.str().c_str());
      }
   }

   ///<summary> determine the bufferSize by querying the size of the backing file.</summary>
   ///<exception cref='std::exception'> if the operation could not be completed.</exception>
   void impl::sizeBufferToBackingFile() 
   {
      if (hFile == INVALID_HANDLE_VALUE) 
      {
         throw error_context("No file handle available");
      }

      bufferSize.LowPart = GetFileSize(hFile, (LPDWORD)(&bufferSize.HighPart));
   }

   ///<summary> relate disk file to memory object.</summary>
   ///<exception cref='std::exception'> if the operation could not be completed.</exception>
   void impl::createFileMapping() 
   {
      hFileMap = CreateFileMapping(hFile,
         nullptr,
         PAGE_READWRITE,
         bufferSize.HighPart,
         bufferSize.LowPart,
         bufferNameW.c_str()
      );

      if (hFileMap != nullptr) 
      {
         if (GetLastError() == ERROR_ALREADY_EXISTS) 
         {
            CloseHandle(hFileMap);
            hFileMap = nullptr;
            throw error_context("CreateFileMapping failed");
         }
      }
   }

   ///<summary> map view of file into memory.</summary>
   ///<exception cref='std::exception'> if the operation could not be completed.</exception>
   void impl::mapViewOfFile() 
   {
      buffer_ptr = MapViewOfFile(hFileMap,
         FILE_MAP_READ | FILE_MAP_WRITE,
         0,
         0,
         0  // entire file
      );

      if (buffer_ptr == nullptr) 
      {
         throw error_context("MapViewOfFile failed");
      }
   }

   ///<summary> commit buffer to disk and release memory.</summary>
   void impl::release() 
   {
      if (hFileMap != nullptr) 
      {
         CloseHandle(hFileMap);
         hFileMap = nullptr;
      }

      if (hFile != INVALID_HANDLE_VALUE) 
      {
         CloseHandle(hFile);
         hFile = INVALID_HANDLE_VALUE;
      }

      filePathW = L"";
      bufferNameW = L"";
      bufferSize.QuadPart = 0;
      buffer_ptr = nullptr;
   }
};

/*
* ***************************************************************************
* PIMPL idiom - public interface for MemoryMappedFile implementation
* ***************************************************************************
*/

///<summary> construct a memory mapped file object</summary>
///<param name='file_path'> path name of file to be used.</param>
///<param name='buffer_name'> name of buffer in memory. Processes can share buffer if they know this name</param>
///<param name='buffer_size'> size of buffer in bytes. If zero the buffer size will be chosen to fit the 
/// size of the actual contents of the swap space file specified by parameter 1. The value can be interrogatedwith get_buffer_size().</param>
///<remarks>The zero buffer size option supports re-construction from earlier buffer data created in a previous session
/// (and persisted to the backing disk file when the buffer was released).</remarks>
///<exception cref='std::exception'>if the operation could not be completed.</exception>
MemoryMappedFile::MemoryMappedFile(const std::string& file_path, const std::string& buffer_name, uint64_t buffer_size) :
   pimpl(spimpl::make_impl<impl>(file_path, buffer_name, buffer_size))
{
}

///<summary> get name of disk file used as swap space for buffer</summary>
///<returns> text string representing file name</returns>
const std::string MemoryMappedFile::get_file_path() const
{ 
   return pimpl->get_file_path();
};

///<summary> get name of memory buffer.</summary>
///<returns> text string representing memory buffer name</returns>
const std::string MemoryMappedFile::get_buffer_name() const
{ 
   return pimpl->get_buffer_name();
};

///<summary> get buffer as a gsl::span.</summary>
///<returns> gsl::span (in mmf)</returns>
gsl::span<unsigned char> MemoryMappedFile::get_span() const
{
   return pimpl->get_span();
};

///<summary> get base address of buffer (a user space virtual address).</summary>
///<returns> address of memory buffer</returns>
const uint64_t MemoryMappedFile::get_buffer_size() const noexcept
{
   return pimpl->get_buffer_size();
};

///<summary> commit buffer to disk and release memory.</summary>
void MemoryMappedFile::release()
{
   pimpl->release();
}
