#include "ast.h"

#include "visitor.h"

inline void FloatLiteralExpression::accept(Visitor& v) { v.visit(*this); }
inline void BinaryExpression::accept(Visitor& v) { v.visit(*this); }
inline void VariableExpression::accept(Visitor& v) { v.visit(*this); }
inline void Program::accept(Visitor& v) { v.visit(*this); }
inline void VariableDeclarationStatement::accept(Visitor& v) { v.visit(*this); }
