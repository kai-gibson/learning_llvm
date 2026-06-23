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

struct VariableAssignmentStatement : public ASTNode {
  VariableAssignmentStatement(std::string name, std::unique_ptr<ASTNode> value)
      : name(std::move(name)), value(std::move(value)) {}

  std::string name;
  std::unique_ptr<ASTNode> value;
  void accept(Visitor& v) override;
};

struct VariableDeclarationStatement : public ASTNode {
  VariableDeclarationStatement(std::string name, std::unique_ptr<ASTNode> value,
                               std::unique_ptr<ASTNode> type = nullptr)
      : name(std::move(name)), value(std::move(value)), type(std::move(type)) {}

  std::string name;
  std::unique_ptr<ASTNode> value;
  std::unique_ptr<ASTNode> type;
  void accept(Visitor& v) override;
};

struct ShowStatement : public ASTNode {
  ShowStatement(std::unique_ptr<ASTNode> expr) : expr(std::move(expr)) {}

  std::unique_ptr<ASTNode> expr;
  void accept(Visitor& v) override;
};

struct FunctionDeclaration : public ASTNode {
  FunctionDeclaration(std::string name) : name(std::move(name)) {}

  std::string name;
  std::vector<std::unique_ptr<ASTNode>> statements;

  void accept(Visitor& v) override;
};

struct FunctionCallExpression : public ASTNode {
  FunctionCallExpression(std::string name) : name(std::move(name)) {}

  std::string name;

  void accept(Visitor& v) override;
};

struct Program : public ASTNode {
  std::vector<std::unique_ptr<ASTNode>> functions;

  void accept(Visitor& v) override;
};

struct ReturnStatement : public ASTNode {
  ReturnStatement(std::unique_ptr<ASTNode> value) : value(std::move(value)) {}

  std::unique_ptr<ASTNode> value;

  void accept(Visitor& v) override;
};

struct TypeExpression : public ASTNode {
  TypeExpression(std::string name) : name(std::move(name)) {}

  std::string name;

  void accept(Visitor& v) override;
};
#endif  // AST_H
