#pragma once

#include <Windows.h>
#include <tlhelp32.h>
#include <string>

struct WindowUtils
{
	static HWND FindTopWindow(const DWORD pid);
	static HWND FindWindowByTitle(std::string title);
	static bool IsAppRunning(const HANDLE& snapshot, PROCESSENTRY32W& entry, wchar_t exeNameWide[MAX_PATH]);
};