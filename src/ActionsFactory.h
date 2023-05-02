#pragma once

#include <vector>
#include <string>

typedef std::vector<struct Action*> Actions;
typedef std::vector<std::string> Lines;
typedef std::map<std::string,std::string> Variables;

class ActionsFactory
{
public:
	static Actions CreateActions(const std::string& filename);
private:
	static int loopCount;
	static int loopBeginIdx;
	static Variables vars;
	static Lines LoadLines(const std::string& filename);
	static Actions ProcessLines(Lines& lines);
};