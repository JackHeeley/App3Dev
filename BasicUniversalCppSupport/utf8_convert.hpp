//
// utf8_convert.hpp : conversions to support utf8 everywhere coding paradigm
//
// Copyright (c) 2016-2017 Jack Heeley, all rights reserved
//
#pragma once

#include <locale>
#include <string>
#include <codecvt>

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
         return std::wstring_convert<std::codecvt_utf8_utf16 <wchar_t>, wchar_t>().to_bytes(wstr);
      }

      ///<summary>convert utf8 string to utf16 string</summary>
      ///<param name='str'>utf8 encoded string</param>
      ///<returns>utf16 encoded string representation of str</returns>
      static inline std::wstring to_utf16(const std::string& str)
      {
         return std::wstring_convert<std::codecvt_utf8_utf16 <wchar_t>, wchar_t>().from_bytes(str);
      }

      ///<summary>convert GUID to utf8 std::string</summary>
      ///<param name='aGuid'>const GUID (e.g. as supplied in winioctl.h)</param>
      ///<returns>utf8 encoded string representation of aGuid</returns>
      static inline std::string from_guid(const GUID aGuid)
      {
         return (utf8::convert::from_utf16(reinterpret_cast<const wchar_t*>(&aGuid)));
      }
   };
}