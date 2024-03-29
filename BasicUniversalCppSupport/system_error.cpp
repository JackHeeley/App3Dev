//
// system_error.cpp : implements system error code and message decoding for Windows
//
// Copyright (c) 2017-2020 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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
#include "stdafx.h"

/*
* ***************************************************************************
* PIMPL idiom - private implementation of SystemError class (Rule of 5)
* ***************************************************************************
*/

///<summary> the private implementation of SystemError.</summary>
///<remarks> Windows types used internally, adheres to "utf8 everywhere" paradigm at public interface</remarks>
class SystemError::impl
{
private:
   ///<summary> error code corresponding to a system error.</summary>
   DWORD error_code; 
   
   ///<summary> working buffer for error message.</summary>
   ///<remarks> can be nullptr if error_code is out of valid range or otherwise unformattable.</remarks>
   LPVOID lpBuffer;

   ///<summary> wrap the system message retrieval.</summary>
   void format_message(int error_code, LPVOID& lpBuffer) noexcept
   {
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
         FORMAT_MESSAGE_FROM_SYSTEM |
         FORMAT_MESSAGE_IGNORE_INSERTS,
         nullptr,
         error_code,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
#pragma warning(disable:26490)
         reinterpret_cast<LPTSTR>(&lpBuffer),
#pragma warning(default:26490)
         0,
         nullptr
      );
   }

public:

   ///<summary> default constructor.</summary>
   impl() noexcept :
      error_code(GetLastError()),
      lpBuffer(nullptr)
   {      
      format_message(error_code, lpBuffer);
   }

   ///<summary> constructor for specified error.</summary>
   ///<param name='an_error_code'> the error code to use.</param>
   impl(int an_error_code) noexcept :
      error_code(an_error_code),
      lpBuffer(nullptr)
   {
      format_message(error_code, lpBuffer);
   }

   ///<summary> copy constructor.</summary>
   impl(const impl& other) noexcept :
      error_code(other.error_code),
      lpBuffer(nullptr)
   {
      format_message(error_code, lpBuffer);
   }

   ///<summary> move constructor.</summary>
   impl(impl&& other) noexcept :
      error_code(other.error_code),
      lpBuffer(std::exchange(other.lpBuffer, nullptr))
   {
   }

   ///<summary> copy assignment operator.</summary>
   impl& operator=(impl& other) noexcept
   {
      if (this != &other)
      {
         error_code = other.error_code;
         format_message(error_code, lpBuffer);
      }
      return (*this);
   }

   ///<summary> move assignment operator.</summary>
   impl& operator=(impl&& other) noexcept
   {
      if (this != &other)
      {
         LocalFree(lpBuffer);
         error_code = std::move(other.error_code);
         lpBuffer = std::exchange(other.lpBuffer, nullptr);
      }
      return (*this);
   }

   ///<summary> equals comparison operator.</summary>
   ///<remarks> defines equals to mean accessible data is identical.</remarks>
   bool operator==(const impl& other) const
   {
      if ((this == nullptr) || (&other == nullptr)) return false;       // NOLINT (safety) msvc std::move leaves a null reference to the stale source object, whereas C++ standard requires a valid object (with undefined behaviour) to remain.
      if (this == &other) return true;                                  // same object
      return (
         (error_code == other.error_code) &&
         (this->get_error_text() == other.get_error_text())
      );    // considered equal if public data is identical
   }

   ///<summary> not equals comparison operator.</summary>
   ///<remarks> defines not equals to mean accessible data differs.</remarks>
   bool operator!=(const impl& other) const
   {
      return !(*this == other);
   }

   ///<summary> get error text.</summary>
   const std::string get_error_text() const
   {
      // some error_codes we can construct (e.g. 0x23) aren't current (don't have a message) 
      if (lpBuffer == nullptr)
      {
         std::stringstream ss;
         ss << "error_code (" << error_code << ") out of range, or otherwise unformattable.\r\n";
         return ss.str().c_str();
      }
      return utf8::convert::from_utf16((LPCTSTR)lpBuffer);
   }

   ///<summary> get error code.</summary>
   const int get_error_code() const noexcept
   {
      return (int)error_code;
   }

   ///<summary> clear error code.</summary>
   void clear_error_code() const noexcept
   {
	   SetLastError(ERROR_SUCCESS);
   }

   ///<summary> destructor.</summary>
   ~impl()
   {
      LocalFree(lpBuffer);
   }
};

/*
* ***************************************************************************
* PIMPL idiom - public interface for SystemError implementation (Rule of 0)
* ***************************************************************************
*/

///<summary> constructs a system error for the last system error.</summary>
SystemError::SystemError() noexcept :
#pragma warning (disable:26447)
	pimpl(spimpl::make_impl<impl>())
#pragma warning (default:26447)
{
}

///<summary> constructs a system error from an error code.</summary>
SystemError::SystemError(int errorCode) noexcept :
#pragma warning (disable:26447)
   pimpl(spimpl::make_impl<impl>(errorCode))
#pragma warning (default:26447)
{
}

///<summary> equals comparison operator.</summary>
///<remarks> objects considered equal if impl's are equal.</remarks>
bool SystemError::operator==(const SystemError& other) const
{
   return *pimpl.get() == *other.pimpl.get();   
}

///<summary> not equals comparison operator.</summary>
///<remarks> objects considered not equal if impl's differ.</remarks>
bool SystemError::operator!=(const SystemError& other) const
{
   return !(*this == other);
}

///<summary> gets the system error text from an error code.</summary>
const std::string SystemError::get_error_text() const
{
   return pimpl->get_error_text();
}

///<summary> gets the system error code.</summary>
const int SystemError::get_error_code() const noexcept
{
   return pimpl->get_error_code();
}

///<summary> clears the system error code.</summary>
void SystemError::clear_error_code() const noexcept
{
	pimpl->clear_error_code();
}
