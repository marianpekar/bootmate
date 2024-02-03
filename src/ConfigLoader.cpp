#include <iostream>
#include <fstream>
#include "ConfigLoader.h"

Dict ConfigLoader::ini;

void ConfigLoader::LoadIniFile(const std::string& filename)
{
    if (std::ifstream file(filename); file.is_open())
    {
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

            const size_t equals = line.find('=');
            if (equals != std::string::npos)
            {
                std::string key = line.substr(0, equals);
                const std::string value = line.substr(equals + 1);
                ini[key] = value;
            }
        }

        file.close();
    }
#if _DEBUG
    else
    {
        std::cerr << "Unable to open file: " << filename << '\n';
    }
#endif
}

bool ConfigLoader::HasElement(const std::string& key)
{
    return ini.contains(key);
}
