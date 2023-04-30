#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include "actions.h"

std::map<std::string, std::string> ReadIniFile(std::string filename)
{
    std::map<std::string, std::string> values;
    std::ifstream file(filename);

    if (file.is_open())
    {
        std::string line;

        while (getline(file, line))
        {
            size_t comment = line.find(';');
            if (comment != std::string::npos)
                line = line.substr(0, comment);

            line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
            line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

            if (line.empty())
                continue;

            size_t equals = line.find('=');
            if (equals != std::string::npos)
            {
                std::string key = line.substr(0, equals);
                std::string value = line.substr(equals + 1);
                values[key] = value;
            }
        }

        file.close();
    }
    else
    {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }

    return values;
}

std::vector<Action*> CreateActions(std::string filename) 
{
    std::vector<Action*> actions;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return actions;
    }

    
    std::string line;

    while (getline(file, line))
    {
        size_t comment = line.find(';');
        if (comment != std::string::npos)
            line = line.substr(0, comment);

        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

        if (line.empty())
            continue;

        size_t equals = line.find(':');
        if (equals == std::string::npos)
            continue;
            
        std::string key = line.substr(0, equals);
        std::string value = line.substr(equals + 1);
        if (key == ActionTypeName.writeAction)
        {
            std::wstring chars(value.begin(), value.end());
            for (const auto& c : chars)
            {
                actions.emplace_back(new WriteAction(c));
            }
        }
        if (key == ActionTypeName.pressAction)
        {
            std::wstring chars(value.begin(), value.end());
            actions.emplace_back(new PressKeyAction(value));                    
        }
        if (key == ActionTypeName.sleepAction)
        {
            DWORD ms = std::stoul(value);
            actions.emplace_back(new SleepAction(ms));
        }      
    }
    file.close();
    return actions;
}

HWND FindTopWindow(DWORD pid)
{
    std::pair<HWND, DWORD> params = { 0, pid };
    BOOL bResult = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        auto pParams = (std::pair<HWND, DWORD>*)(lParam);
        DWORD processId;
        if (GetWindowThreadProcessId(hwnd, &processId) && processId == pParams->second)
        {
            SetLastError(-1);
            pParams->first = hwnd;
            return FALSE;
        }
        return TRUE;
    }, (LPARAM)&params);

    if (!bResult && GetLastError() == -1 && params.first)
        return params.first;

    return 0;
}

int main(int argc, char* argv[])
{
    std::map<std::string, std::string> ini = ReadIniFile("bootmate.ini");

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    char* recipyPathArg = argv[1];
    char* exeArg = argv[2];

    wchar_t exe[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, exeArg, -1, exe, MAX_PATH);

    std::string exePathStr = std::string(exeArg);
    std::replace(exePathStr.begin(), exePathStr.end(), '\\', '/');
    size_t pos = std::string(exePathStr).find_last_of("/");
    std::string exeName = exePathStr.substr(pos + 1);

    wchar_t exeNameWide[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, exeName.c_str(), -1, exeNameWide, MAX_PATH);

    bool noMultipleInstances = (bool)std::stoi(ini["bNoMultipleInstances"]);
    if (noMultipleInstances)
    {
        bool isAppRunning = false;
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (_wcsicmp(entry.szExeFile, exeNameWide) == 0)
            {
                isAppRunning = true;
                break;
            }
        }

        if (isAppRunning)
        {
            std::cout << exeName << " is already running" << std::endl;
            CloseHandle(snapshot);
            return 0;
        }
    }

    std::stringstream cmdLineArgsSs;
    for (size_t i = 3; i < argc; ++i)
    {
        cmdLineArgsSs << " " << argv[i];
    }
    std::string cmdLineArgs = cmdLineArgsSs.str();
    wchar_t cmdLineArgsWide[1024];
    MultiByteToWideChar(CP_UTF8, 0, cmdLineArgs.c_str(), -1, cmdLineArgsWide, MAX_PATH);

    std::wcout << "Running " << exe << " " << cmdLineArgs.c_str() << std::endl;

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    if (CreateProcess(exe, (LPWSTR)(cmdLineArgsWide), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        Sleep(500);

        DWORD processID = 0;
        CloseHandle(snapshot);
        snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (_wcsicmp(entry.szExeFile, exeNameWide) == 0)
            {
                processID = entry.th32ProcessID;
                break;
            }
        }

        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
        HWND targetWindow = NULL;
        bool findWindowByTitle = (bool)std::stoi(ini["bFindWindowByTitle"]);
        if (findWindowByTitle)
        {
            std::string windowTitle = ini["sWindowTitle"];
            std::wstring windowTitleWide(windowTitle.begin(), windowTitle.end());
            targetWindow = FindWindow(NULL, windowTitleWide.c_str());
        }
        else
        {
            targetWindow = FindTopWindow(processID);
        }

        if (targetWindow != NULL)
        {
            SetForegroundWindow(targetWindow);
            SetFocus(targetWindow);

            std::vector<Action*> actions = CreateActions(recipyPathArg);
            for (auto& action : actions)
            {
                if (action->type == "write")
                {
                    WriteAction* wAction = reinterpret_cast<WriteAction*>(action);
                    SendInput(1, &wAction->input, sizeof(INPUT));
                }
                else if (action->type == "press")
                {
                    PressKeyAction* pkAction = reinterpret_cast<PressKeyAction*>(action);
                    SendInput(1, &pkAction->input, sizeof(INPUT));
                }
                else if (action->type == "sleep")
                {
                    SleepAction* sleepAction = reinterpret_cast<SleepAction*>(action);
                    Sleep(sleepAction->ms);
                }
            }
        }

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
    CloseHandle(snapshot);
}