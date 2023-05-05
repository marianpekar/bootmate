#include "Node.h"
#include <stdexcept>

double OperatorNode::Evaluate() const
{
    if (op == "+")
    {
        return left->Evaluate() + right->Evaluate();
    }
    else if (op == "-")
    {
        return left->Evaluate() - right->Evaluate();
    }
    else if (op == "*")
    {
        return left->Evaluate() * right->Evaluate();
    }
    else if (op == "/")
    {
        return left->Evaluate() / right->Evaluate();
    }
    else if (op == "%")
    {
        return fmod(left->Evaluate(), right->Evaluate());
    }
    else if (op == "^")
    {
        return pow(left->Evaluate(), right->Evaluate());
    }

    return 0.0;
}

bool EqualityNode::EvaluateBool() const
{
    if (op == "==")
    {
        return left->Evaluate() == right->Evaluate();
    }
    else if (op == "!=")
    {
        return left->Evaluate() != right->Evaluate();
    }
    else if (op == ">")
    {
        return left->Evaluate() > right->Evaluate();
    }
    else if (op == "<")
    {
        return left->Evaluate() < right->Evaluate();
    }
    else if (op == "<=")
    {
        return left->Evaluate() <= right->Evaluate();
    }
    else if (op == ">=")
    {
        return left->Evaluate() >= right->Evaluate();
    }

    return false;
}

OperatorNode::~OperatorNode()
{
     delete left;
     delete right;
}

EqualityNode::~EqualityNode()
{
    delete left;
    delete right;
}
