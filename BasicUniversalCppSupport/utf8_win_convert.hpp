//
// utf8_win_convert.hpp : conversions to support utf8 everywhere coding paradigm
//
// *** WARNING: this header is specifically for windows platforms ***
//
// Copyright (c) 2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#define NOMINMAX
#include <windows.h>

#include <iomanip>
#include <sstream>
#include <string>

#include "gsl.hpp"

namespace utf8
{
   ///<summary>convert selected windows types to and from utf8</summary>
   class win_convert
   {
   public:

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