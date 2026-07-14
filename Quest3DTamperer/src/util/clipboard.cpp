#include "util/clipboard.h"
#include "pch.h"

namespace util
{
void copy_to_clipboard(HWND hwnd, const std::string& s)
{
    ::OpenClipboard(hwnd);
    ::EmptyClipboard();
    HGLOBAL hg = ::GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
    if(!hg) {
        ::CloseClipboard();
        return;
    }
    std::memcpy(::GlobalLock(hg), s.c_str(), s.size() + 1);
    ::GlobalUnlock(hg);
    ::SetClipboardData(CF_TEXT, hg);
    ::CloseClipboard();
    ::GlobalFree(hg);
}
} // namespace util
