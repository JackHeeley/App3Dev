//
// utf8_assert.h : assert in utf8 everywhere style
//
// Copyright (c) 2015-2017 Jack Heeley, Fox-IT
// 
#pragma once

#ifdef BASICUNIVERSALCPPSUPPORT_EXPORTS
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <Windows.h>
#include <winnt.h>

#include "utf8_convert.hpp"
#include "spimpl.hpp"

namespace utf8
{
   struct __LineInfo;

   class BASICUNIVERSALCPPSUPPORT_API Assert
   {

   public:

      ///<summary> Verify that two objects are equal.</summary>
      ///<remarks> Template use necessarily breaks the desired encapsulation of operating system specifics. :-(</remarks>
      template<typename T> static void Assert::AreEqual(const T& expected, const T& actual, const char* message = NULL, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual
            (  expected, 
               actual, 
               message ? utf8::convert::to_utf16(message).c_str() : NULL, 
               reinterpret_cast<const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pLineInfo)
            );
      };

      ///<summary> double equality comparison.</summary>
      static void AreEqual(double expected, double actual, double tolerance, const char* message = NULL, const __LineInfo* pLineInfo = NULL);

      ///<summary> float equality comparison.</summary>
      static void AreEqual(float expected, float actual, float tolerance, const char* message = NULL, const __LineInfo* pLineInfo = NULL);

      /////<summary> char* string equality comparison.</summary>
      //static void AreEqual(const char* expected, const char* actual, const char* message, const __LineInfo* pLineInfo = NULL);

      ///<summary> char* string equality comparison.</summary>
      static void AreEqual(const char* expected, const char* actual, bool ignoreCase = false, const char* message = NULL, const __LineInfo* pLineInfo = NULL);

      /////<summary> wchar_t* string equality comparison.</summary>
      //static void AreEqual(const wchar_t* expected, const wchar_t* actual, const char* message, const __LineInfo* pLineInfo = NULL);

      ///<summary> wchar_t* string equality comparison.</summary>
      static void AreEqual(const wchar_t* expected, const wchar_t* actual, bool ignoreCase = false, const char* message = NULL, const __LineInfo* pLineInfo = NULL);
 
      ///<summary> double AreNotEqual comparison.</summary>
      static void AreNotEqual(double notExpected, double actual, double tolerance, const char* message = NULL, const __LineInfo* pLineInfo = NULL);

      ///<summary> float AreNotEqual comparison.</summary>
      static void AreNotEqual(float notExpected, float actual, float tolerance, const char* message = NULL, const __LineInfo* pLineInfo = NULL);

      ///<summary> char* string AreNotEqual comparison, with char* message.</summary>
      static void AreNotEqual(const char* notExpected, const char* actual, const char* message, const __LineInfo* pLineInfo = NULL);

      ///<summary> char* string AreNotEqual comparison, with wchar_t* message.</summary>
      static void AreNotEqual(const char* notExpected, const char* actual, const wchar_t* message, const __LineInfo* pLineInfo = NULL);

      ///<summary> char* string AreNotEqual comparison.</summary>
      static void AreNotEqual(const char* notExpected, const char* actual, bool ignoreCase = false, const char* message = NULL, const __LineInfo* pLineInfo = NULL);

      ///<summary> wchar_t* string AreNotEqual comparison with char* message.</summary>
      static void AreNotEqual(const wchar_t* notExpected, const wchar_t* actual, const char* message, const __LineInfo* pLineInfo = NULL);

      ///<summary> wchar_t* string AreNotEqual comparison with wchar_t* message.</summary>
      static void AreNotEqual(const wchar_t* notExpected, const wchar_t* actual, const wchar_t* message, const __LineInfo* pLineInfo = NULL);

      ///<summary> wchar_t* string AreNotEqual comparison.</summary>
      static void AreNotEqual(const wchar_t* notExpected, const wchar_t* actual, bool ignoreCase = false, const char* message = NULL, const __LineInfo* pLineInfo = NULL);

      ///<summary> Verify that a condition is true.</summary>
      static void IsTrue(bool condition, const char* message = NULL, const __LineInfo* pLineInfo = NULL);

      ///<summary> Verify that a conditon is false.</summary>
      static void IsFalse(bool condition, const char* message = NULL, const __LineInfo* pLineInfo = NULL);

      ///<summary> Force the test case result to be Failed.</summary>
      static void Fail(const char* message = NULL, const __LineInfo* pLineInfo = NULL);
   };
}
