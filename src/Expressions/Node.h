#pragma once

#include <string>

struct Node
{
    const char* type;
    Node(const char* type) : type(type) {}
    virtual double Evaluate() const { return 0.0; }
    virtual bool EvaluateBool() const { return false; }
    virtual ~Node() {}
};

class OperandNode : public Node
{
public: 
    OperandNode(double value) : value(value), Node("double") {}
    virtual double Evaluate() const override { return value; }
private:
    double value;
};

class OperatorNode : public Node 
{
public:
    OperatorNode(std::string op, Node* left, Node* right) : op(op), left(left), right(right), Node("double") {}
    virtual double Evaluate() const override;
    virtual ~OperatorNode();
private:
    std::string op;
    Node* left;
    Node* right;
};

class EqualityNode : public Node 
{
public:
    EqualityNode(std::string op, Node* left, Node* right) : op(op), left(left), right(right), Node("bool") {}
    virtual bool EvaluateBool() const override;
    virtual ~EqualityNode();
private:
    std::string op;
    Node* left;
    Node* right;
};