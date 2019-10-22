// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Headers for CppUnitTest
#pragma warning(disable : 26429  26432 26433 26439 26440 26443 26447 26455 26461 26466 26477 26485 26486 26490 26495 26496)
#include "CppUnitTest.h"
#pragma warning(default : 26429  26432 26433 26439 26440 26443 26447 26455 26461 26466 26477 26485 26486 26490 26495 26496)

// TODO: reference additional headers your program requires here
#define NOMINMAX
#include <windows.h>

#pragma warning(disable : 26429 26440 26427 26434 26447 26472 26473 26486 26496 26814)
#include <gsl.h>
#pragma warning(default : 26429 26440 26427 26434 26447 26472 26473 26486 26496 26814)

#include <error_context.hpp>
#include <file_logger.hpp>
#include <logger.hpp>
#include <spimpl.hpp>
#include <system_error.hpp>
#include <utf8_convert.hpp>
#include <utc_timestamp.hpp>

#include <device.hpp>
#include <device_discoverer.hpp>
#include <memory_mapped_file.hpp>
#include <cd_rom_device.hpp>

#include <tray_door_lock.hpp>
#include <ripper.hpp>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>

#include "utf8_assert.hpp"
