#pragma once

#include "pch.h"

namespace util
{

std::string utf8_encode(const std::wstring& wstr);
std::wstring utf8_decode(const std::string& str);

// Replaces every occurrence of `what` with `with` in `inout`, in place.
// Returns the number of replacements made.
std::size_t replace_all(std::string& inout, std::string_view what, std::string_view with);

} // namespace util
