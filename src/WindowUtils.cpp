#include <iostream>
#include <utility>
#include <string>
#include <algorithm>
#include "WindowUtils.h"
#include "ConfigLoader.h"

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

HWND WindowUtils::RunExe(char* exeArg, std::string subprocessCmdLineArgs)
{
    wchar_t wExe[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, exeArg, -1, wExe, MAX_PATH);

    std::string exePath = std::string(exeArg);
    std::replace(exePath.begin(), exePath.end(), '\\', '/');
    size_t pos = std::string(exePath).find_last_of("/");
    std::string exeName = exePath.substr(pos + 1);

    wchar_t wExeName[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, exeName.c_str(), -1, wExeName, MAX_PATH);

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    bool noMultipleInstances = ConfigLoader::HasElement("bNoMultipleInstances") ? (bool)std::stoi(ConfigLoader::ini["bNoMultipleInstances"]) : false;
    if (noMultipleInstances && WindowUtils::IsAppRunning(snapshot, entry, wExeName))
    {
        std::cout << exeName << " is already running and bNoMultipleInstances=1" << std::endl;
        return 0;
    }

    wchar_t wSubprocessCmdLineArgs[2048];
    MultiByteToWideChar(CP_UTF8, 0, subprocessCmdLineArgs.c_str(), -1, wSubprocessCmdLineArgs, MAX_PATH);

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    std::wcout << "Running " << wExe << " " << subprocessCmdLineArgs.c_str() << std::endl;
    if (CreateProcess(wExe, (LPWSTR)(wSubprocessCmdLineArgs), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        Sleep(500);

        DWORD processID = 0;
        CloseHandle(snapshot);
        snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (_wcsicmp(entry.szExeFile, wExeName) == 0)
            {
                processID = entry.th32ProcessID;
                break;
            }
        }
        CloseHandle(snapshot);

        OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
        HWND targetWindow = ConfigLoader::HasElement("bFindWindowByTitle") && std::stoi(ConfigLoader::ini["bFindWindowByTitle"]) == 1 ?
            WindowUtils::FindWindowByTitle(ConfigLoader::ini["sWindowTitle"]) :
            WindowUtils::FindTopWindow(processID);

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

        return targetWindow;
    }

    return NULL;
}