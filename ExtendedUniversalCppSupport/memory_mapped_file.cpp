//
// memory_mapped_file.cpp : implements memory mapped file mechanisms
//
// Copyright (c) 2003-2017 Jack Heeley, all rights reserved
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
      hFileMap(0)
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
      hFileMap(0)
   {
      bufferSize.QuadPart = other.bufferSize.QuadPart;
      openFile();

      createFileMapping();
      mapViewOfFile();
   }

   ///<summary> move constructor.</summary>
   impl(impl&& other) noexcept :
      filePathW(other.filePathW),
      bufferNameW(other.bufferNameW),

      buffer_ptr(other.buffer_ptr),
      hFile(other.hFile),
      hFileMap(other.hFileMap)
   {
      bufferSize.QuadPart = other.bufferSize.QuadPart;
      other.hFile = INVALID_HANDLE_VALUE;
      other.hFileMap = 0;
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
         hFileMap = 0;
         
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
         filePathW = other.filePathW;
         bufferNameW = other.bufferNameW;

         buffer_ptr = other.buffer_ptr;
         hFile = other.hFile;
         hFileMap = other.hFileMap;
         bufferSize.QuadPart = other.bufferSize.QuadPart;
      
         ((impl&)other).hFile = INVALID_HANDLE_VALUE;
         ((impl&)other).hFileMap = 0;
      }
      return (*this);
   }

   ///<summary> destructor.</summary>
   impl::~impl()
   {
      release();
   }

   ///<summary> get path name of disk file used as swap space for buffer</summary>
   ///<returns> text string representing file name</returns>
   const std::string get_file_path()
   {
      return utf8::convert::from_utf16(filePathW);
   };

   ///<summary> get name of memory buffer.</summary>
   ///<returns> text string representing memory buffer name</returns>
   const std::string get_buffer_name()
   {
      return utf8::convert::from_utf16(bufferNameW);
   };

   void* get_buffer_address() noexcept
   {
      return buffer_ptr;
   };

   ///<summary> get buffer as a vector of bytes.</summary>
   ///<returns> vector of bytes (in mmf).</returns>
   mmf_vector get_buffer()
   {
      s_buffer_ptr = static_cast<unsigned char*>(buffer_ptr);  // TODO: weak point & not threadsafe
      return mmf_vector(gsl::narrow_cast<size_t>(get_buffer_size()));
   };
   
   ///<summary> get size of memory buffer.</summary>
   ///<returns> size of memory buffer in bytes</returns>
   uint64_t get_buffer_size() noexcept
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
         throw fox_exception(create_file_failed.str().c_str());
      }
   }

   ///<summary> determine the bufferSize by querying the size of the backing file.</summary>
   ///<exception cref='std::exception'> if the operation could not be completed.</exception>
   void impl::sizeBufferToBackingFile() 
   {
      if (hFile == INVALID_HANDLE_VALUE) 
      {
         throw fox_exception("No file handle available");
      }

      bufferSize.LowPart = GetFileSize(hFile, (LPDWORD)(&bufferSize.HighPart));
   }

   ///<summary> relate disk file to memory object.</summary>
   ///<exception cref='std::exception'> if the operation could not be completed.</exception>
   void impl::createFileMapping() 
   {
      hFileMap = CreateFileMapping(hFile,
         NULL,
         PAGE_READWRITE,
         bufferSize.HighPart,
         bufferSize.LowPart,
         bufferNameW.c_str()
      );

      if (hFileMap != 0) 
      {
         if (GetLastError() == ERROR_ALREADY_EXISTS) 
         {
            CloseHandle(hFileMap);
            hFileMap = 0;
            throw fox_exception("CreateFileMapping failed");
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

      if (buffer_ptr == NULL) 
      {
         throw fox_exception("MapViewOfFile failed");
      }
   }

   ///<summary> commit buffer to disk and release memory.</summary>
   void impl::release() 
   {
      if (hFileMap != 0) 
      {
         CloseHandle(hFileMap);
         hFileMap = 0;
      }

      if (hFile != INVALID_HANDLE_VALUE) 
      {
         CloseHandle(hFile);
         hFile = INVALID_HANDLE_VALUE;
      }

      filePathW = L"";
      bufferNameW = L"";
      bufferSize.QuadPart = 0;
      buffer_ptr = NULL;
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
   impl_(spimpl::make_impl<impl>(file_path, buffer_name, buffer_size))
{
}

///<summary> get name of disk file used as swap space for buffer</summary>
///<returns> text string representing file name</returns>
const std::string MemoryMappedFile::get_file_path() 
{ 
   return impl_->get_file_path();
};

///<summary> get name of memory buffer.</summary>
///<returns> text string representing memory buffer name</returns>
const std::string MemoryMappedFile::get_buffer_name() 
{ 
   return impl_->get_buffer_name();
};

///<summary> get base address of buffer (a user space virtual address).</summary>
///<returns> address of memory buffer</returns>
void* MemoryMappedFile::get_buffer_address() noexcept
{
   return impl_->get_buffer_address();
};

///<summary> get buffer as a vector of bytes.</summary>
///<returns> vector of bytes (in mmf)</returns>
mmf_vector MemoryMappedFile::get_buffer()
{
   return impl_->get_buffer();
};

///<summary> get base address of buffer (a user space virtual address).</summary>
///<returns> address of memory buffer</returns>
uint64_t MemoryMappedFile::get_buffer_size()
{
   return impl_->get_buffer_size();
};

///<summary> commit buffer to disk and release memory.</summary>
void MemoryMappedFile::release()
{
   impl_->release();
}
