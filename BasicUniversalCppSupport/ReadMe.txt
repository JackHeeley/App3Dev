========================================================================
    DYNAMIC LINK LIBRARY : BasicUniversalCppSupport Project Overview
========================================================================

AppWizard has created this BasicUniversalCppSupport DLL for you.

This file contains a summary of what you will find in each of the files that
make up your BasicUniversalCppSupport application.


BasicUniversalCppSupport.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

BasicUniversalCppSupport.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named BasicUniversalCppSupport.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
Programmers notes:
Copyright (c) 2003-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev

    This program is free software : you can redistribute itand /or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.If not, see < http://www.gnu.org/licenses/>.


stdAfx.h, targtver.h modified to target windows 7 and higher

CppUnitTest.hpp
    Wrapper for Microsoft's unit test header CppUnitTest.h (suppression of warnings raised by imported header)

error_context.hpp
    An error::context type (featuring context tagging on std::exception 'what').

file_logger.hpp, file_logger.cpp
    A simple logger implementation using a filesystem file.

gsl.hpp
    Wrapper for Microsoft's Guidlines Support Library header gsl.h (suppression of warnings raised by imported header)

log_helpers.hpp
    static methods to support logging 

logger.hpp
    Exposes logging options and logging macros to programmers

logger_factory.hpp
    A factory pattern for creating loggers

logger_interface.hpp
    Interface for all loggers
    
null_logger.hpp
    A concrete 'do nothing' logger

spimpl.hpp
    Templates support for smart pointer to implementation paradigm using rule of zero. 

system_error.hpp, system_error.cpp
    These files provide a service to fetch the system error text in the default locale.

utc_timestamp.hpp
    Timestamp support (used for logging purposes, and available for etc.)

utf8_assert.hpp
    This header is a utf8 wrapper for Microsoft's CppUnitTest Assert class. 

utf8_console.hpp, utf8_console.cpp 
    These files provide console codepage configuration (needed on windows). 
    
utf8_convert.hpp, utf8_convert.cpp 
    These files provide minimum conversions needed to support the utf8 anywhere 
    programming paradigm (on windows). 

utf8_guid.hpp
    This header supplies utf8 string conversions to and from windows GUID type.
    It is not platform independent, and should only be included in a localized
    way (in .cpp files where an encapsulated windows impl inner class needs it).
