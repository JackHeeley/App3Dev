//
// UnitTestBasicUniversalCppSupport.hpp : shareed header for a utf8 everywhere component unit test project
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
#ifndef __UNIT_TEST_BASIC_UNIVERSAL_CPP_SUPPORT_HPP__
#define __UNIT_TEST_BASIC_UNIVERSAL_CPP_SUPPORT_HPP__

#include <string>
#include "utf8_guid.hpp"


// TODO: We can't control the  test sequence, so there is a race to name the (single) logfile (as used by default logger macros).
// The Current logger_factory implementation doesn't even indicate who won, so unit tests can fail simply because they hard code 
// a wrong log_file_name assumption. The *temporary* fix is just to use/replicate the same log_file_name in ALL unit tests (over 
// ALL namespaces). But this can be improved!

namespace UnitTestBasicUniversalCppSupport
{
   const std::string log_file_name("UnitTest.log");
};

// useful macro conversions for use in unit tests
#define STRINGIZE(x) #x
#define TO_STRING_LITERAL(x) STRINGIZE(x)

namespace Microsoft
{
   namespace VisualStudio
   {
      namespace CppUnitTestFramework
      {
         // This template specialization allows utf8::Assert::AreEqual to compare GUIDs 
         // It is needed to generate diagnostic text as shown in the test explorer summary pane.
         template<>
         static std::wstring ToString<_GUID>(const struct _GUID& t) 
         { 
            return utf8::convert::to_utf16(utf8::guid_convert::from_guid(t)); 
         }

         // TODO: add other specializations here as required
      }
   }
}

#endif // __UNIT_TEST_BASIC_UNIVERSAL_CPP_SUPPORT_HPP__
