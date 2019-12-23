//
// thread_raii.hpp : implements helper class
//
//    Allows an RAII thread to abnormally end, without a program abort arising from the ensuing improper thread state.
//
// Copyright (c) 2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
#pragma once
#include <thread>

///<summary> RAII object used to make std::threads unjoinable on all paths.</summary>
///<remarks> based upon Meyers, Scott. Effective Modern C++: 42 Specific Ways to Improve Your Use of C++11 and C++14. O'Reilly Media. Kindle Edition.</remarks>
class thread_RAII
{
public:
   ///<summary> options available at destruct time</summary>
   enum class DtorAction { join, detach };

private:
   DtorAction action;
   std::thread t;

public:

   ///<summary> Constructs an RAII object that allows threads to fail without impacting parent</summary>
   ///<param name='t'> the std::thread</param>
   ///<param name='a'> the action to take during the RAII object destructor execution</param>
   thread_RAII(std::thread&& t, DtorAction a) noexcept :
      action(a),
      t(std::move(t))
   {
   }

   ///<summary> explicit delete of copy constuctor (rule of 5)</summary>
   thread_RAII(const thread_RAII& other) = delete;

   ///<summary> explicit (default) move constuctor (rule of 5)</summary>
   thread_RAII(thread_RAII&& other) = default;

   ///<summary> explicit delete of copy assignment operator (rule of 5)</summary>
   thread_RAII& thread_RAII::operator=(thread_RAII& other) = delete;
   
   ///<summary> explicit (default) move assignment operator (rule of 5)</summary>
   thread_RAII& thread_RAII::operator=(thread_RAII&& other) = default;

   ///<summary> explicit Dtor</summary>
   ~thread_RAII() noexcept
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
         LOG_WARNING("~thread_RAII had an issue!");
      }
   }

   ///<summary> get the contained thread</summary>
   ///<returns> the thread passed to thread_RAII at construction time</returns>
   std::thread& get() noexcept
   {
      return t;
   }
};
