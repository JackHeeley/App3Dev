//
// utf8_console.cpp : implement configuration of console to support utf8 on Windows
//
// Copyright (c) 2016-2020 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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
#include "stdafx.h"

namespace utf8
{

   ///<summary>configure the system console to accept utf8 strings</summary>
   ///<returns>If the function succeeds, the return value is nonzero. If the function fails, the return value is zero.</returns>
   ///<remarks>To get extended error information, call GetLastError.</remarks>
   bool console::configure_codepage(void) noexcept
   {
      return SetConsoleOutputCP(CP_UTF8);
   }
}