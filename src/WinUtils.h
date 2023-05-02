#pragma once

#include <Windows.h>
#include <tlhelp32.h>
#include <string>

struct WinUtils
{
	static HWND FindTopWindow(const DWORD pid);
	static HWND FindWindowByTitle(std::string title);
	static HWND RunExe(char* exeArg, std::string subprocessCmdLineArgs);
	static bool IsAppRunning(const HANDLE& snapshot, PROCESSENTRY32W& entry, wchar_t exeNameWide[MAX_PATH]);
};