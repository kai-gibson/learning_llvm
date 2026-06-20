#include "code_generation_visitor.h"

#include "compiler.h"

CodegenVisitor::CodegenVisitor() {
  llvm_context = std::make_unique<llvm::LLVMContext>();
  llvm_module = std::make_unique<llvm::Module>("my_module", *llvm_context);
  llvm_builder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);

  auto* printf_type =
      llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvm_context),
                              {llvm::PointerType::get(*llvm_context, 0)}, true);

  llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage, "printf",
                         *llvm_module);

  auto* main_type =
      llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvm_context), false);
  auto* main_fn = llvm::Function::Create(
      main_type, llvm::Function::ExternalLinkage, "main", *llvm_module);

  auto* entry = llvm::BasicBlock::Create(*llvm_context, "entry", main_fn);
  llvm_builder->SetInsertPoint(entry);
}

llvm::Value* CodegenVisitor::emit(ExpressionNode& expr) {
  expr.accept(*this);
  return result;
}

void CodegenVisitor::finalise() {
  auto* fmt = llvm_builder->CreateGlobalStringPtr("%.2f\n", "fmt");
  auto* printf_fn = llvm_module->getFunction("printf");
  llvm_builder->CreateCall(printf_fn->getFunctionType(), printf_fn,
                           {fmt, result});
  llvm_builder->CreateRet(
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvm_context), 0));
}

void CodegenVisitor::compile() {
  emit_object_file(*llvm_module, "output.o");
  link("output.o", "output");
}

void CodegenVisitor::visit(BinaryExpression& expr) {
  auto l = emit(*expr.lhs);
  auto r = emit(*expr.rhs);

  if (!l || !r) throw std::runtime_error("codegen failed on BinaryExpression");

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

void CodegenVisitor::visit(FloatLiteralExpression& expr) {
  result = llvm::ConstantFP::get(*llvm_context, llvm::APFloat(expr.value));
}
