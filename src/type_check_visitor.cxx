#include "type_check_visitor.h"

#include <iostream>

void TypeCheckVisitor::visit(BinaryExpression& expr) {
  auto lhs = emit(*expr.lhs);
  auto rhs = emit(*expr.rhs);

  std::cout << (int)lhs.type_id << " " << (int)rhs.type_id << "\n";
  if (lhs.type_id != rhs.type_id) {
    throw std::runtime_error(std::format(
        "Type Error: left ({}) and right ({}) of binary operation: {} do not "
        "match",
        lhs.identifier, rhs.identifier, token_type_to_str(expr.op)));
  }

  result = Type{.type_id = rhs.type_id, .identifier = rhs.identifier};
  expr.resolved_type = result;
}

void TypeCheckVisitor::visit(FloatLiteralExpression& expr) {
  result = Type{.type_id = TypeId::Float64, .identifier = "Float64"};
  expr.resolved_type = result;
}

void TypeCheckVisitor::visit(IntLiteralExpression& expr) {
  result = Type{.type_id = TypeId::Int32, .identifier = "Int32"};
  expr.resolved_type = result;
}

void TypeCheckVisitor::visit(VariableExpression& expr) {
  auto it = variable_map.find(expr.name);
  if (it == variable_map.end()) {
    throw std::runtime_error(
        std::format("Error: variable: {} not declared", expr.name));
  }

  result = it->second;
  expr.resolved_type = result;
}

void TypeCheckVisitor::visit(Program& program) {
  for (const auto& func : program.functions) {
    func->accept(*this);
  }
}

void TypeCheckVisitor::visit(VariableDeclarationStatement& stmt) {
  std::optional<Type> expected_type;

  if (stmt.type_identifier) {
    expected_type = emit(*stmt.type_identifier);
  }

  auto value = emit(*stmt.value);

  if (expected_type && expected_type->type_id != value.type_id) {
    throw std::runtime_error(
        std::format("Type Error: variable: {} with type: {} does not match "
                    "expression type: {}",
                    stmt.name, expected_type->identifier, value.identifier));
  }

  // else just infer type from expression
  result = value;
  stmt.resolved_type = result;

  // save to symbol map
  variable_map[stmt.name] = result;
}

void TypeCheckVisitor::visit(VariableAssignmentStatement& stmt) {
  auto it = variable_map.find(stmt.name);
  if (it == variable_map.end()) {
    throw std::runtime_error(
        std::format("Error: variable: {} not declared", stmt.name));
  }

  auto var_type = it->second;
  auto expr_type = emit(*stmt.value);

  if (var_type.type_id != expr_type.type_id) {
    throw std::runtime_error(
        std::format("Type Error: cannot assign type \"{}\" to variable \"{}\" "
                    "of type \"{}\"",
                    expr_type.identifier, stmt.name, var_type.identifier));
  }
}

// no-op
void TypeCheckVisitor::visit(ShowStatement& stmt) { (void)stmt; }

void TypeCheckVisitor::visit(FunctionDeclaration& func) {
  // scope variable type map to function
  variable_map.clear();
  for (const auto& stmt : func.statements) {
    stmt->accept(*this);
  }
}

void TypeCheckVisitor::visit(FunctionCallExpression& stmt) { (void)stmt; }

void TypeCheckVisitor::visit(ReturnStatement& stmt) {
  auto value = emit(*stmt.value);
  result = value;
  stmt.resolved_type = result;
}

void TypeCheckVisitor::visit(TypeExpression& expr) {
  auto type_id = get_type_id(expr.name);
  result = Type{.type_id = type_id, .identifier = expr.name};
}
