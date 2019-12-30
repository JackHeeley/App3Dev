//
// UnitTestRAII_thread.cpp : a utf8 everywhere component unit test 
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
using namespace std::chrono_literals;
using namespace utf8;

namespace UnitTestSampleProgram
{
   TEST_CLASS(UnitTestThreadRAII)
   {
   public:

#pragma warning(disable: 26440 26477 26497)
      TEST_CLASS_INITIALIZE(InitializeUnitTestRAII_thread) noexcept
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

      ///<summary>confirms the design without stressing it</summary>
      TEST_METHOD(TestRAII_thread)
      {
         try
         {
            {
               std::atomic<int> progress = 0;
               bool signal_done = false;

               auto show = [&progress, &signal_done]
               {
                  int percent = 0;
                  while (!signal_done && percent < 100)
                  {
                     std::this_thread::sleep_for(1s);
                     percent = progress;
                     std::cout << "\r" << "[" << std::string(percent / 5, '\xfe') << std::string(gsl::narrow_cast<size_t>(100 / 5 - percent / 5), ' ') << "]";
                     std::cout << percent << "%";
                     std::cout.flush();
                  }
                  std::cout << std::endl;
                  std::cout.flush();
               };

               RAII_thread r(std::thread(show), RAII_thread::DtorAction::join);

               while (progress < 100)
               {
                  std::this_thread::sleep_for(10ms);
                  progress++;
               }
               signal_done = true;
            }
         }
         catch (const std::exception & e)
         {
            utf8::Assert::Fail(e.what()); // something went wrong
         }
      }
    };
}