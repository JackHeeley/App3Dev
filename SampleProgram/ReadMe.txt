========================================================================
    CONSOLE APPLICATION : AbstractDevice Project Overview
========================================================================

AppWizard has created this AbstractDevice application for you.

This file contains a summary of what you will find in each of the files that
make up your CheckDevice application.


AbstractDevice.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

AbstractDevice.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

Main.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named CheckDevice.pch and a precompiled types file named StdAfx.obj.

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


StdAfx.h is modified to target windows 7 and higher



History & todo list
===================

DONE:

 1. SystemError now the leading PIMPL reference (OK).
 2. Selected Rule of 5 methods PIMPL strategy: (class rule of 5 methods leverage private_impl rule of 5 methods).
 3. DeviceDiscovery reviewed to match 2.
 4. Device PIMPL shortcut: don't support copy/move. Applied by making copy move & compare explicitly deleted
 5. Test RAII style scoped TrayLocker for cdrom access unit tests has been added
 6. CdromDevice tray door locking responsibility now removed (externalized) from CdRomDevice class.
 7. CdRomDevice reviewed.
 8. Renamed DeviceDiscovery to DeviceDiscoverer
 9. Applied empty() where size()==0 was used.
10. Reworked main to do cdrom image ripping, and commented out the long TestCdromDeviceReadImage unit test
11. Addressed 32-bit sized buffers limit on x86 (reworked MemoryMappedFile & Main).
12. Simplified syntax in SystemError pimpl bridge
13. Revised CdRomDevice PIMPL to explicitly not support copy/move (as per Device).
14. Improved const correctness (removed casting const away) in SystemError. 
15. Explicitly deleted move copy and compare from CdromDevice. 
16. Supplied copy and move for MemoryMappedFile.
17. Converted earlier AbstractDevice.exe into two dll projects and one program project SampleProgram.exe.
18. Structured UnitTest projects to reflect the changed project architecture.
19. Fixed test sequencing vulnerability in TestCdromGetImageSize.
20. Tidy Main program comments and layout.
21. Added Tracing.hpp to give consistent handling of exception texts.
22. Finalized throw_std_exception.
23. Added basic logging support.
24. Sorted some test weakness (intermittent failures) showing on Debug build.
25. Imported spimpl.h as generic pimpl approach. Applied to SystemError and fixed (my) bugs.
26. Implemented generic spimpl approach across all classes.
27. Refining logging exceptions and fixing test failures.
28. Catching by reference (best practice to avoid slicing).
29. Added custom allocator and mmf_vector type alias- CdromDevice::getImage() polymorphs now include param type mmf_vector&.
30. Favoured mmf_vector copying in sample program Ripper (for 64 bit) with fallback to legacy raw pointer if 32 bit.
31. Moved exception to own header (and mitigated name conflict on macro _exception ).
32. Spimpled the file_logger.
33. Fixed debug build custom allocator problem.
33. Cleaned up mmf_vector implementation.
34. Obsoleted mmf_vector in favour of gsl:span.
35. Replaced c++98 style parameter frames (using raw pointers and buffer sizes) with gsl::span.
36. Addressed long standing encapsulation niggles (moved cdrom_device, and utf8_assert to the support libraries, split out private classes in main, reworked unit tests accordingly)
37. Warning improvements for VS15.9.6/7 applied - nullptr in place of NULL - catching exceptions as const - lifetime of spans - pragma disables on affected foreign includes
38. Bugfix: restore warnings to default in utc_timestamp.hpp
39. Clarity: renamed impl_ to pimpl in all the pimpl implementatioms
40. Update to VS 2019 (some warnings addressed)
41. Completed warning treatment so that VS2019 proj compiles clean
42. Correction to XML (use of & replaced with 'and')
43. Updated spimpl.hpp to latest version (1.2) with warning suppression extended and added inside the header
44. Fixed a couple of unit test issues.
45. Reviewed and revised warning supression for VS 16.3.8
46. Reviewed and reduced warning supression for VS 16.3.9
47. Added a progress indication to SampleProgram.
48. Cleaned up progress indication implementation.
49. Completed unique_ptr treatment for CdRomDevice/Device - explicit delete in place of redundant move/copy constructors and assignment operators in impls (points missed at step 13 earlier)
50. Provided a working utf8::to_guid function.
51. Reworked open portability issues and enabled ISO C++17 Standard (std:c++17) Sample Program project.
		Replaced (windows type) GUID from api's with an enum and a lookup directory.
		Moved utf8 guid conversions out of the common headers.
		Accepted certain C++17 deprecations (in gsl and utf8 conversion) by disabling warnings (for now).
		Αccepted argument that u8"" (and u8 aware editors) are no longer an obstacle to reuse these days, and ditched utf8 everywhere manifesto advice to encode in hex in source files.
		Adopted u8"" in main.cpp and elected to save all source documents with utf8 encoding. (It seems that the save option is not enacted by Visual Studio until/unless required).
52. Simplified progress thread and fixed hang after exceptions thrown from main thread (caused by waiting forever on the progress thread)
53. Fixed blocking issue with showing progress. When doing multiple reads we need to seek on the device (seek capability added to device, and used in cd_rom_device).
54. Reworked dllexports, warnings and include files. Nice(!) Upgraded compiler to version 16.4.1
55. Conformed with full recommendations of GPL 3.0 license regarding notices and comments.
55. Reworked main program pre-checking.

IN PROGRESS:

TODO:

1. Consider strategies for logging in a multi-dll program.

2. File logger entry layout could be improved.

3. LOGGER #defines are probably candidates for revision (change to a modern CPP way of working).

4. Some of the disabled warnings do indeed indicate deviations from best practice, and attention should be given.
