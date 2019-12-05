//
// utf8_convert.hpp : conversions to support utf8 everywhere coding paradigm
//
// Copyright (c) 2016-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
#pragma once

#include <codecvt>
#include <iomanip>
#include <locale>
#include <string>

namespace utf8
{
   ///<summary>convert to and from utf8</summary>
   class convert
   {
   public:

      ///<summary>convert utf16 string to utf8 string</summary>
      ///<param name='wstr'>utf16 encoded string</param>
      ///<returns>utf8 encoded string representation of wstr</returns>
      static inline std::string from_utf16(const std::wstring& wstr)
      {
#pragma warning(disable: 4996)
         return std::wstring_convert<std::codecvt_utf8_utf16 <wchar_t>, wchar_t>().to_bytes(wstr);
#pragma warning(default: 4996)
      }

      ///<summary>convert utf8 string to utf16 string</summary>
      ///<param name='str'>utf8 encoded string</param>
      ///<returns>utf16 encoded string representation of str</returns>
      static inline std::wstring to_utf16(const std::string& str)
      {
#pragma warning(disable: 4996)
         return std::wstring_convert<std::codecvt_utf8_utf16 <wchar_t>, wchar_t>().from_bytes(str);
#pragma warning(default: 4996)
      }
   };
}