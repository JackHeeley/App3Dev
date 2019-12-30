//
// UnitTestMemoryMappedFile.cpp : a utf8 everywhere component unit test 
//
// Copyright (c) 2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
//    This program is free software : you can redistribute itand /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.If not, see < http://www.gnu.org/licenses/>.
//
#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace utf8;

namespace UnitTestExtendedUniversalCppSupport
{

   ///<remarks> see: https://stackoverflow.com/questions/14807192/can-i-use-an-stdvector-as-a-facade-for-a-pre-allocated-raw-array </remarks>
   TEST_CLASS(UnitTestMemoryMappedFile)
   {
   public:

#pragma warning(disable: 26440 26477 26497)
      TEST_CLASS_INITIALIZE(InitializeUnitTestMemoryMappedFile) noexcept
#pragma warning(default: 26440 26477 26497)
      {
         try
         {
            CREATE_LOGGER(logger_factory::type::file_logger, log_file_name, DEFAULT_LOG_FILTER);
         }
         catch (...)
         {
            LOG_ERROR("Couldn't create logger.");       // No logger? This will emit on std::cerr
         }
      }

      TEST_METHOD(TestMemoryMappedFileConstructor)
      {
         try
         {
            // prepare for test...
            const std::string file_path("test.iso");
            const std::string buffer_name("test_buffer");
            constexpr uint64_t buffer_size = 1024;

            // perform the operation under test (construct a memory mapped file with a constructor)...
            MemoryMappedFile mmf(file_path, buffer_name, buffer_size);

            // test succeeds if construction doesn't throw and...
            utf8::Assert::AreEqual(mmf.get_buffer_name(), buffer_name, "buffer_name retrieved doesn't match buffer_name supplied");
            utf8::Assert::AreEqual(mmf.get_file_path(), file_path, "file_path retrieved doesn't match file_path supplied");
            utf8::Assert::AreEqual(mmf.get_buffer_size(), buffer_size, "buffer_size retrieved doesn't match buffer_size supplied");
            try
            {
               const auto buffer = mmf.get_span();
            }
            catch (...)
            {
               const SystemError lastError;
               const int errorCode = lastError.get_error_code();
               const std::string errorText = lastError.get_error_text();
               std::stringstream ss;  
               ss << "get_buffer_address()  failed (" << errorCode << ") " << errorText;
               const std::string failString = ss.str();
               utf8::Assert::Fail(failString.c_str());
            }
         }
         catch (std::exception e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }      
      
      TEST_METHOD(TestMemoryMappedFileGetBuffer)
      {
         try
         {
            // prepare for test...
            const std::string file_path("test.iso");
            const std::string buffer_name("test_buffer");
            constexpr uint64_t buffer_size = 1024;
            // perform the operation under test (construct a memory mapped file with a constructor)...
            MemoryMappedFile mmf(file_path, buffer_name, buffer_size);

            // test succeeds if construction doesn't throw and...
            utf8::Assert::AreEqual(mmf.get_buffer_name(), buffer_name, "buffer_name retrieved doesn't match buffer_name supplied");
            utf8::Assert::AreEqual(mmf.get_file_path(), file_path, "file_path retrieved doesn't match file_path supplied");
            utf8::Assert::AreEqual(mmf.get_buffer_size(), buffer_size, "buffer_size retrieved doesn't match buffer_size supplied");
            try
            {
               const auto span = mmf.get_span();
            }
            catch (...)
            {
               const SystemError lastError;
               const int errorCode = lastError.get_error_code();
               const std::string errorText = lastError.get_error_text();
               std::stringstream ss;
               ss << "get_buffer()  failed (" << errorCode << ") " << errorText;
               const std::string failString = ss.str();
               utf8::Assert::Fail(failString.c_str());
            }
         }
         catch (std::exception e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }

   };
}