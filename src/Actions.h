#pragma once

#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include <map>

static struct ActionTypeName
{
    const char* writeAction = "write";
    const char* pressAction = "press";
    const char* sleepAction = "sleep";
    const char* setCursorPosAction = "cursor";
    const char* mousceClickAction = "click";
    const char* mouseScrollAction = "scroll";

} ActionTypeName;

static struct MouseClickNames
{
    const char* leftClick = "left";
    const char* middleClick = "middle";
    const char* rightClick = "right";

} MouseClickNames;

struct Action
{
    const char* type;
    Action(const char* type) : type(type) {}
    virtual void Execute() = 0;
};

struct WriteAction : public Action
{
    const wchar_t c;
    WriteAction(const wchar_t c) : Action(ActionTypeName.writeAction), c(c) {}

    void Execute() override
    {
        INPUT input{ 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wScan = c;
        input.ki.dwFlags = KEYEVENTF_UNICODE;
        SendInput(1, &input, sizeof(INPUT));
    }
};

struct PressKeyAction : public Action
{
    std::string key;
    PressKeyAction(std::string key) : Action(ActionTypeName.pressAction), key(key) {}

    void Execute() override
    {
        INPUT input{ 0 };
        input.type = INPUT_KEYBOARD;
        if (key == "return" || key == "enter")
        {
            input.ki.wVk = VK_RETURN;
        }
        SendInput(1, &input, sizeof(INPUT));
    }
};

struct SleepAction : public Action
{
    DWORD ms;
    SleepAction(DWORD ms) : Action(ActionTypeName.sleepAction), ms(ms) {}

    void Execute() override
    {
        Sleep(ms);
    }
};

struct SetCursorPosAction : public Action
{
    int x, y;
    SetCursorPosAction(int x, int y) : Action(ActionTypeName.setCursorPosAction), x(x), y(y) {}

    void Execute() override
    {
        SetCursorPos(x, y);
    }
};

struct MouseClickAction : public Action
{
    DWORD dwFlags = 0;

    MouseClickAction(std::string mouseClickName) : Action(ActionTypeName.mousceClickAction)
    {
        if (mouseClickName == MouseClickNames.leftClick)
        {
            dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
        }
        else if (mouseClickName == MouseClickNames.middleClick)
        {
            dwFlags = MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP;
        }
        else if (mouseClickName == MouseClickNames.rightClick)
        {
            dwFlags = MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP;
        }
    }

    void Execute() override
    {
        INPUT input{ 0 };
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = dwFlags;
        SendInput(1, &input, sizeof(INPUT));
    }
};

struct MouseScrollAction : public Action
{
    DWORD scrollValue;
    
    MouseScrollAction(DWORD scrollValue) : Action(ActionTypeName.mouseScrollAction), scrollValue(scrollValue) {}

    void Execute() override
    {
        INPUT input{ 0 };
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_WHEEL;
        input.mi.mouseData = scrollValue;
        SendInput(1, &input, sizeof(INPUT));
    }
};
