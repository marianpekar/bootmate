#include "WindowUtils.h"
#include <utility>

HWND WindowUtils::FindTopWindow(const DWORD pid)
{
    std::pair<HWND, DWORD> params = { 0, pid };
    BOOL result = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        auto pParams = (std::pair<HWND, DWORD>*)(lParam);
        DWORD processId;
        if (GetWindowThreadProcessId(hwnd, &processId) && processId == pParams->second)
        {
            SetLastError(-1);
            pParams->first = hwnd;
            return FALSE;
        }
        return TRUE;
    }, 
    (LPARAM)&params);

    if (!result && GetLastError() == -1 && params.first)
        return params.first;

    return 0;
}

bool WindowUtils::IsAppRunning(const HANDLE& snapshot, PROCESSENTRY32W& entry, wchar_t exeNameWide[MAX_PATH])
{
    while (Process32Next(snapshot, &entry) == TRUE)
    {
        if (_wcsicmp(entry.szExeFile, exeNameWide) == 0)
        {
            return true;
        }
    }
    return false;
}

HWND WindowUtils::FindWindowByTitle(std::string title)
{
    std::wstring wTitle(title.begin(), title.end());
    return FindWindow(NULL, wTitle.c_str());
}
