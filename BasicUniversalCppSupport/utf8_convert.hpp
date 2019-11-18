//
// utf8_convert.hpp : conversions to support utf8 everywhere coding paradigm
//
// Copyright (c) 2016-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
#pragma once

#include <locale>
#include <string>
#include <codecvt>
#include <iomanip>

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
      ///<returns>a utf8 encoded string representation of aGuid in the form
      /// "0xhhhhhhhhL, 0xhhhh, 0xhhhh, 0xhh, 0xhh, 0xhh, 0xhh, 0xhh, 0xhh, 0xhh, 0xhh"
      /// where h is any hex digit (lowercase)</returns>
      static inline std::string from_guid(const GUID aGuid)
      {
         std::stringstream ss;
         ss << "0x" << std::nouppercase << std::setfill('0') << std::setw(8) << std::hex << aGuid.Data1 <<"L";   
         ss << ", " << "0x" << std::nouppercase << std::setfill('0') << std::setw(4) << std::hex << aGuid.Data2;
         ss << ", " << "0x" << std::nouppercase << std::setfill('0') << std::setw(4) << std::hex << aGuid.Data3;

         auto span = gsl::as_span(aGuid.Data4);

         for (auto& elem : span)
         {
            ss << ", " << "0x" << std::nouppercase << std::setfill('0') << std::setw(2) << std::hex << (unsigned short)elem;
         }

         return ss.str();
      }

      ///<summary>convert utf8 std::string to GUID</summary>
      ///<param name='aGuidString'>a utf8 encoded string representation of aGuid in the form
      /// "0xhhhhhhhhL, 0xhhhh, 0xhhhh, 0xhh, 0xhh, 0xhh, 0xhh, 0xhh, 0xhh, 0xhh, 0xhh"
      /// where h is any hex digit (lowercase)
      ///<returns>const GUID (e.g. as supplied in winioctl.h)</returns>
      static inline GUID to_guid(const std::string aGuidString)
      {
         GUID aGuid;
         std::stringstream ss;
         std::string str(aGuidString);

         auto const removeCharsFromString = [&str](gsl::not_null<const char*>(charsToRemove))
         {
            for (unsigned int i = 0; i < strlen(charsToRemove); ++i)
#pragma warning(disable: 26486)
               str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
#pragma warning(default: 26486)
         };

         removeCharsFromString("Lx,");
         ss << std::hex << str;
         ss >> aGuid.Data1 >> aGuid.Data2 >> aGuid.Data3;

         auto span = gsl::as_span(aGuid.Data4);
         for (auto& elem : span)
         {
            int value;
            ss >> value;
            elem = value;
         }

         return aGuid;
      }

   };
}