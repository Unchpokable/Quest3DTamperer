#pragma once

#include "pch.h"

namespace util
{

std::string Utf8Encode(const std::wstring& wstr);
std::wstring Utf8Decode(const std::string& str);

// Replaces every occurrence of `what` with `with` in `inout`, in place.
// Returns the number of replacements made.
std::size_t ReplaceAll(std::string& inout, std::string_view what, std::string_view with);

} // namespace util
