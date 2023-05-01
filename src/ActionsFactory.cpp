#include <iostream>
#include <fstream>
#include "Actions.h"
#include "ActionsFactory.h"

std::vector<struct Action*> ActionsFactory::CreateActions(const std::string& filename)
{
    Lines lines = LoadLines(filename);
    Actions actions = ProcessLines(lines);
    return actions;
}

Lines ActionsFactory::LoadLines(const std::string& filename)
{
    Lines lines;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return lines;
    }

    std::string line;
    while (getline(file, line))
    {
        // remove comment
        size_t comment = line.find(';');
        if (comment != std::string::npos)
            line = line.substr(0, comment);

        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

        if (line.empty())
            continue;

        lines.emplace_back(line);
    }
    file.close();

    return lines;
}

int ActionsFactory::loopCount = 0;
int ActionsFactory::loopBeginIdx = 0;
Actions ActionsFactory::ProcessLines(Lines& lines)
{
    Actions actions;
    for (size_t i = 0; i < lines.size(); ++i)
    {
        std::string line = lines[i];

        if (line == "end" && loopCount > 1)
        {
            --loopCount;
            i = loopBeginIdx;
            line = lines[i];
        }

        size_t find = lines[i].find(':');
        if (find == std::string::npos)
            continue;

        std::string key = line.substr(0, find);
        std::string value = line.substr(find + 1);

        if (key == "loop")
        {   
            loopBeginIdx = i + 1;
            loopCount = std::stoi(value);
        }
        else if (key == ActionTypeName.writeAction)
        {
            std::wstring chars(value.begin(), value.end());
            for (const auto& c : chars)
            {
                actions.emplace_back(new WriteAction(c));
            }
        }
        else if (key == ActionTypeName.pressAction)
        {
            actions.emplace_back(new PressKeyAction(value));
        }
        else if (key == ActionTypeName.holdAction)
        {
            actions.emplace_back(new HoldKeyAction(value));
        }
        else if (key == ActionTypeName.releaseAction)
        {
            actions.emplace_back(new ReleaseKeyAction(value));
        }
        else if (key == ActionTypeName.sleepAction)
        {
            DWORD ms = std::stoul(value);
            actions.emplace_back(new SleepAction(ms));
        }
        else if (key == ActionTypeName.setCursorPosAction)
        {
            find = value.find(' ');
            int x = std::stoi(value.substr(0, find));
            int y = std::stoi(value.substr(find + 1));
            actions.emplace_back(new SetCursorPosAction(x, y));
        }
        else if (key == ActionTypeName.moveCursorAction)
        {
            find = value.find(' ');
            int x = std::stoi(value.substr(0, find));
            int y = std::stoi(value.substr(find + 1));
            actions.emplace_back(new MoveCursorAction(x, y));
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
    return actions;
}