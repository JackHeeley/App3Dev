//
// CppUnitTest.hpp :  wrapper for CppUnitTest.h - used to suppress warnings in externally provided source
//
// Copyright (c) 2019 Jack Heeley, all rights reserved
//
#pragma once
#define CPPUNITTEST_WARNINGS_SUPRESSED 26429 26432 26433 26440 26443 26455 26461 26466 26477 26485 26486 26490 26496 
#pragma warning(disable: CPPUNITTEST_WARNINGS_SUPRESSED)
#include <CppUnitTest.h>
#pragma warning(default: CPPUNITTEST_WARNINGS_SUPRESSED)