//
// utc_timestamp.hpp : implements method for time stamping
//
// Copyright (c) 2017-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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
#pragma once

#include <algorithm>
#include <locale>
#include <string>
#include <time.h>

// trim from start (in place)
const auto ltrim = [](std::string& s)
{
#pragma warning (disable: 26486)
   s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
      return !std::isspace(ch, std::locale::classic());
   }));
#pragma warning (default: 26486)
};

// trim from end (in place)
const auto rtrim = [](std::string& s)
{
#pragma warning (disable: 26486)
   s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
      return !std::isspace(ch, std::locale::classic());
   }).base(), s.end());
#pragma warning (default: 26486)
};

// trim from both ends (in place)
const auto trim = [](std::string& s)
{
   ltrim(s);
   rtrim(s);
};

// trim from start (copying)
const auto ltrim_copy = [](std::string s) 
{
   ltrim(s);
   return s;
};

// trim from end (copying)
const auto rtrim_copy = [](std::string s)
{
   rtrim(s);
   return s;
};

// trim from both ends (copying)
const auto trim_copy = [](std::string s) 
{
   trim(s);
   return s;
};

///<summary> get current date and time in UTC</summary>
///<returns> a date time string, the format is system dependent</returns>
const auto utc_timestamp = []()
{
   const time_t now = time(nullptr);
   tm gmtm;
   if (gmtime_s(&gmtm, &now) !=0)
      throw std::domain_error("utc timestamp failed");
   
   char timebuf[26] = { 0 };
   asctime_s(timebuf, &gmtm);
     
   return trim_copy({ timebuf });
};
