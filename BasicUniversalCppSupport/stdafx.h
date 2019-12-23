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

// Additional headers dll requires are here... (See note 1)
#include "gsl.hpp"
#include "spimpl.hpp"

#include "abstract_logger.hpp"
#include "default_logger.hpp"
#include "file_logger.hpp"
#include "logger_factory.hpp"
#include "logger.hpp"

#include "error_context.hpp"
#include "system_error.hpp"

#include "utf8_convert.hpp"
#include "utf8_guid.hpp"         // See note 2
#include "utc_timestamp.hpp"

#include "CppUnitTest.hpp"       // See note 2
#include "utf8_assert.hpp"       // See note 2

// NOTE 1 : Objective is to manage time to compile. So here we touch everything needed by the dll and rely on #pragma once.  
// NOTE 2 : These files are specific to windows (and/or unit test framework). Not appropriate for code compiled with /std:c++17
// NOTE 3 : Clients of the DLL should cherry-pick what they need. They should NOT blindly duplicate this content