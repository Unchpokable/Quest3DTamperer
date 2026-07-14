#include "util/text_convert.h"
#include "pch.h"

namespace util
{

std::string Utf8Encode(const std::wstring& wstr)
{
    if(wstr.empty())
        return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::wstring Utf8Decode(const std::string& str)
{
    if(str.empty())
        return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::size_t ReplaceAll(std::string& inout, std::string_view what, std::string_view with)
{
    std::size_t count {};
    for(std::string::size_type pos {}; inout.npos != (pos = inout.find(what.data(), pos, what.length())); pos += with.length(), ++count) {
        inout.replace(pos, what.length(), with.data(), with.length());
    }
    return count;
}

} // namespace util
