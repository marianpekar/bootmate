#pragma once

#include <vector>
#include <string>

typedef std::vector<struct Action*> Actions;
typedef std::vector<std::string> Lines;
typedef std::vector<std::string> BuildInVariables;
typedef std::map<std::string,std::string> Variables;

class ActionsFactory
{
public:
	static Actions CreateActions(const std::string& filename);
private:
	static int loopCount;
	static size_t loopBeginIdx;
	static size_t ifIdx;
	static size_t elseIfIdx;
	static size_t elseIdx;
	static size_t endIfIdx;
	static bool skipToEndIf;
	static Variables vars;
	static BuildInVariables chronoVars;
	static Lines LoadLines(const std::string& filename);
	static Actions ProcessLines(Lines& lines);
	static void BranchLines(size_t& i, Lines& lines, const std::string inValue);
	static void EvaluateExpressions(std::string& value);
};