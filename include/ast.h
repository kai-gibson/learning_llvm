#ifndef AST_H
#define AST_H

#include "lexer.h"

// forward declaration
struct Visitor;

class ASTNode {
 public:
  virtual ~ASTNode() = default;
  virtual void accept(Visitor& v) = 0;
};

struct FloatLiteralExpression : public ASTNode {
  FloatLiteralExpression(double value) : value(value) {}
  double value;

  void accept(Visitor& v) override;
};

struct BinaryExpression : public ASTNode {
  BinaryExpression(TokenType op, std::unique_ptr<ASTNode> lhs,
                   std::unique_ptr<ASTNode> rhs)
      : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
  TokenType op;
  std::unique_ptr<ASTNode> lhs, rhs;

  void accept(Visitor& v) override;
};

struct VariableExpression : public ASTNode {
  VariableExpression(std::string name) : name(std::move(name)) {}

  std::string name;
  void accept(Visitor& v) override;
};

struct Program : public ASTNode {
  std::vector<std::unique_ptr<ASTNode>> statements;

  void accept(Visitor& v) override;
};

#endif  // AST_H
