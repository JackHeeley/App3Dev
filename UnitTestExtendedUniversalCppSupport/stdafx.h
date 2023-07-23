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

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "error_context.hpp"
#include "file_logger.hpp"
#include "gsl.hpp"
#include "logger.hpp"
#include "logger_interface.hpp"
#include "spimpl.hpp"
#include "system_error.hpp"
#include "utf8_assert.hpp"
#include "utf8_convert.hpp"
#include "utf8_guid.hpp"
#include "utc_timestamp.hpp"

#include "cd_rom_device.hpp"
#include "device.hpp"
#include "device_discoverer.hpp"
#include "device_type_directory.hpp"
#include "memory_mapped_file.hpp"
#include "RAII_cd_exclusive_access_lock.hpp"
#include "RAII_cd_physical_lock.hpp"

#include "UnitTestExtendedUniversalCppSupport.hpp"

#endif // __STDAFX_H__
