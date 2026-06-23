#ifndef PRINT_VISITOR_H
#define PRINT_VISITOR_H
#include <iostream>
#include <string>

#include "ast.h"
#include "visitor.h"

struct PrintVisitor : public Visitor {
  std::size_t indent = 0;

  std::string pad() { return std::string(indent * 2, ' '); }

  void print_resolved_type(ASTNode& node) {
    if (node.resolved_type.has_value()) {
      std::cout << ", ResolvedType(" << node.resolved_type->identifier << ")";
    }
  }

  void visit(FloatLiteralExpression& expr) override {
    std::cout << "FloatLiteral(" << expr.value;
    print_resolved_type(expr);
    std::cout << ")";
  }

  void visit(IntLiteralExpression& expr) override {
    std::cout << "IntLiteralExpression(" << expr.value;
    print_resolved_type(expr);
    std::cout << ")";
  }

  void visit(BinaryExpression& expr) override {
    std::cout << "BinaryExpression(" << token_type_to_str(expr.op);
    print_resolved_type(expr);
    std::cout << "\n";
    indent++;
    std::cout << pad() << "Lhs(";
    expr.lhs->accept(*this);
    std::cout << ")\n";
    std::cout << pad() << "Rhs(";
    expr.rhs->accept(*this);
    std::cout << ")\n";
    indent--;
    std::cout << pad() << ")";
  }

  void visit(VariableExpression& expr) override {
    std::cout << "VariableExpression(" << expr.name;
    print_resolved_type(expr);
    std::cout << ")";
  }

  void visit(VariableDeclarationStatement& stmt) override {
    std::cout << "VariableDeclarationStatement(\"" << stmt.name << "\"";
    print_resolved_type(stmt);
    std::cout << "\n";
    indent++;
    std::cout << pad() << "Value(";
    stmt.value->accept(*this);
    std::cout << ")";

    if (stmt.type_identifier) {
      std::cout << "\n" << pad() << "Type(";
      stmt.type_identifier->accept(*this);
      std::cout << ")";
    }

    indent--;
    std::cout << ")";
  }

  void visit(VariableAssignmentStatement& stmt) override {
    std::cout << "VarAssign(\"" << stmt.name << "\"\n";
    indent++;
    std::cout << pad() << "Value(";
    stmt.value->accept(*this);
    std::cout << ")";
    indent--;
    std::cout << ")";
  }

  void visit(ShowStatement& stmt) override {
    std::cout << "Show(";
    stmt.expr->accept(*this);
    std::cout << ")";
  }

  void visit(FunctionDeclaration& func) override {
    std::cout << "FunctionDeclaration(\n";
    indent++;
    std::cout << pad() << "Name(\"" << func.name << "\")";
    print_resolved_type(func);
    std::cout << "\n";
    for (const auto& stmt : func.statements) {
      std::cout << pad();
      stmt->accept(*this);
      std::cout << "\n";
    }
    indent--;
    std::cout << pad() << ")";
  }

  void visit(FunctionCallExpression& call) override {
    std::cout << "FunctionCall(\"" << call.name << "\"";
    print_resolved_type(call);
    std::cout << ")";
  }

  void visit(Program& program) override {
    std::cout << "Program(\n";
    indent++;
    for (const auto& func : program.functions) {
      std::cout << pad();
      func->accept(*this);
      std::cout << "\n";
    }
    indent--;
    std::cout << ")\n";
  }

  void visit(ReturnStatement& ret) override {
    std::cout << "ReturnStatement(\n";
    indent++;
    ret.value->accept(*this);
    indent--;
    std::cout << ")\n";
  }

  void visit(TypeExpression& ret) override {
    std::cout << "TypeExpression(\n";
    indent++;
    std::cout << "Name(\"" << ret.name << "\")";
    indent--;
    std::cout << ")\n";
  }
};

#endif  // PRINT_VISITOR_H
