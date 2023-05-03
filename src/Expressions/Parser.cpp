#include "Parser.h"
#include <stdexcept>

Node* Parser::Parse() 
{
    Node* left = ParseSubAdd();
    return left;
}

Node* Parser::ParseSubAdd() 
{
    Node* left = ParseDivMul();
    while (pos < tokens.size()) 
    {
        char op = tokens[pos++][0];
        if (op == '+' || op == '-') 
        {
            Node* right = ParseDivMul();
            left = new OperatorNode(op, left, right);
        }
        else 
        {
            pos--;
            break;
        }
    }
    return left;
}

Node* Parser::ParseDivMul() 
{
    Node* left = ParseExp();
    while (pos < tokens.size()) 
    {
        char op = tokens[pos++][0];
        if (op == '*' || op == '/') 
        {
            Node* right = ParseExp();
            left = new OperatorNode(op, left, right);
        }
        else 
        {
            pos--;
            break;
        }
    }
    return left;
}

Node* Parser::ParseExp() 
{
    Node* left = ParseMod();
    while (pos < tokens.size()) 
    {
        char op = tokens[pos++][0];
        if (op == '^') {
            Node* right = ParseMod();
            left = new OperatorNode(op, left, right);
        }
        else {
            pos--;
            break;
        }
    }
    return left;
}

Node* Parser::ParseMod() 
{
    Node* left = ParseUnary();
    while (pos < tokens.size()) 
    {
        char op = tokens[pos++][0];
        if (op == '%') {
            Node* right = ParseUnary();
            left = new OperatorNode(op, left, right);
        }
        else 
        {
            pos--;
            break;
        }
    }
    return left;
}

Node* Parser::ParseUnary() 
{
    std::string token = tokens[pos++];
    if (token == "-") 
    {
        Node* node = ParseBrackets();
        return new OperatorNode(token[0], new OperandNode(0), node);
    }
    else if (token == "(")
    {
        Node* node = Parse();
        if (tokens[pos++] != ")") 
        {
            throw std::invalid_argument("Missing closing bracket");
        }
        return node;
    }
    else 
    {
        double value = 0.0;
        try 
        {
            value = std::stod(token);
        }
        catch (const std::invalid_argument&) 
        {
            throw std::invalid_argument("Invalid token: " + token);
        }
        return new OperandNode(value);
    }
}

Node* Parser::ParseBrackets() 
{
    std::string token = tokens[pos++];
    if (token == "(") 
    {
        Node* node = Parse();
        if (pos >= tokens.size() || tokens[pos++] != ")") 
        {
            throw std::invalid_argument("Missing closing bracket");
        }
        return node;
    }
    else 
    {
        double value = 0.0;
        try 
        {
            value = std::stod(token);
        }
        catch (const std::invalid_argument&) 
        {
            throw std::invalid_argument("Invalid token: " + token);
        }
        return new OperandNode(value);
    }
}