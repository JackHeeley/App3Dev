// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef __STDAFX_H__
#define __STDAFX_H__

// add check for tools limitations (clang support) with impact on build preferences
#include "toolsver.h"

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// Additional headers program requires are here...

#include "error_context.hpp"
#include "file_logger.hpp"
#include "gsl.hpp"
#include "logger_factory.hpp"
#include "logger_interface.hpp"
#include "logger.hpp"
#include "null_logger.hpp"
#include "spimpl.hpp"
#include "system_error.hpp"
#include "utc_timestamp.hpp"
#include "utf8_console.hpp"
#include "utf8_convert.hpp"

#include "cd_rom_device.hpp"
#include "device_discoverer.hpp"
#include "memory_mapped_file.hpp"
#include "RAII_cd_exclusive_access_lock.hpp"
#include "RAII_cd_physical_lock.hpp"

#include "progress_tracker.hpp"
#include "ripper.hpp"

#endif // __STDAFX_H__
