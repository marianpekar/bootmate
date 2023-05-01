#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include <Windows.h>
#include <tlhelp32.h>

#include "Actions.h"
#include "ActionsFactory.h"
#include "ConfigLoader.h"
#include "WindowUtils.h"

std::string CreateSubprocessCmdLineArgs(int argc, char* argv[])
{
    std::stringstream ss;
    for (size_t i = 3; i < argc; ++i)
    {
        ss << " " << argv[i];
    }
    return ss.str();
}

int main(int argc, char* argv[])
{
    Dict ini = ConfigLoader::ReadIniFile("bootmate.ini");

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    char* recipyPathArg = argv[1];
    char* exeArg = argv[2];

    wchar_t wExe[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, exeArg, -1, wExe, MAX_PATH);

    std::string exePath = std::string(exeArg);
    std::replace(exePath.begin(), exePath.end(), '\\', '/');
    size_t pos = std::string(exePath).find_last_of("/");
    std::string exeName = exePath.substr(pos + 1);

    wchar_t wExeName[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, exeName.c_str(), -1, wExeName, MAX_PATH);

    bool noMultipleInstances = (bool)std::stoi(ini["bNoMultipleInstances"]);
    if (noMultipleInstances && WindowUtils::IsAppRunning(snapshot, entry, wExeName))
    {
        std::cout << exeName << " is already running and bNoMultipleInstances=1" << std::endl;
        return 0;
    }

    std::string subprocessCmdLineArgs = CreateSubprocessCmdLineArgs(argc, argv);
    wchar_t wSubprocessCmdLineArgs[2048];
    MultiByteToWideChar(CP_UTF8, 0, subprocessCmdLineArgs.c_str(), -1, wSubprocessCmdLineArgs, MAX_PATH);

    std::wcout << "Running " << wExe << " " << subprocessCmdLineArgs.c_str() << std::endl;

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
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

        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
        HWND targetWindow = targetWindow = (bool)std::stoi(ini["bFindWindowByTitle"]) ? 
            WindowUtils::FindWindowByTitle(ini["sWindowTitle"]) : 
            WindowUtils::FindTopWindow(processID);

        int defaultDelay = ConfigLoader::HasElement(ini, "iDefaultDelay") ? std::stoi(ini["iDefaultDelay"]) : 1;
        if (targetWindow != NULL)
        {
            SetForegroundWindow(targetWindow);
            SetFocus(targetWindow);

            std::vector<Action*> actions = ActionsFactory::CreateActions(recipyPathArg);
            HoldKeyAction* hkAction = nullptr;
            for (auto& action : actions)
            {
                if (action->type == ActionTypeName.writeAction)
                {
                    WriteAction* wAction = reinterpret_cast<WriteAction*>(action);
                    wAction->Execute();
                    delete wAction;
                }
                else if (action->type == ActionTypeName.pressAction)
                {
                    PressKeyAction* pkAction = reinterpret_cast<PressKeyAction*>(action);
                    pkAction->Execute();
                    delete pkAction;
                }
                else if (action->type == ActionTypeName.holdAction)
                {
                    hkAction = reinterpret_cast<HoldKeyAction*>(action);
                    hkAction->Execute();
                }
                else if (action->type == ActionTypeName.releaseAction)
                {
                    ReleaseKeyAction* rkAction = reinterpret_cast<ReleaseKeyAction*>(action);
                    rkAction->Execute();
                    delete rkAction;
                }
                else if (action->type == ActionTypeName.sleepAction)
                {
                    SleepAction* sleepAction = reinterpret_cast<SleepAction*>(action);
                    sleepAction->Execute();
                    delete sleepAction;
                }
                else if (action->type == ActionTypeName.setCursorPosAction)
                {
                    SetCursorPosAction* scpAction = reinterpret_cast<SetCursorPosAction*>(action);
                    scpAction->Execute();
                    delete scpAction;
                }
                else if (action->type == ActionTypeName.moveCursorAction)
                {
                    MoveCursorAction* mcAction = reinterpret_cast<MoveCursorAction*>(action);
                    mcAction->Execute();
                    delete mcAction;
                }
                else if (action->type == ActionTypeName.mouseClickAction)
                {
                    MouseClickAction* clickAction = reinterpret_cast<MouseClickAction*>(action);
                    clickAction->Execute();
                    delete clickAction;
                }
                else if (action->type == ActionTypeName.mouseScrollAction)
                {
                    MouseScrollAction* scrollAction = reinterpret_cast<MouseScrollAction*>(action);
                    scrollAction->Execute();
                    delete scrollAction;
                }
                Sleep(defaultDelay);
            }
            
            if (hkAction != nullptr)
            {
                delete hkAction;
            }
        }
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
    CloseHandle(snapshot);
}
