//
// CppUnitTest.hpp :  wrapper for CppUnitTest.h - used to suppress warnings in externally provided source
//
// Copyright (c) 2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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
#define CPPUNITTEST_WARNINGS_SUPRESSED 26429 26432 26433 26440 26443 26455 26461 26466 26477 26485 26486 26490 26496 
#pragma warning(disable: CPPUNITTEST_WARNINGS_SUPRESSED)
#include <CppUnitTest.h>
#pragma warning(default: CPPUNITTEST_WARNINGS_SUPRESSED)