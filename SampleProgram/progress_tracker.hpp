//
// progress_tracker.hpp : support for showing progress bar on stdout.
//
// Copyright (c) 2020 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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
#pragma once

#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

class progress_tracker
{
private:
   std::atomic<int>& m_progress;

   static constexpr int stalled_limit = 300;
   int stalled_count;
   int previous_value;

   const bool is_finished() noexcept
   {
      return !(m_progress < 100);
   };

   const bool is_stalled() noexcept
   {
      if (m_progress == previous_value)
         stalled_count++;
      else
         stalled_count = 0;

      previous_value = m_progress;
      return !(stalled_count < stalled_limit);
   };

   std::string build_progress_bar(int percent)
   {
      std::stringstream bar;
      bar << "\r" << "[";
      for (int i = 0; i < 20; i++)
      {
         bar << ((i < percent / 5) ? u8"█" : " ");
      }
      bar << "]" << std::setw(3) << percent << "%";
      return bar.str();
   };

public:

   progress_tracker(std::atomic<int>& progress) noexcept :
      m_progress(progress),
      stalled_count(0),
      previous_value(progress)
   {
   }
   
   const bool operator()()
   {
      while (!is_finished() && !is_stalled())
      {
         using namespace std::chrono_literals;
         std::this_thread::sleep_for(100ms);
         std::cout << build_progress_bar(m_progress);
      }
      std::cout << build_progress_bar(m_progress) << std::endl;

      return is_finished();
   }
};

