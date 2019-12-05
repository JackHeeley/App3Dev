//
// spimple.hpp : wrapper for spimpl.h - used to suppress warnings in externally provided source
//
// Copyright (c) 2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
#pragma once
#define SPIMPLE_WARNINGS_SUPPRESSED 26401 26402 26409 26432 26434 26440 26456 26486 26487 26489
#pragma warning(disable: SPIMPLE_WARNINGS_SUPPRESSED)
#include "spimpl.h"
#pragma warning(default: SPIMPLE_WARNINGS_SUPPRESSED)
