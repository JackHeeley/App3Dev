// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#define NOMINMAX
#include <windows.h>

#include <fstream>
#include <sstream>
#include <string>
#include <time.h>
#include <atomic>
#include <codecvt>
#include <stdexcept>
#include <stdint.h>

// Additional headers dll requires are here...
#include "CppUnitTest.hpp"       
#include "error_context.hpp"
#include "file_logger.hpp"
#include "gsl.hpp"
#include "logger.hpp"
#include "logger_factory.hpp"
#include "logger_interface.hpp"
#include "null_logger.hpp"
#include "spimpl.hpp"
#include "system_error.hpp"
#include "utc_timestamp.hpp"
#include "utf8_assert.hpp"       
#include "utf8_console.hpp"
#include "utf8_convert.hpp"
#include "utf8_guid.hpp"         

