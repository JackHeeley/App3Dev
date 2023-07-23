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
#ifndef __PROGRESS_TRACKER_HPP__
#define __PROGRESS_TRACKER_HPP__

#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>

///<summary>a class to report progress (of some arbitrary process) on stdout</summary>
class progress_tracker
{
private:

   ///<summary>reference to a shared (usually static) percentage progress variable maintaned by another thread.</summary>
   std::atomic<int>& m_progress;

   ///<summary>maximum number of times (in a row) to allow m_progress to remain unchanged.</summary>
   static constexpr int stalled_limit = 300;

   ///<summary>actual number of times we have found that m_progress has not changed.</summary>
   int stalled_count;

   ///<summary>actual value of m_progress the last time we checked it.</summary>
   int previous_value;

   ///<summary> test if progress has reached 100 percent</summary>
   ///<returns> false if progress is less than 100, otherwise true.</returns>
   const bool is_finished() noexcept
   {
      return !(m_progress < 100);
   };

   ///<summary> test if progress has stalled.</summary>
   ///<returns> false if progress hasn't improved recently (true if it hasn't improved for a predefined number of tests).</returns>
   const bool is_stalled() noexcept
   {
      if (m_progress <= previous_value)
         stalled_count++;
      else
         stalled_count = 0;

      previous_value = m_progress;
      return !(stalled_count < stalled_limit);
   };

   ///<summary> build a progress bar as a utf8 encoded string.</summary>
   ///<param name='percent'> current progress expressed as an integer percentage.</param>
   ///<returns>a std::string representing a (visually growing) progress bar, appended with current percentage as a decimal.</returns>
   std::string build_progress_bar(int percent)
   {
      std::stringstream bar;
      bar << "\r" << "[";
      for (int i = 0; i < 20; i++)
      {
         bar << ((i < percent / 5) ? U8("█") : " ");
      }
      bar << "]" << std::setw(3) << percent << "%";
      return bar.str();
   };

public:
   ///<summary> construct the progress tracker.</summary>
   ///<param name='a_progress'> a reference to the (usually static) std::atomic&lt;int&gt; as
   /// used by another thread to track progress of some ongoing process.</param>
   progress_tracker(std::atomic<int>& a_progress) noexcept :
      m_progress(a_progress),
      stalled_count(0),
      previous_value(a_progress)
   {
      Expects(0 <= a_progress && a_progress <= 100);
   }

   ///<summary> functor displays progress to stdout.</summary>
   ///<returns> true if progress reaches 100 percent, and false if the process stalls.</returns>
   const bool operator()()
   {
      while (!is_finished() && !is_stalled())
      {
         using namespace std::chrono_literals;
         std::this_thread::sleep_for(100ms);
         std::cout << build_progress_bar(m_progress) << std::flush;
      }
      std::cout << build_progress_bar(m_progress) << std::endl;

      return is_finished();
   }
};

#endif __PROGRESS_TRACKER_HPP__
