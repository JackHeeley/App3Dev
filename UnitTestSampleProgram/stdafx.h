// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef __STDAFX_H__
#define __STDAFX_H__

// add check for tools limitations (clang support) with impact on build preferences
#include "toolsver.h"

#include "targetver.h"

// Headers for CppUnitTest
#include <CppUnitTest.hpp>

// TODO: reference additional headers your program requires here
#define NOMINMAX
#include <windows.h>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>

#include <gsl.hpp>
#include <spimpl.hpp>
#include <logger.hpp>

#include <error_context.hpp>
#include <file_logger.hpp>
#include <logger_interface.hpp>
#include <system_error.hpp>
#include <utf8_convert.hpp>
#include <utc_timestamp.hpp>

#include <device.hpp>
#include <device_discoverer.hpp>
#include <memory_mapped_file.hpp>
#include <cd_rom_device.hpp>

#include <RAII_cd_physical_lock.hpp>
#include <RAII_cd_exclusive_access_lock.hpp>
#include <ripper.hpp>

#include <utf8_assert.hpp>

#include "UnitTestSampleProgram.hpp"

#endif // __STDAFX_H__

