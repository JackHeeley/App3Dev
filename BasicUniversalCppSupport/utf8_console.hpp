//
// utf8_console.hpp : configuration of console is required for windows
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
#ifndef __UTF8_CONSOLE_HPP__
#define __UTF8_CONSOLE_HPP__

#ifdef BASICUNIVERSALCPPSUPPORT_EXPORTS
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

namespace utf8
{
   ///<summary>console configuration support</summary>
   class console
   {
   public:

      ///<summary>configure the console output to use utf8 string</summary>
      BASICUNIVERSALCPPSUPPORT_API static bool configure_codepage(void) noexcept;

   };
}

#endif // __UTF8_CONSOLE_HPP__
