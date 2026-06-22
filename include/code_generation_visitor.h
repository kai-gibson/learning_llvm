#ifndef CODE_GENERATION_VISITOR_H
#define CODE_GENERATION_VISITOR_H

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include "ast.h"
#include "visitor.h"

class CodegenVisitor : public Visitor {
 public:
  CodegenVisitor();
  void finalise();
  void visit(BinaryExpression& expr) override;
  void visit(FloatLiteralExpression& expr) override;
  void visit(VariableExpression& expr) override;
  void visit(Program& expr) override;
  void visit(VariableDeclarationStatement& stmt) override;
  void visit(VariableAssignmentStatement& stmt) override;

  llvm::Value* emit(ASTNode& expr);
  void compile();

  std::unique_ptr<llvm::LLVMContext> llvm_context;
  std::unique_ptr<llvm::Module> llvm_module;
  std::unique_ptr<llvm::IRBuilder<>> llvm_builder;

  std::unordered_map<std::string, llvm::Value*> named_values;

 private:
  llvm::Value* result;
  llvm::Function* current_function;
};

#endif  // CODE_GENERATION_VISITOR_H
