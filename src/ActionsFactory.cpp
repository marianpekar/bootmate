#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <chrono>
#include <iomanip>
#include "Actions.h"
#include "ActionsFactory.h"
#include "Expressions/Parser.h"
#include "Expressions/Tokenizer.h"

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
        size_t comment = line.find(';');
        while (comment != std::string::npos)
        {
            if (comment > 0 && line[comment - 1] == '`')
            {
                line.replace(comment - 1, 2, ";");
            }
            else
            {
                line = line.substr(0, comment);
                break;
            }
            comment = line.find(';', comment + 1);
        }

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
size_t ActionsFactory::loopBeginIdx = 0;
Variables ActionsFactory::vars;
BuildInVariables ActionsFactory::chronoVars = { "weekday", "day", "month", "year", "hour", "minute", "second" };
Actions ActionsFactory::ProcessLines(Lines& lines)
{
    Actions actions;
    for (size_t i = 0; i < lines.size(); ++i)
    {
        if (skipToEndIf && (i == elseIdx || i == elseIfIdx))
        {
            i = endIfIdx;
            skipToEndIf = false;
            continue;
        }

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

        for (auto& var : vars)
        {
            size_t varFind = 0;
            while ((varFind = value.find("$" + var.first, varFind)) != std::string::npos)
            {
                value.replace(varFind, std::string("$" + var.first).length(), var.second);
                varFind += var.second.length();
            }
        }

        for (auto& var : chronoVars)
        {
            size_t varFind = 0;
            while ((varFind = value.find("$" + var, varFind)) != std::string::npos)
            {
                auto now = std::chrono::system_clock::now();
                std::string result;
                if (var == "weekday")
                {
                    auto weekday = std::chrono::weekday(std::chrono::floor<std::chrono::days>(now));
                    result = std::to_string(static_cast<int>(weekday.c_encoding()));
                }
                else if (var == "day")
                {
                    auto ymd = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(now));
                    result = std::to_string(static_cast<unsigned>(ymd.day()));
                }
                else if (var == "month")
                {
                    auto month = std::chrono::duration_cast<std::chrono::months>(now.time_since_epoch()).count() % 12 + 1;
                    result = std::to_string(month);
                }
                else if (var == "year")
                {
                    auto year = std::chrono::duration_cast<std::chrono::years>(now.time_since_epoch()).count();
                    result = std::to_string(year + 1970);
                }
                else if (var == "hour")
                {
                    auto hour = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch()).count() % 24;
                    result = std::to_string(hour);
                }
                else if (var == "minute")
                {
                    auto minute = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch()).count() % 60;
                    result = std::to_string(minute);
                }
                else if (var == "second")
                {
                    auto second = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() % 60;
                    result = std::to_string(second);
                }

                value.replace(varFind, std::string("$" + var).length(), result);
                varFind += result.length();
            }
        }

        EvaluateExpressions(value);

        for (auto& var : vars)
        {
            if (key == var.first)
            {
                vars[key] = value;
            }
        }

        if (key == "var")
        {
            size_t findEqual = value.find(' ');
            if (findEqual != std::string::npos)
            {
                std::string varName = value.substr(0, findEqual);
                std::string varValue = value.substr(findEqual + 1);
                vars[varName] = varValue;
            }
            else
            {
                vars[value] = "";
            }
        }

        if (key == "loop")
        {
            loopBeginIdx = i + 1;
            loopCount = std::stoi(value);
        }
        if (key == "if" || key == "else if")
        {
            BranchLines(i, lines, value);
        }
        else if (key == ActionTypeName.write)
        {
            actions.emplace_back(new WriteAction(value));
        }
        else if (key == ActionTypeName.press)
        {
            actions.emplace_back(new PressKeyAction(value));
        }
        else if (key == ActionTypeName.hold)
        {
            actions.emplace_back(new HoldKeyAction(value));
        }
        else if (key == ActionTypeName.release)
        {
            actions.emplace_back(new ReleaseKeyAction(value));
        }
        else if (key == ActionTypeName.sleep)
        {
            DWORD ms = std::stoul(value);
            actions.emplace_back(new SleepAction(ms));
        }
        else if (key == ActionTypeName.setCursorPos)
        {
            find = value.find(' ');
            int x = std::stoi(value.substr(0, find));
            int y = std::stoi(value.substr(find + 1));
            actions.emplace_back(new SetCursorPosAction(x, y));
        }
        else if (key == ActionTypeName.moveCursor)
        {
            find = value.find(' ');
            int x = std::stoi(value.substr(0, find));
            int y = std::stoi(value.substr(find + 1));
            actions.emplace_back(new MoveCursorAction(x, y));
        }
        else if (key == ActionTypeName.mouseClick)
        {
            actions.emplace_back(new MouseClickAction(value));
        }
        else if (key == ActionTypeName.mouseScroll)
        {
            DWORD scrollValue = std::stol(value);
            actions.emplace_back(new MouseScrollAction(scrollValue));
        }
        else if (key == ActionTypeName.holdMouseButton)
        {
            actions.emplace_back(new HoldMouseButtonAction(value));
        }
        else if (key == ActionTypeName.releaseMouseButton)
        {
            actions.emplace_back(new ReleaseMouseButtonAction(value));
        }
        else if (key == ActionTypeName.run)
        {
            std::string exe, args;
            if (value[0] == '\"') 
            {
                size_t end_quote = value.find('\"', 1);
                exe = value.substr(1, end_quote - 1);
                args = value.substr(end_quote + 1);
            }
            else 
            {
                size_t find = value.find(" ");
                exe = value.substr(0, find);
                args = value.substr(find + 1);
            }

            std::filesystem::path exePath(exe);
            if (!exePath.is_absolute()) 
            {
                std::filesystem::path cwd = std::filesystem::current_path();
                exePath = cwd / exePath;
                exe = exePath.string();
            }

            actions.emplace_back(new RunAction(exe, args));
        }
    }
    return actions;
}

void ActionsFactory::EvaluateExpressions(std::string& value)
{
    std::string leftTag = "${";
    std::string rightTag = "}";
    size_t startPos = 0;
    while ((startPos = value.find(leftTag, startPos)) != std::string::npos) 
    {
        startPos += leftTag.size();
        size_t endPos = value.find(rightTag, startPos);
        std::string expr = value.substr(startPos, endPos - startPos);

        Tokens tokens = Tokenizer::Tokenize(expr);
        Parser parser(tokens);
        Node* node = parser.Parse();
        std::ostringstream oss;
        if (node->type == "double")
        {
            double result = node->Evaluate();
            oss << std::setprecision(8) << std::noshowpoint << result;
        }
        else if (node->type == "bool")
        {
            bool result = node->EvaluateBool();
            if (result == true)
            {
                oss << "true";
            }
            else
            {
                oss << "false";
            }
        }

        value.replace(startPos - leftTag.size(), endPos + rightTag.size() - startPos + leftTag.size(), oss.str());
        startPos = startPos - leftTag.size() + oss.str().size() + rightTag.size();
    }
}

size_t ActionsFactory::ifIdx = 0;
size_t ActionsFactory::elseIfIdx = 0;
size_t ActionsFactory::elseIdx = 0;
size_t ActionsFactory::endIfIdx = 0;
bool ActionsFactory::skipToEndIf = false;
void ActionsFactory::BranchLines(size_t& i, Lines& lines, const std::string inValue)
{
    size_t current = i;
    ifIdx = current;
    std::string key;
    std::string value;
    elseIdx = -1;
    elseIfIdx = -1;
    endIfIdx = lines.size() - 1;
    while (key != "end if" || current > lines.size())
    {
        current++;
        size_t find = lines[current].find(':');
        key = lines[current].substr(0, find);
        value = lines[current].substr(find + 1);
        if (key == "else if")
        {
            elseIfIdx = current;
        }
        if (key == "else")
        {
            elseIdx = current;
        }
        else if (key == "end if")
        {
            endIfIdx = current;
            break;
        }
    }

    if (inValue == "true")
    {
        skipToEndIf = true;
    }
    else
    {
        i = (elseIfIdx != - 1 ? elseIfIdx : elseIdx) - 1;    
    }
}