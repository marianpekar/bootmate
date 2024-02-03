#include <iostream>
#include <sstream>
#include <string>
#include <Windows.h>
#include "Actions.h"
#include "ActionsFactory.h"
#include "ConfigLoader.h"
#include "WinUtils.h"

void CookRecipe(const char* recipePathArg)
{
    const int defaultDelay = ConfigLoader::HasElement("iDefaultDelay") ? std::stoi(ConfigLoader::ini["iDefaultDelay"]) : 1;
    
    for (const std::vector<Action*> actions = ActionsFactory::CreateActions(recipePathArg); const auto& action : actions)
    {
        action->Execute();
        Sleep(defaultDelay);
    }

    for (const std::vector<Action*> actions = ActionsFactory::CreateActions(recipePathArg); const auto& action : actions)
    {
        delete action;
    }
}

std::string CreateSubprocessCmdLineArgs(const int argc, char* argv[])
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
    ConfigLoader::LoadIniFile("bootmate.ini");

    if (argc < 2)
    {
        std::cerr << "You must provide at least a recipe file\n";
        return -1;
    }

    const char* recipePathArg = argv[1];

    if (argc < 3)
    {
        if (ConfigLoader::HasElement("bFindWindowByTitle") && std::stoi(ConfigLoader::ini["bFindWindowByTitle"]) == 1)
        {
            const HWND targetWindow = WinUtils::FindWindowByTitle(ConfigLoader::ini["sWindowTitle"]);
            if (targetWindow != NULL)
            {
                SetForegroundWindow(targetWindow);
                SetFocus(targetWindow);
                CookRecipe(recipePathArg);
                return 0;
            }
            std::cerr << "Window not found\n";
            return -1;
        }
        CookRecipe(recipePathArg);
        return 0;
    }

    const char* exeArg = argv[2];
    const std::string subprocessCmdLineArgs = CreateSubprocessCmdLineArgs(argc, argv);

    const HWND targetWindow = WinUtils::RunExe(exeArg, subprocessCmdLineArgs);
    if (targetWindow != NULL)
    {
        SetForegroundWindow(targetWindow);
        SetFocus(targetWindow);
        CookRecipe(recipePathArg);
    }

    return 0;
}
