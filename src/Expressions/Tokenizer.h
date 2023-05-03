#pragma once

#include <vector>
#include <string>

typedef std::vector<std::string> Tokens;

struct Tokenizer
{
	static Tokens Tokenize(const std::string& expr);
};