#ifndef PRINT_VISITOR_H
#define PRINT_VISITOR_H
#include <iostream>

#include "ast.h"
#include "visitor.h"

struct PrintVisitor : public Visitor {
  void visit(FloatLiteralExpression& expr) override {
    std::cout << "FloatLiteral(" << expr.value << ")";
  }

  void visit(BinaryExpression& expr) override {
    std::cout << "BinaryExpression(" << token_type_to_str(expr.op) << ", Lhs(";
    expr.lhs->accept(*this);
    std::cout << "), Rhs(";
    expr.rhs->accept(*this);
    std::cout << ")\n";
  }

  void visit(VariableExpression& expr) override {
    std::cout << "VariableExpression(" << expr.name << ")";
  }

  void visit(Program& program) override {
    std::cout << "Program(";
    for (const auto& statement : program.statements) {
      statement->accept(*this);
    }

    std::cout << ")";
  }
};

#endif  // PRINT_VISITOR_H
