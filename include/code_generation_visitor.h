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

struct CodegenVisitor : public Visitor {
  std::unique_ptr<llvm::LLVMContext> llvm_context;
  std::unique_ptr<llvm::Module> llvm_module;
  std::unique_ptr<llvm::IRBuilder<>> llvm_builder;

  CodegenVisitor() {
    llvm_context = std::make_unique<llvm::LLVMContext>();
    llvm_module = std::make_unique<llvm::Module>("my_module", *llvm_context);
    llvm_builder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);

    auto* printf_type = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*llvm_context),
        {llvm::PointerType::get(*llvm_context, 0)}, true);

    llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage,
                           "printf", *llvm_module);

    auto* main_type =
        llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvm_context), false);
    auto* main_fn = llvm::Function::Create(
        main_type, llvm::Function::ExternalLinkage, "main", *llvm_module);

    auto* entry = llvm::BasicBlock::Create(*llvm_context, "entry", main_fn);
    llvm_builder->SetInsertPoint(entry);
  }

  llvm::Value* result;

  void finalise() {
    auto* fmt = llvm_builder->CreateGlobalStringPtr("%.2f\n", "fmt");
    auto* printf_fn = llvm_module->getFunction("printf");
    llvm_builder->CreateCall(printf_fn->getFunctionType(), printf_fn,
                             {fmt, result});
    llvm_builder->CreateRet(
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvm_context), 0));
  }

  void visit(BinaryExpression& expr) override {
    expr.lhs->accept(*this);
    auto l = result;
    expr.rhs->accept(*this);
    auto r = result;

    if (!l || !r)
      throw std::runtime_error("codegen failed on BinaryExpression");

    switch (expr.op) {
      case TokenType::Plus:
        result = llvm_builder->CreateFAdd(l, r, "addtmp");
        break;
      case TokenType::Minus:
        result = llvm_builder->CreateFSub(l, r, "subtmp");
        break;
      case TokenType::Asterisk:
        result = llvm_builder->CreateFMul(l, r, "multmp");
        break;
      case TokenType::ForwardSlash:
        result = llvm_builder->CreateFDiv(l, r, "divtmp");
        break;
      default:
        throw std::runtime_error("Unsupported binary operation");
    }
  }

  void visit(FloatLiteralExpression& expr) override {
    result = llvm::ConstantFP::get(*llvm_context, llvm::APFloat(expr.value));
  }
};

#endif  // CODE_GENERATION_VISITOR_H
