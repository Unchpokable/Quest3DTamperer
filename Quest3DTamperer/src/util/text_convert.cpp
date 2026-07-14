#include "util/text_convert.h"
#include "pch.h"

namespace util
{

std::string utf8_encode(const std::wstring& wstr)
{
    if(wstr.empty())
        return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string str_to(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str_to[0], size_needed, nullptr, nullptr);
    return str_to;
}

std::wstring utf8_decode(const std::string& str)
{
    if(str.empty())
        return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
    std::wstring wstr_to(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr_to[0], size_needed);
    return wstr_to;
}

std::size_t replace_all(std::string& inout, std::string_view what, std::string_view with)
{
    std::size_t count {};
    for(std::string::size_type pos {}; inout.npos != (pos = inout.find(what.data(), pos, what.length())); pos += with.length(), ++count) {
        inout.replace(pos, what.length(), with.data(), with.length());
    }
    return count;
}

} // namespace util
