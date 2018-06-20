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

BasicUniversalCppSupport.cpp
    This is the main DLL source file.

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

StdAfx.h, targtver.h modified to target windows 7 and higher

Logger.hpp
    Abstract logging support

error_context.hpp
    An error::context type (featuring context tagging on std::exception 'what').

FileLogger.hpp, FileLogger.cpp
    A simple logger implementation using a filesystem file.

spimpl.hpp
    Templates support for smart pointer to implementation paradigm using rule of zero. 

SystemError.hpp, SystemError.cpp
    These files provide a service to fetch the system error text in the 
    default locale.

utc_timestamp.hpp
    Timestamp support (used for logging purposes, and available for etc.)

utf8_convert.hpp
    This header includes minimum conversions needed to support the 
    utf8 anywhere programming paradigm (on windows). 
