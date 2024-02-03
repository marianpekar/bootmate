#pragma once

#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include <map>
#include <utility>
#include "ConfigLoader.h"
#include "WinUtils.h"

static struct ActionTypeName
{
    const char* write = "write";
    const char* press = "press";
    const char* hold = "hold";
    const char* release = "release";
    const char* sleep = "sleep";
    const char* setCursorPos = "set cursor";
    const char* moveCursor = "move cursor";
    const char* pointCursor = "point cursor";
    const char* mouseClick = "click";
    const char* mouseScroll = "scroll";
    const char* holdMouseButton = "hold mouse";
    const char* releaseMouseButton = "release mouse";
    const char* run = "run";

} ActionTypeName;

static struct MouseButtonNames
{
    const char* left = "left";
    const char* middle = "middle";
    const char* right = "right";

} MouseClickNames;

struct Action
{
    const char* type;
    Action(const char* type) : type(type) {}
    virtual void Execute() = 0;
    virtual ~Action() {}
};

struct WriteAction : Action
{
    std::string chars;
    DWORD delay;
    WriteAction(std::string chars) : Action(ActionTypeName.write), chars(std::move(chars)),
        delay(ConfigLoader::HasElement("iDefaultWriteDelay") ? std::stoi(ConfigLoader::ini["iDefaultWriteDelay"]) : 1) {}
    
    void Execute() override
    {
        for (const auto& c : chars)
        {
            INPUT input{ 0 };
            input.type = INPUT_KEYBOARD;
            input.ki.wScan = c;
            input.ki.dwFlags = KEYEVENTF_UNICODE;
            SendInput(1, &input, sizeof(INPUT));
            Sleep(delay);
        }
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
    { "esc", VK_ESCAPE },
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
    { "*", VK_MULTIPLY },
    { "add", VK_ADD },
    { "+", VK_ADD },
    { "subtract", VK_SUBTRACT },
    { "-", VK_SUBTRACT },
    { "decimal", VK_DECIMAL },
    { ".", VK_DECIMAL },
    { "divide", VK_DIVIDE },
    { "/", VK_DIVIDE },
    { "0", 0x30 },
    { "1", 0x31 },
    { "2", 0x32 },
    { "3", 0x33 },
    { "4", 0x34 },
    { "5", 0x35 },
    { "6", 0x36 },
    { "7", 0x37 },
    { "8", 0x38 },
    { "9", 0x39 },
    { "a", 0x41 },
    { "b", 0x42 },
    { "c", 0x43 },
    { "d", 0x44 },
    { "e", 0x45 },
    { "f", 0x46 },
    { "g", 0x47 },
    { "h", 0x48 },
    { "i", 0x49 },
    { "j", 0x4A },
    { "k", 0x4B },
    { "l", 0x4C },
    { "m", 0x4D },
    { "n", 0x4E },
    { "o", 0x4F },
    { "p", 0x50 },
    { "q", 0x51 },
    { "r", 0x52 },
    { "s", 0x53 },
    { "t", 0x54 },
    { "u", 0x55 },
    { "v", 0x56 },
    { "w", 0x57 },
    { "x", 0x58 },
    { "y", 0x59 },
    { "z", 0x5A }
};

struct PressKeyAction : Action
{
    std::string keyName;
    PressKeyAction(std::string keyName) : Action(ActionTypeName.press), keyName(std::move(keyName)) {}

    void Execute() override
    {
        INPUT input{ 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = virtualKeys[keyName];
        SendInput(1, &input, sizeof(INPUT));
    }
};

struct ReleaseKeyAction : Action
{
    std::string keyName;
    ReleaseKeyAction(std::string keyName) : Action(ActionTypeName.release), keyName(std::move(keyName)) {}

    void Execute() override
    {
        INPUT input{ 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = virtualKeys[keyName];
        input.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_EXTENDEDKEY;
        SendInput(1, &input, sizeof(INPUT));
    }
};

struct HoldKeyAction : Action
{
    std::string keyName;
    HoldKeyAction(std::string keyName) : Action(ActionTypeName.hold), keyName(std::move(keyName)) {}

    void Execute() override
    {
        INPUT input{ 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = virtualKeys[keyName];
        input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
        SendInput(1, &input, sizeof(INPUT));
    }

    ~HoldKeyAction() override
    {
        ReleaseKeyAction(keyName).Execute();
    }
};

struct SleepAction : Action
{
    DWORD ms;
    SleepAction(DWORD ms) : Action(ActionTypeName.sleep), ms(ms) {}

    void Execute() override
    {
        Sleep(ms);
    }
};

struct SetCursorPosAction : Action
{
    int x, y;
    SetCursorPosAction(const int x, const int y) : Action(ActionTypeName.setCursorPos), x(x), y(y) {}

    void Execute() override
    {
        SetCursorPos(x, y);
    }
};

struct MoveCursorAction : Action
{
    int x, y;
    MoveCursorAction(const int x, const int y) : Action(ActionTypeName.moveCursor), x(x), y(y) {}

    void Execute() override
    {
        POINT currentPos;
        GetCursorPos(&currentPos);
        SetCursorPos(currentPos.x + x, currentPos.y + y);
    }
};

struct PointCursorAction : Action
{
    std::string windowTitle;
    PointCursorAction(std::string windowTitle) : Action(ActionTypeName.pointCursor), windowTitle(std::move(windowTitle)) {}

    void Execute() override
    {
        const HWND windowHandle =WinUtils::FindWindowByTitle(windowTitle);
        if (windowHandle == NULL)
            return;

        RECT rect;
        if(!GetWindowRect(windowHandle, &rect))
            return;
        
        SetCursorPos(rect.left, rect.top);
    }
};

static std::map<std::string, int> mouseUpEvents
{
    { MouseClickNames.left, MOUSEEVENTF_LEFTUP },
    { MouseClickNames.middle, MOUSEEVENTF_MIDDLEUP },
    { MouseClickNames.right, MOUSEEVENTF_RIGHTUP },
};

static std::map<std::string, int> mouseDownEvents
{
    { MouseClickNames.left, MOUSEEVENTF_LEFTDOWN },
    { MouseClickNames.middle, MOUSEEVENTF_MIDDLEDOWN },
    { MouseClickNames.right, MOUSEEVENTF_RIGHTDOWN },
};


struct MouseClickAction : Action
{
    DWORD dwFlags = 0;

    MouseClickAction(const std::string& mouseClickName) : Action(ActionTypeName.mouseClick)
    {    
        dwFlags = mouseDownEvents[mouseClickName] | mouseUpEvents[mouseClickName];
    }

    void Execute() override
    {
        INPUT input{ 0 };
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = dwFlags;
        SendInput(1, &input, sizeof(INPUT));
    }
};

struct ReleaseMouseButtonAction : Action
{
    DWORD dwFlags = 0;

    ReleaseMouseButtonAction(const std::string& mouseClickName) : Action(ActionTypeName.releaseMouseButton)
    {
        dwFlags = mouseUpEvents[mouseClickName];
    }

    void Execute() override
    {
        INPUT input{ 0 };
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = dwFlags;
        SendInput(1, &input, sizeof(INPUT));
    }
};

struct HoldMouseButtonAction : Action
{
    std::string mouseClickName;

    HoldMouseButtonAction(std::string mouseClickName) : Action(ActionTypeName.holdMouseButton), mouseClickName(std::move(mouseClickName)) {}

    void Execute() override
    {
        INPUT input{ 0 };
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = mouseDownEvents[mouseClickName];
        SendInput(1, &input, sizeof(INPUT));
    }

    ~HoldMouseButtonAction() override
    {
        ReleaseMouseButtonAction(mouseClickName).Execute();
    }
};

struct MouseScrollAction : Action
{
    DWORD scrollValue;
    
    MouseScrollAction(DWORD scrollValue) : Action(ActionTypeName.mouseScroll), scrollValue(scrollValue) {}

    void Execute() override
    {
        INPUT input{ 0 };
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_WHEEL;
        input.mi.mouseData = scrollValue;
        SendInput(1, &input, sizeof(INPUT));
    }
};

struct RunAction : Action
{
    std::string exe;
    std::string args;

    RunAction(std::string exePath, std::string args) : Action(ActionTypeName.run), exe(std::move(exePath)), args(std::move(args)) {}

    void Execute() override
    {
        const HWND targetWindow = WinUtils::RunExe(exe.c_str(), args);
        if (targetWindow != NULL)
        {
            SetForegroundWindow(targetWindow);
            SetFocus(targetWindow);
        }
    }
};
