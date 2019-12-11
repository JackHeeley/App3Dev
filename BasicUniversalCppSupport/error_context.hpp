//
// error_context.hpp : implements exception message support
//
// Copyright (c) 2017-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
#pragma once

#ifdef BASICUNIVERSALCPPSUPPORT_EXPORTS
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <exception>
#include <sstream>
#include <string>

#include "system_error.hpp"

///<summary>use the predefined ANSI/ISO C99 C preprocessor macros to inject locus and context details when constructing an error::context</summary>
#define error_context(_text) error::context(__FILE__, __LINE__, __func__, (_text))

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
      ///<param name='a_func'> use predefined ANSI/ISO C99 C preprocessor macro __func__ </param>
      ///<param name='a_what'> a short description of the exception.</param>
      BASICUNIVERSALCPPSUPPORT_API context(const char* a_path, int a_line, const char* a_func, const char* a_what) :
         _file((std::string(a_path).substr(std::string(a_path).find_last_of("/\\") + 1))),
         _line(a_line),
         _func(a_func),
         _reason(SystemError().get_error_text()),
         std::exception(a_what)
      {
         _full_what = decorate_what(::std::exception::what());
      };

      ///<summary> get full description of exception.</summary>
      ///<returns> a full description of what went wrong (including details of where in the code and any system error that is implicated).
      /// Note: return value is raw pointer to short lived memory that will be invalidated when the exception destructor is called.</returns>
      BASICUNIVERSALCPPSUPPORT_API const char* full_what() const noexcept
      {
         return _full_what.c_str();
      }

   private:
      ///<summary> the source file where the exception is thrown.</summary>
      const std::string _file;

      ///<summary> the source code line number where the exception is thrown.</summary>
      const int _line;

      ///<summary> the name of the enclosing function where the exception is thrown.</summary>
      const std::string _func;

      ///<summary> the underlying system error message (if any system error occurred).</summary>
      const std::string _reason;

      ///<summary> the full description of what went wrong.</summary>
      std::string _full_what;

      ///<summary> collate details relating to the exception.</summary>
      ///<param name='a_what'> the programmer's description of what went wrong.</param>
      ///<returns> the full description of what went wrong.</returns>
      std::string decorate_what(const char* a_what)
      {
         std::stringstream ss;
         ss << _file << ", L" << _line << " " << _func << "() : " << a_what << ". " << _reason;
         return ss.str();
      };

   };
}
