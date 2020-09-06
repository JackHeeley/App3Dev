//
// utf8_guid.hpp : conversions to support utf8 everywhere coding paradigm
//
// *** WARNING: this header is specifically for windows platforms ***
//
// Copyright (c) 2019-2020 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#define NOMINMAX
#include <windows.h>

#include <iomanip>
#include <sstream>
#include <string>

#include <gsl/gsl>

namespace utf8
{
   ///<summary>functor supporting stripping all instances of a set of selected characters from a target string. 
   /// Use object as a predicate for std::remove_if in an erase-remove construct/idiom applied to the target.</summary>
   class char_stripper
   {
   private:
      const std::string chars_to_strip;

   public:
      ///<summary>construct the functor</summary>
      ///<param name='some_chars_to_strip'>specifies a specific set of characters to be stripped</param>
      ///<remarks>removes all instance of each of the characters specified from the target string</remarks>
      char_stripper(std::string some_chars_to_strip) :
         chars_to_strip(some_chars_to_strip)
      {
      }

      ///<summary> functor acts as predicate for std::remove_if</summary>
      ///<param name='ch'> a character to be tested for removal</param>
      ///<returns> true if ch is selected for removal, or false if not.</returns>
      bool operator()(char ch) noexcept
      {
         return std::string::npos != chars_to_strip.find_first_of(ch);
      }
   };

   ///<summary>convert selected windows types to and from utf8</summary>
   class guid_convert
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

#pragma warning(disable : 26485) // false positive vc16.7.2
         auto span = gsl::make_span(aGuid.Data4);
#pragma warning(default : 26485) // false positive vc16.7.2

         for (auto& elem : span)
         {
            ss << ", " << "0x" << std::nouppercase << std::setfill('0') << std::setw(2) << std::hex << (unsigned short)elem;
         }

         return ss.str();
      }

      ///<summary>convert utf8 std::string to GUID</summary>
      ///<param name='aGuidString'>a utf8 encoded string representation of aGuid in the form
      /// "0xhhhhhhhhL, 0xhhhh, 0xhhhh, 0xhh, 0xhh, 0xhh, 0xhh, 0xhh, 0xhh, 0xhh, 0xhh"
      /// where h is any hex digit</param>
      ///<returns>const GUID (e.g. as supplied in winioctl.h)</returns>
      static inline GUID to_guid(std::string aGuidString)
      {
         GUID aGuid;
         std::stringstream ss;
         const char_stripper strip("Lx,");

#pragma warning (disable: 26486)
         aGuidString.erase(remove_if(aGuidString.begin(), aGuidString.end(), strip), aGuidString.end());
#pragma warning (default: 26486)

         ss << std::hex << aGuidString;
         ss >> aGuid.Data1 >> aGuid.Data2 >> aGuid.Data3;

#pragma warning(disable : 26485) // false positive vc16.7.2
         auto span = gsl::make_span(aGuid.Data4);
#pragma warning(default : 26485) // false positive vc16.7.2

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