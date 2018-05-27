//
// utf8_assert.h : assert in utf8 everywhere style
//				this is a windows platform implementation
//				builds and runs on x86 & x64 architectures
//
// Copyright (c) 2015 Jack Heeley, Fox-IT
// 
#pragma once

#include "stdafx.h"

#include <CppUnitTest.h>

#include "utf8_convert.hpp"
#include "utf8_assert.hpp"

namespace utf8
{
   struct __LineInfo
   {
      __LineInfo(const wchar_t* pszFileName, const char* pszFuncName, int lineNumber)
         :pszFile(pszFileName),
         pszFunc(reinterpret_cast<const unsigned char*>(pszFuncName)),
         line(lineNumber)
      {
      }

      const wchar_t* pszFile;
      const unsigned char* pszFunc;
      int line;
   };

   ///<summary> double equality comparison.</summary>
   void Assert::AreEqual(double expected, double actual, double tolerance, const char* message, const __LineInfo* pLineInfo)
   {
      Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected, actual, tolerance, message ? utf8::convert::to_utf16(message).c_str() : NULL, reinterpret_cast<const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pLineInfo));
   }

   ///<summary> float equality comparison.</summary>
   void Assert::AreEqual(float expected, float actual, float tolerance, const char* message, const __LineInfo* pLineInfo)
   {
      Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected, actual, tolerance, message ? utf8::convert::to_utf16(message).c_str() : NULL, reinterpret_cast<const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pLineInfo));
   }

   ///<summary> char* string equality comparison.</summary>
   void Assert::AreEqual(const char* expected, const char* actual, bool ignoreCase, const char* message, const __LineInfo* pLineInfo)
   {
      Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected, actual, ignoreCase, message ? utf8::convert::to_utf16(message).c_str() : NULL, reinterpret_cast<const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pLineInfo));
   }

   ///<summary> wchar_t* string equality comparison.</summary>
   void Assert::AreEqual(const wchar_t* expected, const wchar_t* actual, bool ignoreCase, const char* message, const __LineInfo* pLineInfo)
   {
      Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected, actual, ignoreCase, message ? utf8::convert::to_utf16(message).c_str() : NULL, reinterpret_cast<const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pLineInfo));
   }

   ///<summary> double AreNotEqual comparison.</summary>
   void Assert::AreNotEqual(double notExpected, double actual, double tolerance, const char* message, const __LineInfo* pLineInfo)
   {
      Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(notExpected, actual, tolerance, message ? utf8::convert::to_utf16(message).c_str() : NULL, reinterpret_cast<const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pLineInfo));
   }

   ///<summary> float AreNotEqual comparison.</summary>
   void Assert::AreNotEqual(float notExpected, float actual, float tolerance, const char* message, const __LineInfo* pLineInfo)
   {
      Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(notExpected, actual, tolerance, message ? utf8::convert::to_utf16(message).c_str() : NULL, reinterpret_cast<const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pLineInfo));
   }

   ///<summary> char* string AreNotEqual comparison, with char* message.</summary>
   void Assert::AreNotEqual(const char* notExpected, const char* actual, bool ignoreCase, const char* message, const __LineInfo* pLineInfo)
   {
      Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(notExpected, actual, ignoreCase, message ? utf8::convert::to_utf16(message).c_str() : NULL, reinterpret_cast<const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pLineInfo));
   }

   ///<summary> char* string AreNotEqual comparison, with wchar_t* message.</summary>
   void Assert::AreNotEqual(const char* notExpected, const char* actual, const wchar_t* message, const __LineInfo* pLineInfo)
   {
      Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(notExpected, actual, message, reinterpret_cast<const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pLineInfo));
   }

   ///<summary> wchar_t* string AreNotEqual comparison, with char* message.</summary>
   void Assert::AreNotEqual(const wchar_t* notExpected, const wchar_t* actual, bool ignoreCase, const char* message, const __LineInfo* pLineInfo)
   {
      Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(notExpected, actual, ignoreCase, message ? utf8::convert::to_utf16(message).c_str() : NULL, reinterpret_cast<const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pLineInfo));
   }

   ///<summary> wchar_t* string AreNotEqual comparison with wchar_t* message.</summary>
   void Assert::AreNotEqual(const wchar_t* notExpected, const wchar_t* actual, const wchar_t* message, const __LineInfo* pLineInfo)
   {
      Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(notExpected, actual, message, reinterpret_cast<const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pLineInfo));
   }

   ///<summary> Verify that a condition is true.</summary>
   void Assert::IsTrue(bool condition, const char* message, const __LineInfo* pLineInfo)
   {
      Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(condition, message ? utf8::convert::to_utf16(message).c_str() : NULL, reinterpret_cast<const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pLineInfo));
   }

   ///<summary> Verify that a conditon is false.</summary>
   void Assert::IsFalse(bool condition, const char* message, const __LineInfo* pLineInfo)
   {
      Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsFalse(condition, message ? utf8::convert::to_utf16(message).c_str() : NULL, reinterpret_cast<const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pLineInfo));
   }

   ///<summary> Force the test case result to be Failed.</summary>
   void Assert::Fail(const char* message, const __LineInfo* pLineInfo)
   {
      Microsoft::VisualStudio::CppUnitTestFramework::Assert::Fail(message ? utf8::convert::to_utf16(message).c_str() : NULL, reinterpret_cast<const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pLineInfo));
   }
}