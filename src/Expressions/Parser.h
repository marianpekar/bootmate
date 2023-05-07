#pragma once

#include "Node.h"
#include <vector>
#include <string>

class Parser {
public:
    Parser(const std::vector<std::string>& tokens) : tokens(tokens), pos(0) {}
    Node* Parse();
private:
    Node* ParseLogical();
    Node* ParseEquality();
    Node* ParseSubAdd();
    Node* ParseDivMul();
    Node* ParseExp();
    Node* ParseMod();
    Node* ParseUnary();
    Node* ParseBrackets();
    std::vector<std::string> tokens;
    int pos;
};