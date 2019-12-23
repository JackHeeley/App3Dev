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

#include "abstract_logger.hpp"
#include "default_logger.hpp"
#include "file_logger.hpp"
#include "logger_factory.hpp"
#include "logger.hpp"

#include "error_context.hpp"
#include "gsl.hpp"

#include "spimpl.hpp"
#include "system_error.hpp"
#include "utf8_convert.hpp"
#include "utc_timestamp.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <sstream>
#include <vector>

#include "utf8_assert.hpp"
