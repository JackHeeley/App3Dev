========================================================================
    DYNAMIC LINK LIBRARY : ExtendedUniversalCppSupport Project Overview
========================================================================

AppWizard has created this ExtendedUniversalCppSupport DLL for you.

This file contains a summary of what you will find in each of the files that
make up your ExtendedUniversalCppSupport application.


ExtendedUniversalCppSupport.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

ExtendedUniversalCppSupport.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named ExtendedUniversalCppSupport.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
Programmers notes:
Copyright (c) 2003-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev

StdAfx.h, targtver.h modified to target windows 7 and higher

device.hpp, device.cpp
    These files wrap the windows kernel mode device API with enough 
    functionality to perform read, write and i/o control operations
    on instances of specific device types.

device_discoverer.hpp, device_discoverer.cpp
    These files wrap the windows SetupDi API with enough functionality to
    name and open enumerated instances of permanent and removable devices, 
    discovered in real-time by their system device type identifier 
    (interface class guid), and zero based ordinal index.

memory_mapped_file.hpp, memory_mapped_file.cpp
    These files wrap the windows memory mapping functions with enough 
    functionality to supply a shared memory buffer backed by a file system
    (eg disk) file.

cd_rom_device.cpp, cd_rom_device.hpp
    These files represent a CDROM device with enough functionality to
    acquire (read) raw content, and perform some basic ioctls.