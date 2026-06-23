#include "code_generation_visitor.h"

#include "compiler.h"
#include "operation_map.h"

CodegenVisitor::CodegenVisitor() {
  llvm_context = std::make_unique<llvm::LLVMContext>();
  llvm_module = std::make_unique<llvm::Module>("my_module", *llvm_context);
  llvm_builder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);

  auto* printf_type =
      llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvm_context),
                              {llvm::PointerType::get(*llvm_context, 0)}, true);

  llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage, "printf",
                         *llvm_module);
}

llvm::Value* CodegenVisitor::emit(ASTNode& expr) {
  expr.accept(*this);
  return result;
}

llvm::Type* CodegenVisitor::get_llvm_type(Type& type) {
  switch (type.type_id) {
    case TypeId::Bool:
      return llvm::Type::getInt1Ty(*llvm_context);
    case TypeId::Int8:
      return llvm::Type::getInt8Ty(*llvm_context);
    case TypeId::Int16:
      return llvm::Type::getInt16Ty(*llvm_context);
    case TypeId::Int32:
      return llvm::Type::getInt32Ty(*llvm_context);
    case TypeId::Int64:
      return llvm::Type::getInt64Ty(*llvm_context);
    case TypeId::UInt8:
      return llvm::Type::getInt8Ty(*llvm_context);
    case TypeId::UInt16:
      return llvm::Type::getInt16Ty(*llvm_context);
    case TypeId::UInt32:
      return llvm::Type::getInt32Ty(*llvm_context);
    case TypeId::UInt64:
      return llvm::Type::getInt64Ty(*llvm_context);
    case TypeId::Float32:
      return llvm::Type::getFloatTy(*llvm_context);
    case TypeId::Float64:
      return llvm::Type::getDoubleTy(*llvm_context);
    case TypeId::String:
    case TypeId::UserDefined:
      throw std::runtime_error(
          std::format("Datatype {} not implemented", type.identifier));
  }
}

void CodegenVisitor::finalise() {}

void CodegenVisitor::compile() {
  emit_object_file(*llvm_module, "output.o");
  link("output.o", "output");
}

void CodegenVisitor::visit(VariableDeclarationStatement& stmt) {
  auto* exists = named_values[stmt.name];
  if (exists) {
    throw std::runtime_error(
        "Cannot declare same variable twice; use `set x = 1` to modify "
        "value");
  }

  auto r = emit(*stmt.value);

  if (!stmt.resolved_type) {
    throw std::runtime_error("Unable to resolve variable type");
  }

  auto type = get_llvm_type(*stmt.resolved_type);
  auto* alloca = llvm_builder->CreateAlloca(type, nullptr, stmt.name);
  llvm_builder->CreateStore(r, alloca);
  named_values[stmt.name] = alloca;
  result = r;
  return;
}

void CodegenVisitor::visit(VariableAssignmentStatement& stmt) {
  auto* variable = named_values[stmt.name];
  if (!variable) {
    throw std::runtime_error(
        std::format("Cannot assign to non-existent variable: {}", stmt.name));
  }

  auto r = emit(*stmt.value);

  llvm_builder->CreateStore(r, variable);
  result = r;
  return;
}

void CodegenVisitor::visit(BinaryExpression& expr) {
  auto l = emit(*expr.lhs);
  auto r = emit(*expr.rhs);

  if (!l || !r) throw std::runtime_error("codegen failed on BinaryExpression");

  if (!expr.resolved_type) {
    throw std::runtime_error("Couldn't resolve type for binary expression");
  }

  auto builder = build_type_operations(*expr.resolved_type, *llvm_builder);
  result = builder->apply(expr.op, l, r);
}

void CodegenVisitor::visit(FloatLiteralExpression& expr) {
  result = llvm::ConstantFP::get(*llvm_context, llvm::APFloat(expr.value));
}

void CodegenVisitor::visit(IntLiteralExpression& expr) {
  result =
      llvm::ConstantInt::get(*llvm_context, llvm::APInt(32, expr.value, true));
}

void CodegenVisitor::visit(VariableExpression& expr) {
  auto* alloca = named_values[expr.name];
  if (!alloca)
    throw std::runtime_error(
        std::format("Unknown variable name: {}", expr.name));

  if (!expr.resolved_type) {
    throw std::runtime_error("Unable to resolve variable expression type");
  }

  auto type = get_llvm_type(*expr.resolved_type);
  result = llvm_builder->CreateLoad(type, alloca, expr.name);
}

void CodegenVisitor::visit(Program& expr) {
  for (auto& func : expr.functions) {
    result = emit(*func);
  }
}

void CodegenVisitor::visit(ShowStatement& stmt) {
  auto* fmt = llvm_builder->CreateGlobalStringPtr("%.2f\n", "fmt");

  auto* printf_fn = llvm_module->getFunction("printf");

  result = emit(*stmt.expr);
  llvm_builder->CreateCall(printf_fn->getFunctionType(), printf_fn,
                           {fmt, result});
}

void CodegenVisitor::visit(FunctionDeclaration& func_declaration) {
  named_values.clear();

  auto* func_type =
      llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvm_context), false);
  auto* func =
      llvm::Function::Create(func_type, llvm::Function::ExternalLinkage,
                             func_declaration.name, *llvm_module);
  auto* entry = llvm::BasicBlock::Create(*llvm_context, "entry", func);
  llvm_builder->SetInsertPoint(entry);

  for (auto& statement : func_declaration.statements) result = emit(*statement);

  // llvm_builder->CreateRet(
  //     llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvm_context), 0));
}

void CodegenVisitor::visit(ReturnStatement& ret) {
  auto value = emit(*ret.value);

  llvm_builder->CreateRet(value);
}

void CodegenVisitor::visit(TypeExpression& ret) {
  // noop
}

void CodegenVisitor::visit(FunctionCallExpression& funccall) {
  auto* fn = llvm_module->getFunction(funccall.name);
  if (!fn)
    throw std::runtime_error(
        std::format("Function {} does not exist", funccall.name));

  llvm_builder->CreateCall(fn->getFunctionType(), fn, {});
}
