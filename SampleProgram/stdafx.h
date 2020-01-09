// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// Additional headers program requires are here...

#include <error_context.hpp>
#include <file_logger.hpp>
#include <gsl.hpp>
#include <logger_factory.hpp>
#include <logger_interface.hpp>
#include <logger.hpp>
#include <null_logger.hpp>
#include <spimpl.hpp>
#include <system_error.hpp>
#include <utc_timestamp.hpp>
#include <utf8_convert.hpp>

#include <cd_rom_device.hpp>
#include <device_discoverer.hpp>
#include <memory_mapped_file.hpp>

#include "RAII_physical_lock.hpp"
#include "RAII_thread.hpp"
#include "ripper.hpp"

// TODO: We can't control the  test sequence, so there is a race to name the (single) logfile (as used by default logger macros).
// The Current logger_factory implementation doesn't even indicate who won, so unit tests can fail simply because they hard code 
// a wrong log_file_name assumption. The *temporary* fix is just to use/replicate the same log_file_name in ALL unit tests (over 
// ALL namespaces). But this can be improved!

namespace UnitTestBasicUniversalCppSupport
{
#pragma warning(disable: 26426)
   const std::string log_file_name("UnitTest.log");
#pragma warning(default: 26426)
};
