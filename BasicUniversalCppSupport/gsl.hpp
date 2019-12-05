//
// gsl.hpp : wrapper for gsl.h - used to suppress warnings in externally provided source
//
// Copyright (c) 2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
#pragma once
#define GSL_WARNINGS_SUPPRESSED 4996 26429 26440 26434 26472 26473 26486 26496 26814
#pragma warning(disable: GSL_WARNINGS_SUPPRESSED)
#include <gsl.h>
#pragma warning(default: GSL_WARNINGS_SUPPRESSED)
