//
// gsl.hpp : wrapper for gsl.h - used to suppress warnings in externally provided source
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
#define GSL_WARNINGS_SUPPRESSED 4996 26429 26440 26434 26472 26473 26486 26496 26814
#pragma warning(disable: GSL_WARNINGS_SUPPRESSED)
#include <gsl.h>
#pragma warning(default: GSL_WARNINGS_SUPPRESSED)
