//
// utf8_convert.cpp : implement conversions to support utf8 on Windows
//
// Copyright (c) 2016-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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

#include <atomic>
#include <codecvt>
#include <stdexcept>  // for std::runtime_error
#include <stdint.h>   // for uint32_t

namespace utf8
{
   // std::wstring_convert is deprecated at C++17 (for security reasons) with no standard alternative yet available.
   // The community advice is to use a library, or build your own, or accept the risk. Microsoft advise using
   // MultiByteToWideChar and WideCharToMultiByte as alternatives. See:
   // 
   //  https://docs.microsoft.com/en-us/archive/msdn-magazine/2016/september/c-unicode-encoding-conversions-with-stl-strings-and-win32-apis
   //
   //  Here we adopt this recommendation (and use the published source with minor changes)...
   //

   // In this source file (and only here) 'throw error_context("reason");' is NOT a viable strategy - due to mutual recursion.
   // We just pull in an alternative from the blog... (and don't even bother to dereference it when catching). These exceptions
   // are handled locally, so this does not impact the SEH design as employed everywhere else.

   // Represents an error during UTF-8 encoding conversions 
   class Utf8ConversionException : public std::runtime_error
   {
      // Error code from GetLastError()
      uint32_t _errorCode;

   public:
      Utf8ConversionException( const char* message, uint32_t errorCode) : 
         std::runtime_error(message),
         _errorCode(errorCode)
      { 
      }
      
      uint32_t ErrorCode() const noexcept
      {
         return _errorCode;
      }
   }; 

   // Exceptions that may arise in the safe conversion, are handled here (we retry using unsafe conversion).
   // Since conversion is part of error reporting, we need to arrange that system errors are not disturbed
   // (in the exception path).
   class RAII_preserve_last_error
   {
      const DWORD _error;

   public:
      ///<summary> preserve thread last error at construction.</summary>
      RAII_preserve_last_error() noexcept :
         _error(::GetLastError())
      {
      }

      ///<summary> no copy constructor.</summary>
      RAII_preserve_last_error(const RAII_preserve_last_error& other) = delete;

      ///<summary> move constructor.</summary>
      RAII_preserve_last_error(RAII_preserve_last_error&& other) noexcept = delete;

      ///<summary> no copy assignment operator.</summary>
      RAII_preserve_last_error& operator=(RAII_preserve_last_error& other) = delete;

      ///<summary> no move assignment operator.</summary>
      RAII_preserve_last_error& operator=(RAII_preserve_last_error&& other) = delete;

      ///<summary> restore thread last error on all paths.</summary>
      ~RAII_preserve_last_error()
      {
         ::SetLastError(_error);
      }
   };

   ///<summary>convert utf8 string to utf16 string using ::MultiByteToWideChar</summary>
   ///<param name='utf8'>utf8 encoded string</param>
   ///<returns>utf16 encoded string representation of utf8</returns>
   ///<remarks>WARNING: AVOID THROWING error_context HERE. This can lead to mutual recursion/infinite loop.</remarks>
   static std::wstring to_utf16_safe(const std::string& utf8)
   {
      std::wstring utf16;
      if (utf8.empty()) return utf16;
      
      constexpr DWORD kFlags = MB_ERR_INVALID_CHARS;

      if (utf8.length() > gsl::narrow_cast<size_t>(std::numeric_limits<int>::max()))
      {
         throw std::overflow_error("Input string too long: size_t-length doesn't fit into int.");
      }

      // Safely convert from size_t (STL string's length) to int (for Win32 APIs)
      const int utf8Length = gsl::narrow_cast<int>(utf8.length());

      const int utf16Length = MultiByteToWideChar(
         CP_UTF8,       // Source string is in UTF-8
         kFlags,        // Conversion flags
         utf8.data(),   // Source UTF-8 string pointer
         utf8Length,    // Length of the source UTF-8 string, in chars
         nullptr,       // Unused - no conversion done in this step
         0              // Request size of destination buffer, in wchar_ts
      );

      if (utf16Length == 0)
      {
         const DWORD error = ::GetLastError();
         throw Utf8ConversionException("Cannot get result string length when converting from UTF-8 to UTF-16 (MultiByteToWideChar failed).", error);
      }

      utf16.resize(utf16Length);

      // Convert from UTF-8 to UTF-16
      const int result = MultiByteToWideChar(
         CP_UTF8,             // Source string is in UTF-8
         kFlags,              // Conversion flags
         utf8.data(),         // Source UTF-8 string pointer
         utf8Length,          // Length of source UTF-8 string, in chars
         &gsl::at(utf16, 0),  // Pointer to destination buffer
         utf16Length          // Size of destination buffer, in wchar_ts          
      );

      if (result == 0)
      {
         const DWORD error = ::GetLastError();
         throw Utf8ConversionException("Cannot convert from UTF-8 to UTF-16 (MultiByteToWideChar failed).", error);
      }

      return utf16;
   }

   ///<summary>convert utf16 string to utf8 string using ::WideCharToMultiByte</summary>
   ///<param name='utf16'>utf16 encoded string</param>
   ///<returns>utf8 encoded string representation of utf16</returns>
   ///<remarks>WARNING: AVOID USING error_context HERE. This can lead to mutual recursion/infinite loop.</remarks>
   static std::string from_utf16_safe(const std::wstring& utf16)
   {
      std::string utf8;
      if (utf16.empty()) return utf8;

      constexpr DWORD kFlags = WC_ERR_INVALID_CHARS;

      if (utf16.length() > gsl::narrow_cast<size_t>(std::numeric_limits<int>::max()))
      {
         throw std::overflow_error("Input string too long: size_t-length doesn't fit into int."); 
      }

      // Safely convert from size_t (STL string's length) to int (for Win32 APIs)
      const int utf16Length = gsl::narrow_cast<int>(utf16.length());

      const int utf8Length = WideCharToMultiByte(
         CP_UTF8,          // TODO: FURTHER CLARIFICATION NEEDED 
         kFlags,           // Conversion flags
         utf16.data(),     // Source UTF-16 string pointer
         utf16Length,      // Length of the source UTF-16 string, in wchar_ts
         nullptr,          // Unused - no conversion done in this step
         0,                // Request size of destination buffer, in chars
         nullptr,          // optional default char (not used)
         nullptr           // optional default char used (not used)
      );

      if (utf8Length == 0)
      {
         const DWORD error = ::GetLastError();
         throw Utf8ConversionException("Cannot get result string length when converting from UTF-16 to UTF-8 (WideCharToMultiByte failed).", error);
      }
      utf8.resize(utf8Length);

      // Convert from UTF-16 to UTF-8
      const int result = WideCharToMultiByte(
         CP_UTF8,             // TODO: FURTHER CLARIFICATION NEEDED 
         kFlags,              // Conversion flags
         utf16.data(),        // Source UTF-16 string pointer
         utf16Length,         // Length of source UTF-16 string, in wchar_ts
         &gsl::at(utf8, 0),   // Pointer to destination buffer
         utf8Length,          // Size of destination buffer, in chars
         nullptr,             // optional default char (not used)
         nullptr              // optional default char used (not used)
      );

      if (result == 0)
      {
         const DWORD error = ::GetLastError();
         throw Utf8ConversionException("Cannot convert from UTF-16 to UTF-8 (WideCharToMultiByte failed).", error);
      }
     
      return utf8;
   }

   // std::wstring_convert is deprecated at C++17 (for security reasons) with no standard alternative yet availble.
   // The community advice is to use a library, or build your own, or accept the risk. Here we keep the deprecated 
   // (unsafe) conversions, but only use them if the safe conversions fail. We accept that this mitigation could 
   // still be exploited.

#pragma warning(disable: 4996)

   ///<summary>convert utf16 string to utf8 string using std::codecvt</summary>
   ///<param name='wstr'>utf16 encoded string</param>
   ///<returns>utf8 encoded string representation of wstr</returns>
   static std::string from_utf16_unsafe(const std::wstring& wstr)
   {
      return std::wstring_convert<std::codecvt_utf8_utf16 <wchar_t>, wchar_t>().to_bytes(wstr);
   }

   ///<summary>convert utf8 string to utf16 string using std::codecvt</summary>
   ///<param name='str'>utf8 encoded string</param>
   ///<returns>utf16 encoded string representation of str</returns>
   static std::wstring to_utf16_unsafe(const std::string& str)
   {
      return std::wstring_convert<std::codecvt_utf8_utf16 <wchar_t>, wchar_t>().from_bytes(str);
   }

#pragma warning(default: 4996)

   ////////////////////////////////////////////////////////////////////////////////////////////////
   //
   //   STILL A GOOD PRINCIPLE TO SUPPLY A PUBLIC BRIDGE TO THE PRIVATE (STATIC) IMPLEMENTATION
   //
   ////////////////////////////////////////////////////////////////////////////////////////////////

   ///<summary>convert utf16 string to utf8 string</summary>
   ///<param name='wstr'>utf16 encoded string</param>
   ///<returns>utf8 encoded string representation of wstr</returns>
   ///<remarks>preserves last error state (convert::from_utf16 is used in exception path)</returns>
   std::string convert::from_utf16(const std::wstring& wstr)
   {
      try
      {
         const RAII_preserve_last_error keep;
         return from_utf16_safe(wstr);
      }
      catch (...)
      {
         return from_utf16_unsafe(wstr);
      }
   }

   ///<summary>convert utf8 string to utf16 string</summary>
   ///<param name='str'>utf8 encoded string</param>
   ///<returns>utf16 encoded string representation of str</returns>
   ///<remarks>preserves last error state (convert::to_utf16 is used in exception path)</returns>
   std::wstring convert::to_utf16(const std::string& str)
   {
      try
      {
         const RAII_preserve_last_error keep;
         return to_utf16_safe(str);
      }
      catch (...)
      {
         return to_utf16_unsafe(str);
      }
   }
}