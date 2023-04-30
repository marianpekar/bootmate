#include <iostream>
#include <fstream>
#include "Actions.h"
#include "ActionsFactory.h"

std::vector<struct Action*> ActionsFactory::CreateActions(const std::string filename)
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

        size_t i = line.find(':');
        if (i == std::string::npos)
            continue;

        std::string key = line.substr(0, i);
        std::string value = line.substr(i + 1);
        if (key == ActionTypeName.writeAction)
        {
            std::wstring chars(value.begin(), value.end());
            for (const auto& c : chars)
            {
                actions.emplace_back(new WriteAction(c));
            }
        }
        else if (key == ActionTypeName.pressAction)
        {
            std::wstring chars(value.begin(), value.end());
            actions.emplace_back(new PressKeyAction(value));
        }
        else if (key == ActionTypeName.sleepAction)
        {
            DWORD ms = std::stoul(value);
            actions.emplace_back(new SleepAction(ms));
        }
        else if (key == ActionTypeName.setCursorPosAction)
        {
            i = value.find(' ');
            int x = std::stoi(value.substr(0, i));
            int y = std::stoi(value.substr(i + 1));
            actions.emplace_back(new SetCursorPosAction(x, y));
        }
        else if (key == ActionTypeName.mousceClickAction)
        {
            actions.emplace_back(new MouseClickAction(value));
        }
        else if (key == ActionTypeName.mouseScrollAction)
        {
            DWORD scrollValue = std::stol(value);
            actions.emplace_back(new MouseScrollAction(scrollValue));
        }
    }
    file.close();
    return actions;
}
