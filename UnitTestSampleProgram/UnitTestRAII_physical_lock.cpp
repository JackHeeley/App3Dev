//
// UnitTestRAII_physical_lock.cpp : a utf8 everywhere component unit test 
//
// Copyright (c) 2019-2020 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
//    This program is free software : you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.If not, see < http://www.gnu.org/licenses/ >.
//
#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::chrono_literals;
using namespace utf8;

namespace UnitTestSampleProgram
{
   TEST_CLASS(UnitTestRAII_physical_lock)
   {
   public:

#pragma warning(disable: 26440 26497)
      TEST_CLASS_INITIALIZE(InitializeUnitTestRAII_physical_lock) noexcept // NOLINT(clang-diagnostic-missing-braces)
#pragma warning(default: 26440 26497)
      {
         try
         {
            CREATE_LOGGER(logger_factory::logger_type::file_logger, log_file_name, DEFAULT_LOG_FILTER);
         }
         catch (...)
         {
            LOG_ERROR("Couldn't create logger.");       // No logger? This will emit on std::cerr
         }
      }

      TEST_METHOD(TestRAII_physical_lock)
      {
         // check test preconditions (at least one physical cdrom needed)
         DeviceDiscoverer cdrom_interface(DeviceTypeDirectory::DeviceType::CDROM_DEVICES);
         utf8::Assert::IsFalse(cdrom_interface.device_path_map.get().empty(), "no system cdrom devices were discovered");

         //prepare test 
         CdromDevice m_cdr(cdrom_interface.device_path_map.get()[0]);

         try
         {
            //perform operation under test
            RAII_physical_lock lock(m_cdr);
            
            //check results phase I ...
            utf8::Assert::IsTrue(m_cdr.get_locked(), "drive door should still be locked"); 

            try
            {
               m_cdr.eject();
               utf8::Assert::Fail("eject succeeded even though the door was locked"); 
            }
            catch (...)
            {
               utf8::Assert::IsTrue(m_cdr.get_locked(), "drive door should still be locked");
            }

            utf8::Assert::IsTrue(m_cdr.get_locked(), "drive door should still be locked"); 
         }
         catch (const std::exception & e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }

         //check results phase II ...
         utf8::Assert::IsTrue(!m_cdr.get_locked(), "drive door should now be unlocked"); // something went wrong
      }
   };
}