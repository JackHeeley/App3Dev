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

#include <iostream>
#include <sstream>
#include <string>
#include <sstream>
#include <vector>

#include "error_context.hpp" 
#include "file_logger.hpp"
#include "gsl.hpp"
#include "logger.hpp"           
#include "logger_interface.hpp"
#include "logger_factory.hpp"
#include "null_logger.hpp"
#include "RAII_thread.hpp"
#include "spimpl.hpp"
#include "system_error.hpp"
#include "utf8_assert.hpp"
#include "utf8_convert.hpp"
#include "utf8_guid.hpp"
#include "utc_timestamp.hpp"

#include "UnitTestBasicUniversalCppSupport.hpp"

#endif // __STDAFX_H__