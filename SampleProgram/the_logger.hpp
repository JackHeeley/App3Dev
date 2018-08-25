//
// the_logger.hpp : implements the actual logger
//
// Copyright (c) 2018 Jack Heeley, all rights reserved
//
#pragma once
#include <file_logger.hpp>

///<summary> expose the (single) file logger in use - defined in main program main.cpp.</summary>
///<remarks> must be called "Logger" to match c-style #defines in support library's logger.hpp</remarks>
extern file_logger Logger;