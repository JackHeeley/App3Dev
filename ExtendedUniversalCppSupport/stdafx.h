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

#pragma warning(disable : 26429 26440 26427 26434 26447 26472 26473 26486 26496 26814)
#include <gsl.h>
#pragma warning(default : 26429 26440 26427 26434 26447 26472 26473 26486 26496 26814)

#include <vector>
#include <sstream>

// Additional headers this dll requires are here...
#include <error_context.hpp>
#include <file_logger.hpp>
#include <logger.hpp>
#include <spimpl.hpp>
#include <system_error.hpp>
#include <utf8_convert.hpp>
#include <utc_timestamp.hpp>