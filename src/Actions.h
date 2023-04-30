#pragma once

#include <Windows.h>
#include <tlhelp32.h>
#include <string>

static struct ActionTypeName
{
    const char* writeAction = "write";
    const char* pressAction = "press";
    const char* sleepAction = "sleep";

} ActionTypeName;

struct Action
{
    const char* type;
    Action(const char* type) : type(type) {}
};

struct WriteAction : public Action
{
    INPUT input{ 0 };

    WriteAction(const wchar_t c) : Action(ActionTypeName.writeAction)
    {
        input.type = INPUT_KEYBOARD;
        input.ki.wScan = c;
        input.ki.dwFlags = KEYEVENTF_UNICODE;
    }
};

struct PressKeyAction : public Action
{
    INPUT input{ 0 };

    PressKeyAction(std::string key) : Action(ActionTypeName.pressAction)
    {
        input.type = INPUT_KEYBOARD;
        if (key == "return" || key == "enter")
        {
            input.ki.wVk = VK_RETURN;
        }
    }
};

struct SleepAction : public Action
{
    DWORD ms;
    SleepAction(DWORD ms) : Action(ActionTypeName.sleepAction), ms(ms) {}
};