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

#include <vector>
#include <sstream>

// Additional headers this dll requires are here...
#include <error_context.hpp>
#include <file_logger.hpp>
#include <gsl.hpp>
#include <logger.hpp>
#include <spimpl.hpp>
#include <system_error.hpp>
#include <use_logger.hpp>
#include <utf8_convert.hpp>
#include <utc_timestamp.hpp>