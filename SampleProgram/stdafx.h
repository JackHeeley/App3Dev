// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// Additional headers program requires are here...
#define NOMINMAX
#include <windows.h> // TODO: almost ready to remove this and set project for pure ANSI C++ 14

#include <error_context.hpp>
#include <file_logger.hpp>
#include <gsl.hpp>
#include <logger.hpp>
#include <spimpl.hpp>
#include <system_error.hpp>
#include <utf8_convert.hpp>
#include <utc_timestamp.hpp>

#include <device.hpp>
#include <device_discoverer.hpp>
#include <memory_mapped_file.hpp>
#include <cd_rom_device.hpp>

#include "ripper.hpp"
#include "the_logger.hpp"
#include "tray_door_lock.hpp"
