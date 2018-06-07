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

#pragma warning (disable:4251 4290)

template< typename C, typename T, typename A >
std::basic_string<C, T, A> trim(const std::basic_string<C, T, A>& str,
   const std::locale& loc = std::locale::classic())
{
   auto begin = str.begin(); // first non ws from left
   while (begin != str.end() && std::isspace(*begin, loc)) ++begin;
   if (begin == str.end()) return{}; // EDIT

   auto rbegin = str.rbegin(); // first non ws from right
   while (rbegin != str.rend() && std::isspace(*rbegin, loc)) ++rbegin;

   return{ begin, rbegin.base() };
}

///<summary> lambda to get current date and time in UTC</summary>
///<returns> a date time string, the format is system dependent</returns>
const auto utc_timestamp = []()
{
	char timebuf[26] = { 0 };
   time_t now = time(0);
   tm gmtm;
   if (gmtime_s(&gmtm, &now) !=0)
      throw std::domain_error("utc timestamp failed");
   asctime_s(timebuf, &gmtm);
      
   std::string s(timebuf);
   return trim(s);
};

#pragma warning (default:4251 4290)
