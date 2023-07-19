//
// RAII_thread.hpp : implements helper class *** DEPRECATED (use std::async) *** 
//
//    Allows a RAII thread to abnormally end, without a program abort arising from the ensuing improper thread state.
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
#pragma once
#include <thread>
#include "logger.hpp"

///<summary> RAII object used to make std::threads unjoinable on all paths.</summary>
///<remarks> based upon Meyers, Scott. Effective Modern C++: 42 Specific Ways to Improve Your Use of C++11 and C++14. O'Reilly Media. Kindle Edition.</remarks>
class RAII_thread
{
public:
   ///<summary> options available at destruct time</summary>
   enum class DtorAction { join, detach };

private:
   DtorAction action;
   std::thread t;

public:

   ///<summary> Constructs a RAII object that allows threads to fail without impacting parent</summary>
   ///<param name='t'> the std::thread</param>
   ///<param name='a'> the action to take during the RAII object destructor execution</param>
   RAII_thread(std::thread&& t, DtorAction a) noexcept :
      action(a),
      t(std::move(t))
   {
   }

   ///<summary> explicit delete of copy constuctor (rule of 5)</summary>
   RAII_thread(const RAII_thread& other) = delete;

   ///<summary> explicit (default) move constuctor (rule of 5)</summary>
   RAII_thread(RAII_thread&& other) = default;

   ///<summary> explicit delete of copy assignment operator (rule of 5)</summary>
   RAII_thread& operator=(RAII_thread& other) = delete;

   ///<summary> explicit (default) move assignment operator (rule of 5)</summary>
   RAII_thread& operator=(RAII_thread&& other) = default;

   ///<summary> explicit Dtor</summary>
   ~RAII_thread() noexcept
   {
      try
      {
         if (t.joinable())
         {
            if (action == DtorAction::join)
            {
               t.join();
            }
            else
            {
               t.detach();
            }
         }
      }
      catch (...)
      {
         LOG_WARNING("~RAII_thread had an issue!");
      }
   }

   ///<summary> get the contained thread</summary>
   ///<returns> the thread passed to RAII_thread at construction time</returns>
   std::thread& get() noexcept
   {
      return t;
   }
};
