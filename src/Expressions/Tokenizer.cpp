#include "Tokenizer.h"
#include <unordered_set>

Tokens Tokenizer::Tokenize(const std::string& expr)
{
    Tokens tokens;
    std::string token;
    const std::unordered_set<std::string> operators = { "==", "!=", "<=", ">=", "<", ">", "+", "-", "*", "/", "(", ")", "%", "^", "&&", "||" };

    for (size_t i = 0; i < expr.size();)
    {
        char c = expr[i];
        if (isspace(c))
        {
            i++;
        }
        else
        {
            bool is_keyword = false;
            for (size_t len = 1; len <= (expr.size() - i); len++)
            {
                std::string subexpr = expr.substr(i, len);
                if (operators.count(subexpr) > 0)
                {
                    if (!token.empty())
                    {
                        tokens.push_back(token);
                        token.clear();
                    }
                    if (subexpr == "<" || subexpr == ">")
                    {
                        if (i < expr.size() - 1 && expr[i + 1] == '=')
                        {
                            subexpr += "=";
                            len++;
                        }
                    }
                    tokens.push_back(subexpr);
                    i += len;
                    is_keyword = true;
                    break;
                }
            }
            if (!is_keyword)
            {
                token.push_back(c);
                i++;
            }
        }
    }
    if (!token.empty())
    {
        tokens.push_back(token);
    }

    return tokens;
}



