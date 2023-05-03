#include "Node.h"
#include <stdexcept>

double OperatorNode::Evaluate() const
{
    switch (op)
    {
        case '+': return left->Evaluate() + right->Evaluate();
        case '-': return left->Evaluate() - right->Evaluate();
        case '*': return left->Evaluate() * right->Evaluate();
        case '/': return left->Evaluate() / right->Evaluate();
        case '%': return fmod(left->Evaluate(), right->Evaluate());
        case '^': return pow(left->Evaluate(), right->Evaluate());
    }

    return 0.0;
}

OperatorNode::~OperatorNode()
{
     delete left;
     delete right;
}
