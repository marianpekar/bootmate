#pragma once

struct Node
{
    virtual double Evaluate() const = 0;
    virtual ~Node() {}
};

class OperandNode : public Node
{
public:
    OperandNode(double value) : value(value) {}
    virtual double Evaluate() const override { return value; }
private:
    double value;
};

class OperatorNode : public Node {
public:
    OperatorNode(char op, Node* left, Node* right) : op(op), left(left), right(right) {}
    virtual double Evaluate() const override;
    virtual ~OperatorNode();
private:
    char op;
    Node* left;
    Node* right;
};