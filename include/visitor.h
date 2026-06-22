#ifndef VISITOR_H
#define VISITOR_H

#include "ast.h"

struct Visitor {
  virtual void visit(FloatLiteralExpression& expr) = 0;
  virtual void visit(BinaryExpression& expr) = 0;
  virtual void visit(VariableExpression& expr) = 0;
  virtual void visit(Program& expr) = 0;
  virtual void visit(VariableDeclarationStatement& expr) = 0;
  virtual void visit(VariableAssignmentStatement& expr) = 0;
};

#endif  // VISITOR_H
