#pragma once

#include <vector>
#include <string>

struct ActionsFactory
{
	static std::vector<struct Action*> CreateActions(const std::string filename);
};