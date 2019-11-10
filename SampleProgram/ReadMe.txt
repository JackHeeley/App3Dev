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
45. Reviewed and revised warning supression for newest compiler

IN PROGRESS:

TODO:

1. Consider how to more conveniently share a single file_logger in a multi-dll program.

2. File logger entry layout could be improved.

3. LOGGER #defines are probably candidates for revision (change to a modern CPP way of working).

4. Replace GUID params with strings as a step towards support for PURE ANSI C++ SampleProgram)
	utf8::to_GUID() and utf8::from_GUID() are under development

