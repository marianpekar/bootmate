#include "Tokenizer.h"

Tokens Tokenizer::Tokenize(const std::string& expr)
{
    Tokens tokens;
    std::string token;

    for (char c : expr) 
    {
        if (c == '+' || c == '/' || c == '*' || c == '-' || c == '(' || c == ')' || c == '%' || c == '^')
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(std::string(1, c));
        }
        else
        {
            token.push_back(c);
        }
    }
    if (!token.empty())
    {
        tokens.push_back(token);
    }

    return tokens;
}
