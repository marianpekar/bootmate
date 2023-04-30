#pragma once

#include <map>
#include <string>

typedef std::map<std::string, std::string> Dict;
struct ConfigLoader
{
	static Dict ReadIniFile(const std::string filename);
};