#include <iostream>
#include <sstream>
#include <string>
#include <Windows.h>
#include "Actions.h"
#include "ActionsFactory.h"
#include "ConfigLoader.h"
#include "WinUtils.h"

std::string CreateSubprocessCmdLineArgs(int argc, char* argv[])
{
    std::stringstream ss;
    for (size_t i = 3; i < argc; ++i)
    {
        ss << " " << argv[i];
    }
    return ss.str();
}

void CookRecipe(char* recipePathArg)
{
    int defaultDelay = ConfigLoader::HasElement("iDefaultDelay") ? std::stoi(ConfigLoader::ini["iDefaultDelay"]) : 1;
    std::vector<Action*> actions = ActionsFactory::CreateActions(recipePathArg);
    HoldKeyAction* hkAction = nullptr;
    HoldMouseButtonAction* hmbAction = nullptr;
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
        else if (action->type == ActionTypeName.holdMouseButtonAction)
        {
            hmbAction = reinterpret_cast<HoldMouseButtonAction*>(action);
            hmbAction->Execute();
        }
        else if (action->type == ActionTypeName.releaseMouseButtonAction)
        {
            ReleaseMouseButtonAction* rmbAction = reinterpret_cast<ReleaseMouseButtonAction*>(action);
            rmbAction->Execute();
            delete rmbAction;
        }
        Sleep(defaultDelay);
    }

    if (hkAction != nullptr)
    {
        delete hkAction;
    }

    if (hmbAction != nullptr)
    {
        delete hmbAction;
    }
}

int main(int argc, char* argv[])
{
    ConfigLoader::LoadIniFile("bootmate.ini");

    if (argc < 2)
    {
        std::cerr << "You must provide at least a recipe file" << std::endl;
        return -1;
    }

    char* recipePathArg = argv[1];

    if (argc < 3)
    {
        if (ConfigLoader::HasElement("bFindWindowByTitle") && std::stoi(ConfigLoader::ini["bFindWindowByTitle"]) == 1)
        {   
            HWND targetWindow = WinUtils::FindWindowByTitle(ConfigLoader::ini["sWindowTitle"]);
            if (targetWindow != NULL)
            {
                SetForegroundWindow(targetWindow);
                SetFocus(targetWindow);
                CookRecipe(recipePathArg);
                return 0;
            }
            std::cerr << "Window not found" << std::endl;
            return -1;
        }
        CookRecipe(recipePathArg);
        return 0;
    }

    char* exeArg = argv[2];
    std::string subprocessCmdLineArgs = CreateSubprocessCmdLineArgs(argc, argv);

    HWND targetWindow = WinUtils::RunExe(exeArg, subprocessCmdLineArgs);
    if (targetWindow != NULL)
    {
        SetForegroundWindow(targetWindow);
        SetFocus(targetWindow);
        CookRecipe(recipePathArg);
    }

    return 0;
}
