//
// gsl.hpp : wrapper for gsl.h - suppress warnings in externally provided source
//
// This wrapper allows a foreign supplied header file to be used without modification when 
// compiling in a Code Analysis regime with Microsoft All Rules active. Without this wrapper
// the build output space would be pulluted with (acceptable) warnings arising from the
// work of other parties, who have not (or not yet) adopted this level of code analysis.
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
#define GSL_WARNINGS_SUPPRESSED 26472 26481
#pragma warning(disable: GSL_WARNINGS_SUPPRESSED)
#include <gsl/gsl>
#pragma warning(default: GSL_WARNINGS_SUPPRESSED)
