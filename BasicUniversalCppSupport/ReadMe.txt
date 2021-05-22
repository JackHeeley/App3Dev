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

==============================================================================
Motivation:

To provide the smallest set of support needed to allow a standardized approach
to cross-cutting issues affecting modern c++ applications; and to present
orthodox, efficient and robust character encoding, exception handling, 
unit-testing and logging features and strategies.

This dll has unit tests provided by another project in this solution.
==============================================================================

CppUnitTest.hpp
    Wrapper for Microsoft's unit test header CppUnitTest.h (suppression of warnings raised by imported header)

error_context.hpp
    An error::context type (featuring context tagging on std::exception 'what'). The context thrown when an
    unhandled error occurs is sufficient for a last-ditch exception handler to fully notify a user (or 
    tester) of what went wrong (often in terms that a user will undertand).

file_logger.hpp, file_logger.cpp
    A simple logger implementation using a filesystem file. Writes are synchronized for multi-threading.

gsl.hpp
    Wrapper for Microsoft's Guidlines Support Library header gsl/gsl (suppression of warnings raised by imported header)

log_helpers.hpp
    static methods to support logging. Logging is configurable at build time and degenerates to no-op/comment when disabled. 

logger.hpp
    Exposes logging options and logging macros to programmers. Logging can be configured differently for 
    release and debug builds. A build optional to allow dynamic re-configuring of logging at run time 
    is available here (with some performance penalty).

logger_factory.hpp
    A factory pattern for creating loggers. The active logger is accessed via a shared pointer, available to all runtime 
    components (main program and dlls). 

logger_interface.hpp
    Interface for all loggers
    
null_logger.hpp
    A 'do nothing' logger implementation (used if no active file_logger has been provided)

spimpl.hpp
    Templates support for "smart pointer to implementation" paradigm using rule of zero.
    The hpp file just wraps Andrey Upadyshev's spimpl.h. This is a key recommendation for
    encapsulation (e.g. of platform code), and is used extensively in App3Dev.

system_error.hpp, system_error.cpp
    These files provide a service to fetch the system error text in the default locale.

toolsver.h
    Provides an upgrade warning to users of earlier visual studio versions.

utc_timestamp.hpp
    Timestamp support (provided for logging purposes, and available to client code).

utf8_assert.hpp
    This header is a utf8 wrapper for Microsoft's CppUnitTest Assert class. 

utf8_console.hpp, utf8_console.cpp 
    These files provide console codepage configuration (needed on windows). 
    
utf8_convert.hpp, utf8_convert.cpp 
    These files provide minimum conversions needed to support the utf8 anywhere 
    programming paradigm (on windows). 

utf8_guid.hpp
    This header supplies utf8 string conversions to and from windows GUID type.
    It is not platform independent, and client code should be careful (localize) where 
    this header is included (e.g. in .cpp files where an encapsulated windows impl inner class needs it).

NOTE: A HISTORY OF CHANGES MATCHING CHECK-IN NUMBERS HAS BEEN MAINTAINED (in the SampleProgram project Readme.txt)