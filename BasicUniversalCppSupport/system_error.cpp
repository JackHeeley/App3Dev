//
// system_error.cpp : implements system error code and message decoding for Windows
//
// Copyright (c) 2017 Jack Heeley, all rights reserved
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
   void format_message(int error_code, LPVOID& lpBuffer)
   {
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
         FORMAT_MESSAGE_FROM_SYSTEM |
         FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL,
         error_code,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
         (LPTSTR)& lpBuffer,
         0,
         NULL
      );
   }

public:

   ///<summary> default constructor.</summary>
   impl() :
      error_code(GetLastError()),
      lpBuffer(nullptr)
   {      
      format_message(error_code, lpBuffer);
   }

   ///<summary> constructor for specified error.</summary>
   ///<param name='an_error_code'> the error code to use.</param>
   impl(int an_error_code) :
      error_code(an_error_code),
      lpBuffer(nullptr)
   {
      format_message(error_code, lpBuffer);
   }

   ///<summary> copy constructor.</summary>
   impl(const impl& other) :
      error_code(other.error_code),
      lpBuffer(nullptr)
   {
      format_message(error_code, lpBuffer);
   }

   ///<summary> move constructor.</summary>
   impl(impl&& other) :
      error_code(other.error_code),
      lpBuffer(other.lpBuffer)
   {
      other.lpBuffer = nullptr;
   }

   ///<summary> copy assignment operator.</summary>
   impl& impl::operator=(impl& other)
   {
      if (this != &other)
      {
         error_code = other.error_code;
         format_message(error_code, lpBuffer);
      }
      return (*this);
   }

   ///<summary> move assignment operator.</summary>
   impl& impl::operator=(impl&& other)
   {
      if (this != &other)
      {
         error_code = other.error_code;
         lpBuffer = other.lpBuffer;
         other.lpBuffer = nullptr;
      }
      return (*this);
   }

   ///<summary> equals comparison operator.</summary>
   ///<remarks> defines equals to mean accessible data is identical.</remarks>
   bool impl::operator==(const impl& other) const
   {
      if ((this == nullptr) || (&other == nullptr)) return false;       // moved object
      if (this == &other) return true;                                  // same object
      return (
         (error_code == other.error_code) &&
         (this->get_error_text() == other.get_error_text())
      );    // considered equal if public data is identical
   }

   ///<summary> not equals comparison operator.</summary>
   ///<remarks> defines not equals to mean accessible data differs.</remarks>
   bool impl::operator!=(const impl& other) const
   {
      return !(*this == other);
   }

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

   const int get_error_code() const
   {
      return (int)error_code;
   }

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
SystemError::SystemError() :
   impl_(spimpl::make_impl<impl>())
{
}

///<summary> constructs a system error from an error code.</summary>
SystemError::SystemError(int errorCode) :
   impl_(spimpl::make_impl<impl>(errorCode))
{
}

///<summary> equals comparison operator.</summary>
///<remarks> objects considered equal if impl's are equal.</remarks>
bool SystemError::operator==(const SystemError& other) const
{
   return *impl_.get() == *other.impl_.get();   
}

///<summary> not equals comparison operator.</summary>
///<remarks> objects considered not equal if impl's differ.</remarks>
bool SystemError::operator!=(const SystemError& other) const
{
   return !(*this == other);
}

///<summary> gets the system error text from an error code.</summary>
std::string SystemError::get_error_text() const
{
   return impl_->get_error_text();
}

///<summary> gets the system error code.</summary>
int SystemError::get_error_code() const
{
   return impl_->get_error_code();
}