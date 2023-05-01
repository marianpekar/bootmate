#include <iostream>
#include <fstream>
#include "ConfigLoader.h"

Dict ConfigLoader::ReadIniFile(const std::string filename)
{
    Dict values;
    std::ifstream file(filename);

    if (file.is_open())
    {
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

            size_t equals = line.find('=');
            if (equals != std::string::npos)
            {
                std::string key = line.substr(0, equals);
                std::string value = line.substr(equals + 1);
                values[key] = value;
            }
        }

        file.close();
    }
#if _DEBUG
    else
    {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
#endif

    return values;
}

bool ConfigLoader::HasElement(Dict ini, const std::string key)
{
    return ini.find(key) != ini.end();

}
