// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Headers for CppUnitTest
#include <CppUnitTest.hpp>

// TODO: reference additional headers your program requires here
#define NOMINMAX
#include <windows.h>

#include "error_context.hpp"
#include "file_logger.hpp"
#include "gsl.hpp"
#include "logger.hpp"

#include "spimpl.hpp"
#include "system_error.hpp"
#include "utf8_convert.hpp"
#include "utc_timestamp.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "utf8_assert.hpp"
