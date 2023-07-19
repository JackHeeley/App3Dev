//
// utf8_convert.hpp : conversions to support utf8 everywhere coding paradigm
//
// Copyright (c) 2016-2020 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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
#pragma once

#ifdef BASICUNIVERSALCPPSUPPORT_EXPORTS
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <algorithm>
#include <string>

#if defined(__cpp_char8_t)
template<typename T>
const char* u8Cpp20(T&& t) noexcept 
{ 
#pragma warning (disable: 26490)
   return reinterpret_cast<const char*>(t);
#pragma warning (default: 26490)
}
#define U8(x) u8Cpp20(u8##x)
#else
#define U8(x) u8##x
#endif

namespace utf8
{
   ///<summary>convert to and from utf8</summary>
   class convert
   {
   public:

      ///<summary>convert utf16 string to utf8 string</summary>
      ///<param name='wstr'>utf16 encoded string</param>
      ///<returns>utf8 encoded string representation of wstr</returns>
      BASICUNIVERSALCPPSUPPORT_API static std::string from_utf16(const std::wstring& wstr);

      ///<summary>convert utf8 string to utf16 string</summary>
      ///<param name='str'>utf8 encoded string</param>
      ///<returns>utf16 encoded string representation of str</returns>
      BASICUNIVERSALCPPSUPPORT_API static std::wstring to_utf16(const std::string& str);
   };

   ///<summary>counts the number of code points in a utf8 encoded string</summary>
   ///<param name='str'>a utf8 encoded string</param>
   ///<returns>a count of the number of code points found in str.</returns>
   ///<remarks> See https://en.wikipedia.org/wiki/Code_point
   /// convenient because individual character representations in utf8 vary in length.
   /// codepoint_count is always less than or equal to the raw data storage length.</remarks>
   static size_t count_codepoints(const std::string& str)
   {
      return std::count_if(str.begin(), str.end(), [](auto ch) { return ((ch & 0xc0) != 0x80); });
   };

}