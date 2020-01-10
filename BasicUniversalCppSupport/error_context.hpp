//
// error_context.hpp : implements exception message support
//
// Copyright (c) 2017-2020 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
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

#ifdef BASICUNIVERSALCPPSUPPORT_EXPORTS
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <exception>
#include <sstream>

#include "logger.hpp"
#include "system_error.hpp"

///<summary>use the predefined ANSI/ISO C99 C preprocessor macros to inject locus and context details when constructing an error::context</summary>
#define error_context(_text) error::context(__FILE__, __LINE__, __FUNCTION__, (_text))

namespace error
{
   ///<summary> a "preferred" exception type with added locus/context details.</summary>
   ///<remarks> the macro 'error_context(_text)' will construct one of these for you.</remarks>
   class context : public std::exception
   {
   public:
      ///<summary> construct an error::context (an exception type with added locus and context details).</summary>
      ///<remarks> macro 'error_context(_text)' will construct one in line.</remarks>
      ///<param name='a_path'> use predefined ANSI/ISO C99 C preprocessor macro __SOURCE__ </param>
      ///<param name='a_line'> use predefined ANSI/ISO C99 C preprocessor macro __LINE__ </param>
      ///<param name='a_func'> use predefined ANSI/ISO C99 C preprocessor macro __FUNCTION__ </param>
      ///<param name='a_what'> a short description of the exception.</param>
      BASICUNIVERSALCPPSUPPORT_API context(const char* a_path, int a_line, const char* a_func, const char* a_what) :
         m_file((std::string(a_path).substr(std::string(a_path).find_last_of("/\\") + 1))),
         m_line(a_line),
         m_func(a_func),
         m_reason(SystemError().get_error_text()),
         std::exception(a_what)
      {
         m_full_what = logging::decorate_error_context(std::string(m_file), m_line, std::string(m_func), std::exception::what(), m_reason);
      };

      ///<summary> get full description of exception.</summary>
      ///<returns> a full description of what went wrong (including details of where in the code and any system error that is implicated).
      /// Note: return value is raw pointer to short lived memory that will be invalidated when the exception destructor is called.</returns>
      BASICUNIVERSALCPPSUPPORT_API const char* full_what() const noexcept
      {
         return m_full_what.c_str();
      }

   private:
      ///<summary> the source file where the exception is thrown.</summary>
      const std::string m_file;

      ///<summary> the source code line number where the exception is thrown.</summary>
      const int m_line;

      ///<summary> the name of the enclosing function where the exception is thrown.</summary>
      const std::string m_func;

      ///<summary> the underlying system error message (if any system error occurred).</summary>
      const std::string m_reason;

      ///<summary> the full description of what went wrong.</summary>
      std::string m_full_what;
   };
}
