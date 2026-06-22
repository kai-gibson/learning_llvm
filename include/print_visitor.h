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

  void visit(VariableDeclarationStatement& stmt) override {
    std::cout << "VariableDeclarationStatement(\n\t\tName(\"" << stmt.name
              << "\"),\n\t\tValue(";
    stmt.value->accept(*this);

    std::cout << ")";
  }

  void visit(VariableAssignmentStatement& stmt) override {
    std::cout << "VariableAssignmentStatement(\n\t\tName(\"" << stmt.name
              << "\"),\n\t\tValue(";
    stmt.value->accept(*this);

    std::cout << ")";
  }

  void visit(Program& program) override {
    std::cout << "Program(\n";
    for (const auto& statement : program.statements) {
      std::cout << "\t";
      statement->accept(*this);
      std::cout << ",\n";
    }

    std::cout << ")";
  }

  void visit(ShowStatement& stmt) override {
    std::cout << "Show(\n";
    stmt.expr->accept(*this);
    std::cout << ")";
  }
};

#endif  // PRINT_VISITOR_H
