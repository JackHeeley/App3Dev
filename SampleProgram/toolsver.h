#ifndef __TOOLSVER_H__
#define __TOOLSVER_H__

// Check for known limitation with visual studio clang support. Fixed at VS 2019 16.9.0 preview 2 (compiler version 19.282.9617)
#if defined _MSC_VER
#define STRINGIZE(x) #x
#define TO_STRING_LITERAL(x) STRINGIZE(x)
#if _MSC_FULL_VER < 192829617
#pragma message("WARNING: Compiler version #" TO_STRING_LITERAL(_MSC_FULL_VER) " has issues with Clang-Tidy! An update is recommended. App3Dev x64 Release configuration is affected but WILL BUILD cleanly if you disable Clang-Tidy.")  // NOLINT(clang-diagnostic-#pragma-messages)
#pragma message("To disable Clang-Tidy: Go to the 'Project Properties/Code Analysis/General' property page and set 'Enable Clang-Tidy = No' for all projects, configurations, and platforms")  // NOLINT(clang-diagnostic-#pragma-messages)
#ifdef __clang__
#error Clang-Tidy requires at least Visual Sudio 2019 vesion 16.9.0 preview 2.
#endif // __clang__
#endif // _MSC_FULL_VER < 192829617
#endif // _MSC_VER

#endif // __TOOLSVER_H__