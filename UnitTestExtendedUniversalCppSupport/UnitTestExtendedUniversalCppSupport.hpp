//
// UnitTestExtendedUniversalCppSupport.hpp : shared header for a utf8 everywhere component unit test project
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

#include <string>

// TODO: We can't control the  test sequence, so there is a race to name the (single) logfile (as used by default logger macros).
// The Current logger_factory implementation doesn't even indicate who won, so unit tests can fail simply because they hard code 
// a wrong log_file_name assumption. The *temporary* fix is just to use/replicate the same log_file_name in ALL unit tests (over 
// ALL namespaces). But this can be improved!

namespace UnitTestExtendedUniversalCppSupport
{
   const std::string log_file_name("UnitTest.log");
};

// useful macro conversions for use in unit tests@END
#ifndef STRINGIZE
#define STRINGIZE(x) #x
#define TO_STRING_LITERAL(x) STRINGIZE(x)
#endif



/// <summary>
/// RAII impersonation helper available for use in various tests.
/// </summary>
class RAII_impersonate
{

public:
   RAII_impersonate() noexcept
   {
      if (!ImpersonateSelf(SecurityAnonymous))
      {
         LOG_ERROR("RAII_impersonate ImpersonateSelf failed");
      }
   }

   ///<summary> deleted copy constructor.</summary>
   RAII_impersonate(RAII_impersonate& other) = delete;

   ///<summary> deleted move constructor.</summary>
   RAII_impersonate(RAII_impersonate&& other) = delete;

   ///<summary> deleted copy assignment.</summary>
   RAII_impersonate& operator=(RAII_impersonate& other) = delete;

   ///<summary> deleted move assignment.</summary>
   RAII_impersonate& operator=(RAII_impersonate&& other) = delete;

   ~RAII_impersonate()
   {
      RevertToSelf();
   }
};
