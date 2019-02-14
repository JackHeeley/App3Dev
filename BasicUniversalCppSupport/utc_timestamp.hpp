//
// utc_timestamp.hpp : implements method for time stamping
//
// Copyright (c) 2017 Jack Heeley, all rights reserved
//
#pragma once

#ifdef BASICUNIVERSALCPPSUPPORT_EXPORTS
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <string>
#include <time.h>
#include <locale>
#include <algorithm>

#pragma warning (disable: 26444 26489)

// trim from start (in place)
static inline void ltrim(std::string &s) 
{
   s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
      return !std::isspace(ch, std::locale::classic());
   }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
   s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
      return !std::isspace(ch, std::locale::classic());
   }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
   ltrim(s);
   rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
   ltrim(s);
   return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
   rtrim(s);
   return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
   trim(s);
   return s;
}

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

//#pragma warning (default: 26444 26489 )
