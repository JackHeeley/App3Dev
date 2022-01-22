========================================================================
    CONSOLE APPLICATION : SampleProgram Project Overview
========================================================================

AppWizard has created this SampleProgram application for you.

This file contains a summary of what you will find in each of the files that
make up your CheckDevice application.


SampleProgram.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

SampleProgram.vcxproj.filters
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
Copyright (c) 2003-2020 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev

    This program is free software : you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.If not, see < http://www.gnu.org/licenses/ >.

THIS PROJECT IS BUILT WITH ISO C++17 /std:c++17 compiler option

StdAfx.h, targtver.h modified to target windows 7 and higher

==============================================================================
Motivation:

To provide working examples of recommended practices in modern C++ circa 2020.

This program has unit tests provided by another project in this solution.
==============================================================================

main.cpp
    This file contains the entrypoint and main program. It manages the
    rip (data copy) operation, logging, progress reporting, exception handling,
    and program interruption.

    main() demonstrates robust approaches to: character encoding, a flexible and
    efficient logging idiom, structured exception handling, signal handling, and 
    multi-threading. These are recommended as being readable examples of effective,
    simple and reusable ways to apply modern c++ to arbitrary assignments. The 
    facilities used to demonstrate these approaches are provided by the reusable
    "basic support library" (dll). 

progress_tracker.hpp
    Provides mechanisms to display a simple progress bar on the console.
    The functor is designed to run as a task launched by std::async. This is
    multi-threaded, shows synchronizing of stateful data (progress). It could be
    modified to demonstrate that logging is thread-safe, and that exception throwing 
    within tasks, is catchable by the thread that launched the task.

ripper.hpp
    Assembles the various components provided in the "extended support library" which 
    is an example of domain specific specific encapsulation (win32 kernel api's) in a dll. 
    ripper provides a functor that efficiently copies the contents of an optical disk 
    into a system file, using the extended support provided by the dll.

toolsver.h
    Provides an upgrade warning to users of earlier visual studio versions.
       
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
56. Reworked main program pre-checking.
57. Revised terminology in output messages, and tightened up the progress reporting design by adding gsl Ensures & Expects.
58. Reworked the utf8 conversion, since std::wstring_convert is deprecated at C++17.
59. Added signal handling to unlock the optical disk drive (needed when user initiates program abort).
60. Reworked logger strategy for logging in a multi-dll program.
        Factory now allows main to build a singleton file logger, that can be used anywhere in the dll stack.
        LOGGER #defines revised to a modern c++17 style (with lambdas).
61. Fixed compile time disabling of logging (so that it too builds without errors or warnings).
62. Reworked BasicUniversalCppSupport (header cleanup).
63. Reviewed ExtendedUniversalCppSupport SampleProgram. Made minor changes (comments, text strings etc.).
64. Reviewed memory_mapped_file. Now has explicit unmapview, and sets last modified time on file.
65. Improved logging (revised 61) to avoid spurious warnings (e.g. unreferenced parameters when logging is disabled at compile time).
66. Changed cdrom lock/unlock feature not to throw (noexcept). Now any problems are logged and ignored.
67. Addressed unit tests (logger tests broken).
68. Fixed a residual thread issue in the file_logger.
69. Fixed logger #define glitch that was raising a compiler warning with some parameters.
70. Rationalized the progress parameter passing (reworked Ripper).
71. Extending tests, and attending to unit test issues. Made one correction to dll exports in devicetypedirectory.hpp a static member
    was invisible to unit test dlls. Intent is to confirm basic logging initialization supports use of logging in unit tests (reliably)
    so that we have a start point for test-driven logging improvements.
72. Reworked logging  and log testing, in readiness for known todo's. Marked slow unit tests with TEST_IGNORE() attribute. 
73. Significant review and clean-up (readability). No significant functional changes. Some unit test improvements.
74. Logging optimizations (untidy but base functionality works & new and revised logging units test pass).
75. Logging optimizations now acceptably tidy. Unit test device ioctl has been reworked to retry device not ready (occurs in some paths).
76. Refactored logging to a good level of readability. Reworked some comments and warnings.
77. Comprehensive review of comments.
78. Replaced deprecated nuget gsl package with ms-gsl 3.0.1 (copied headers to solution).
79. Fixed broken check-in (ms-gsl 3.0.1 headers were not added properly to solution).
80. Fixed error handling in MemoryMappedFile (impacting copy constructor) & improved unit test
81. Fixed failure to truncate size on disk of large memory mapped files when committing a subsequent, but smaller image.
82. Robustness. Potential small leaks avoided in exception paths by closing handles in ~MemoryMappedFile().
83. Applied passing by const reference to function input parameters (performance opportunities that we missed earlier).
    Small changes in progress_tracker to improve robustness (check expectation that progress runs forward in range from 0 to 100).
    Fixed utf8::to_guid() and removed disable warnings no longer triggered (was probably a const promise violation, just fixed).
    Improved consistency and style of string concatenations used in logging.
84. Re-checked and revised all disabled warnings for compiler version 16.8.2
85. Fixed implementation of the template ToString specialization to allow _GUID parameters in Asserts 
    (now GUID's are correctly displayed in Assert outputs (e.g. as shown in the Test Explorer summary pane)
86. Revisited some comments and checked/corrected disabled warnings for clean build with the compile time disable debug option selected 
87. Upgraded ms-gsl 3.0.1 to ms-gsl 3.1.0 (copied new headers into solution) and rebuilt with updated compiler version 16.8.3
88. Removed word "using" from code comments and strings so that keyword can be searched for.
    Removed two instances of naked "using namespace..." in headers at global scope (relocated them to a locally scoped inner block)
89. Prepared code to build clean with clang-tidy (using compiler Version 16.9.0 Preview 2.0) You can use the preview and enable clang-tidy
    using the project properties code analysis setting. Because it is slow consider keeping it disabled in debug builds 
    (optimize core build cycle).
90. Added toolsver.h to all projects so that clang-tidy limitations and workround are explained to affected parties (a soon to be released
    compiler update will fix these but some developers will still be running earlier versions well into the future). Reconfigured projects 
    so that code analysis is ONLY APPLIED to the premium x64 release build. This is a compromise because of the time needed to do (full and 
    deep static analysis. The core development/debug pdca cycle efficiency should be respected.
91. Additional tray door lock tests added. They do not yet stress or replicate the issue seen - which seems to be that the device open
    as implemented - does not apply security controls as we anticipated. Opening the device with default security in shared mode results 
    in the driver scoping its block-the-unlock response to requests originating from the same local process.
92. CdRom device tray door lock feature was found to be been broken and had been fixed. Invoking the unit tests could break a simultaneously 
    running SampleProgram.exe. Tests could succeed in ejecting the current media while door is theoretically "locked" and while the cd is 
    being read by another program. Unit tests on lock feature were passing. Issue in cdRomDevice fixed. Also needed to claim exclusive access
    when tray door is locked. Unit tests have been extended. 
93. Added RAII_exclusive_access_lock.hpp and reworked / simplified exclusive access and tray door locking to remove redundant responsibilities.
94. Cleaned up RAII_cd... modularity inaccuracies (removing duplicate declarations in unit tests etc.)
95. Removed redundant class name qualifiers (used in some method declarations). These become illegal at /std:c++20 but were missed by the static analysers.
96. Added U8("") workround to utf8_convert.hpp. This addresses char8_t backward incompatibilities affecting utf8 string literals. 
    Code base is now ready for /std:C++20.
97. Reconsidered, and disabled clang-tidy in project settings for best out-of-box experience with the current 16.8.x compiler.
    Code base is now ready for clang-tidy.
98. Updated to 16.9.2 (now available) and restored clang enabled settings for x64 release builds. Builds clean with /std:c++latest.
99. Reworked exception handling for media present checks. When probing, it no longer consumes throws of 'unexpected' errors. 
    For example a CD inserted upside down will now trigger a report of an I/O error (instead of just asking to insert a disk).
100.Fixed possible unintended copy in two (disabled) unit tests - warning that auto can't deduce references.
101.Fixed two initialize variable warnings and removed unnecessary disable warning pragma.
102.Reviewed and trimmed disabled warnings and NOLINT comments for compiler version 16.9.4   
103.Comment correcting.
104.Updated forgotten entry in change history. 
105.Removed linker warning caused by /LTCG:incremental (release builds)
106.Updates to projects for VS2022 (4 bit) 17.0.5 including platform toolset 142->143 supression of unit test warnings and fix to unit test TestDeviceIoCtl
107.Added utf8::count_codepoints and unit test.
108.Made utf8::count_codepoints static.

TODO: @106 (vs22) Began to notice new issues with incremental builds (now need to rebuild all to clear some link errors)