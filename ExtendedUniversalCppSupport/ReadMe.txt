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

==============================================================================
Motivation:

To provide an example of encapsulating domain knowledge (in a dll). Here we 
provide facilities to read the content of an optical disk and write it efficiently
to a filesystem (disk file). Encapsulation isolates the client code from platform 
dependencies, and the client programmer from the need to have or develop expertise 
in the specific platform api's (in this case win32 device i/o and setupDi) as
used here.

Contains multiple examples of using the pimpl paradigm to isolate and localize
code that is unable to conform to our adopted/preferred coding style rules. In 
this case we are obliged to use legacy c-style api's, which don't follow current 
gsl guidance, expose inconsistent ways to indicate success, and use wide character 
unicode strings as parameters and return codes.

We use pimpl to avoid propagating these issues into our wider client code.

The code also demonstrates that cross-cutting solutions for character encoding, 
logging and structured exception handling are available to support dll's.

This dll has unit tests provided by another project in this solution.
==============================================================================

cd_rom_device.hpp, cd_rom_device.cpp
    These files represent a CDROM device with enough functionality to
    acquire (read) raw content, and perform some basic ioctls.
    
device.hpp, device.cpp
    These files wrap the windows kernel mode device API with enough 
    functionality to perform read, write and i/o control operations
    on instances of specific device types.

device_discoverer.hpp, device_discoverer.cpp
    These files wrap the windows SetupDi API with enough functionality to
    name and open enumerated instances of permanent and removable devices, 
    discovered in real-time by their system device type identifier 
    (interface class guid), and zero based ordinal index.

device_type_directory.hpp, device_type_directory.hpp
    These files provide a catalog of system device types. The catalog
    provides a generic (system agnostic) way to reference an arbitrary set 
    of "supported" system device types (including cdrom).

memory_mapped_file.hpp, memory_mapped_file.cpp
    These files wrap the windows memory mapping functions with enough 
    functionality to supply a shared memory buffer backed by a file system
    (eg disk) file.