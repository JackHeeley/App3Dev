//
// system_error.hpp : implements system error code and message decoding
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
#ifndef __SYSTEM_ERROR_HPP__
#define __SYSTEM_ERROR_HPP__

#ifdef BASICUNIVERSALCPPSUPPORT_EXPORTS
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllexport)
#else
#define BASICUNIVERSALCPPSUPPORT_API __declspec(dllimport)
#endif

#include <string>

#include "spimpl.hpp"

///<summary> wraps the system error facility.</summary>
class SystemError {

public:
   ///<summary> constructs a system error for the last system error.</summary>
   BASICUNIVERSALCPPSUPPORT_API SystemError() noexcept;

   ///<summary> constructs a system error from an error code.</summary>
   BASICUNIVERSALCPPSUPPORT_API SystemError(int errorCode) noexcept;

   ///<summary> equals comparison operator.</summary>
   ///<remarks> defines equals to mean identical error_code content.</remarks>
   BASICUNIVERSALCPPSUPPORT_API bool operator==(const SystemError& other) const;

   ///<summary> not equals comparison operator.</summary>
   ///<remarks> defines not equals to mean differing error_code content.</remarks>
   BASICUNIVERSALCPPSUPPORT_API bool operator!=(const SystemError& other) const;

   ///<summary> get the system error text corresponding to the error code.</summary>
   BASICUNIVERSALCPPSUPPORT_API const std::string get_error_text() const;

   ///<summary> get the error code.</summary>
   BASICUNIVERSALCPPSUPPORT_API const int get_error_code() const noexcept;

   ///<summary> clear the error code.</summary>
   BASICUNIVERSALCPPSUPPORT_API void clear_error_code() const noexcept;

private:
   ///<summary> forward reference to private implementation.</summary>
   class impl;

   ///<summary> smart unique pointer to private implementation.</summary>
   ///<remarks> with default copy, move and compare support.</remarks>
    spimpl::impl_ptr<impl> pimpl;
};

#endif // __SYSTEM_ERROR_HPP__
