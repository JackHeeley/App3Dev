//
// system_error.hpp : implements system error code and message decoding
//
// Copyright (c) 2017 Jack Heeley, all rights reserved
//
#pragma once

#ifdef BASICUNIVERSALCPPSUPPORT_EXPORTS
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <string>
#include <memory>

#include "spimpl.hpp"

#pragma warning (disable:4251 4290)

///<summary> wraps the system error facility.</summary>
class BASICUNIVERSALCPPSUPPORT_API SystemError {

public:
   ///<summary> constructs a system error for the last system error.</summary>
   SystemError() noexcept;

   ///<summary> constructs a system error from an error code.</summary>
   SystemError(int errorCode) noexcept;

   ///<summary> equals comparison operator.</summary>
   ///<remarks> defines equals to mean identical error_code content.</remarks>
   bool operator==(const SystemError& other) const;

   ///<summary> not equals comparison operator.</summary>
   ///<remarks> defines not equals to mean differing error_code content.</remarks>
   bool operator!=(const SystemError& other) const;

   ///<summary> get the system error text corresponding to the error code.</summary>
   const std::string get_error_text() const;

   ///<summary> get the error code.</summary>
   const int get_error_code() const noexcept;

   ///<summary> clear the error code.</summary>
   void clear_error_code() const noexcept;

private:
   ///<summary> forward reference to private implementation.</summary>
   class impl;

   ///<summary> smart unique pointer to private implementation.</summary>
   ///<remarks> with default copy, move and compare support.</remarks>
    spimpl::impl_ptr<impl> pimpl;
};

#pragma warning (default:4251 4290)