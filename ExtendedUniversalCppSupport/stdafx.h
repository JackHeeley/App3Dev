// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef __STDAFX_H__
#define __STDAFX_H__

// add check for tools limitations (clang support) with impact on build preferences
#include "toolsver.h"

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#define NOMINMAX
#include <windows.h>

#include <vector>
#include <sstream>

// Additional headers this dll requires are here...

#include "error_context.hpp"
#include "file_logger.hpp"
#include "gsl.hpp"
#include "logger.hpp"
#include "logger_interface.hpp"
#include "spimpl.hpp"
#include "system_error.hpp"
#include "utc_timestamp.hpp"
#include "utf8_convert.hpp"

#endif // __STDAFX_H__