#pragma once

#include <map>
#include <string>

typedef std::map<std::string, std::string> Dict;
struct ConfigLoader
{
	static Dict ini;
	static void LoadIniFile(const std::string filename);
	static bool HasElement(const std::string key);
};