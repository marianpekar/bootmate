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

static std::map<std::string, int> virtualKeys {
    { "return", VK_RETURN },
    { "enter", VK_RETURN },
    { "shift", VK_SHIFT },
    { "ctrl", VK_CONTROL },
    { "control", VK_CONTROL},
    { "alt", VK_MENU },
    { "backspace", VK_BACK },
    { "delete", VK_DELETE },
    { "tab", VK_TAB },
    { "escape", VK_ESCAPE },
    { "space", VK_SPACE },
    { "left", VK_LEFT },
    { "right", VK_RIGHT },
    { "up", VK_UP },
    { "down", VK_DOWN },
    { "home", VK_HOME },
    { "end", VK_END },
    { "pageup", VK_PRIOR },
    { "pagedown", VK_NEXT },
    { "insert", VK_INSERT },
    { "f1", VK_F1 },
    { "f2", VK_F2 },
    { "f3", VK_F3 },
    { "f4", VK_F4 },
    { "f5", VK_F5 },
    { "f6", VK_F6 },
    { "f7", VK_F7 },
    { "f8", VK_F8 },
    { "f9", VK_F9 },
    { "f10", VK_F10 },
    { "f11", VK_F11 },
    { "f12", VK_F12 },
    { "numpad0", VK_NUMPAD0 },
    { "numpad1", VK_NUMPAD1 },
    { "numpad2", VK_NUMPAD2 },
    { "numpad3", VK_NUMPAD3 },
    { "numpad4", VK_NUMPAD4 },
    { "numpad5", VK_NUMPAD5 },
    { "numpad6", VK_NUMPAD6 },
    { "numpad7", VK_NUMPAD7 },
    { "numpad8", VK_NUMPAD8 },
    { "numpad9", VK_NUMPAD9 },
    { "multiply", VK_MULTIPLY },
    { "add", VK_ADD },
    { "separator", VK_SEPARATOR },
    { "subtract", VK_SUBTRACT },
    { "decimal", VK_DECIMAL },
    { "divide", VK_DIVIDE },
};
struct PressKeyAction : public Action
{
    std::string keyName;
    PressKeyAction(std::string keyName) : Action(ActionTypeName.pressAction), keyName(keyName) {}

    void Execute() override
    {
        INPUT input{ 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = virtualKeys[keyName];
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
