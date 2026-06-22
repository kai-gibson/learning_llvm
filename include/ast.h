#ifndef AST_H
#define AST_H

#include "lexer.h"

// forward declaration
struct Visitor;

class ExpressionNode {
 public:
  virtual ~ExpressionNode() = default;
  virtual void accept(Visitor& v) = 0;
};

struct FloatLiteralExpression : public ExpressionNode {
  FloatLiteralExpression(double value) : value(value) {}
  double value;

  void accept(Visitor& v) override;
};

struct BinaryExpression : public ExpressionNode {
  BinaryExpression(TokenType op, std::unique_ptr<ExpressionNode> lhs,
                   std::unique_ptr<ExpressionNode> rhs)
      : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
  TokenType op;
  std::unique_ptr<ExpressionNode> lhs, rhs;

  void accept(Visitor& v) override;
};

struct VariableExpression : public ExpressionNode {
  VariableExpression(std::string name) : name(std::move(name)) {}

  std::string name;
  void accept(Visitor& v) override;
};

struct Program : public ExpressionNode {
  std::vector<std::unique_ptr<ExpressionNode>> statements;

  void accept(Visitor& v) override;
};

#endif  // AST_H
