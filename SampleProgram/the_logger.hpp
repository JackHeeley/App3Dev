//
// the_logger.hpp : implements the actual logger
//
// Copyright (c) 2018-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
#pragma once
#include <file_logger.hpp>

///<summary> expose the (single) file logger in use - defined in main program main.cpp.</summary>
///<remarks> must be called "Logger" to match c-style #defines in support library's logger.hpp</remarks>
extern file_logger Logger;