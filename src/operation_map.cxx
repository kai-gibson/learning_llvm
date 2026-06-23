#include "operation_map.h"

std::unique_ptr<TypeOperations> build_type_operations(
    Type type, llvm::IRBuilder<>& builder) {
  switch (type.type_id) {
    case TypeId::Int8:
    case TypeId::Int16:
    case TypeId::Int32:
    case TypeId::Int64:
      return std::make_unique<IntOperations>(builder, true);

    case TypeId::UInt8:
    case TypeId::UInt16:
    case TypeId::UInt32:
    case TypeId::UInt64:
      return std::make_unique<IntOperations>(builder, false);

    case TypeId::Float32:
    case TypeId::Float64:
      return std::make_unique<FloatOperations>(builder);

    case TypeId::Bool:
    case TypeId::String:
    case TypeId::UserDefined:
      throw std::runtime_error(std::format(
          "Datatype {} operations not implemented", type.identifier));
  }
}

llvm::Value* TypeOperations::apply(TokenType token_type, llvm::Value* lhs,
                                   llvm::Value* rhs) {
  switch (token_type) {
    case TokenType::Plus:
      return add(lhs, rhs);
    case TokenType::Minus:
      return subtract(lhs, rhs);
    case TokenType::Asterisk:
      return multiply(lhs, rhs);
    case TokenType::ForwardSlash:
      return divide(lhs, rhs);
    case TokenType::GreaterThan:
      return gt(lhs, rhs);
    case TokenType::LessThan:
      return lt(lhs, rhs);
    case TokenType::GreaterThanEquals:
      return gte(lhs, rhs);
    case TokenType::LessThanEquals:
      return lte(lhs, rhs);
    default:
      throw std::runtime_error(std::format("Unsupported operation: {}",
                                           token_type_to_str(token_type)));
  }
}

llvm::Value* IntOperations::add(llvm::Value* lhs, llvm::Value* rhs) {
  return builder.CreateAdd(lhs, rhs, "addtmp");
}

llvm::Value* IntOperations::subtract(llvm::Value* lhs, llvm::Value* rhs) {
  return builder.CreateSub(lhs, rhs, "subtmp");
}

llvm::Value* IntOperations::multiply(llvm::Value* lhs, llvm::Value* rhs) {
  return builder.CreateMul(lhs, rhs, "multmp");
}

llvm::Value* IntOperations::divide(llvm::Value* lhs, llvm::Value* rhs) {
  return is_signed ? builder.CreateSDiv(lhs, rhs, "divtmp")
                   : builder.CreateUDiv(lhs, rhs, "divtmp");
}

llvm::Value* IntOperations::gt(llvm::Value* lhs, llvm::Value* rhs) {
  return is_signed ? builder.CreateICmpSGT(lhs, rhs, "gttmp")
                   : builder.CreateICmpUGT(lhs, rhs, "gttmp");
}
llvm::Value* IntOperations::lt(llvm::Value* lhs, llvm::Value* rhs) {
  return is_signed ? builder.CreateICmpSLT(lhs, rhs, "lttmp")
                   : builder.CreateICmpULT(lhs, rhs, "lttmp");
}
llvm::Value* IntOperations::gte(llvm::Value* lhs, llvm::Value* rhs) {
  return is_signed ? builder.CreateICmpSGE(lhs, rhs, "gtetmp")
                   : builder.CreateICmpUGE(lhs, rhs, "gtetmp");
}
llvm::Value* IntOperations::lte(llvm::Value* lhs, llvm::Value* rhs) {
  return is_signed ? builder.CreateICmpSLE(lhs, rhs, "gtetmp")
                   : builder.CreateICmpULE(lhs, rhs, "gtetmp");
}

llvm::Value* FloatOperations::add(llvm::Value* lhs, llvm::Value* rhs) {
  return builder.CreateFAdd(lhs, rhs, "faddtmp");
}

llvm::Value* FloatOperations::subtract(llvm::Value* lhs, llvm::Value* rhs) {
  return builder.CreateFSub(lhs, rhs, "fsubtmp");
}

llvm::Value* FloatOperations::multiply(llvm::Value* lhs, llvm::Value* rhs) {
  return builder.CreateFMul(lhs, rhs, "fmultmp");
}

llvm::Value* FloatOperations::divide(llvm::Value* lhs, llvm::Value* rhs) {
  return builder.CreateFDiv(lhs, rhs, "fdivtmp");
}

llvm::Value* FloatOperations::gt(llvm::Value* lhs, llvm::Value* rhs) {
  return builder.CreateFCmpOGT(lhs, rhs, "fgttmp");
}

llvm::Value* FloatOperations::lt(llvm::Value* lhs, llvm::Value* rhs) {
  return builder.CreateFCmpOLT(lhs, rhs, "flttmp");
}

llvm::Value* FloatOperations::gte(llvm::Value* lhs, llvm::Value* rhs) {
  return builder.CreateFCmpOGE(lhs, rhs, "fgetmp");
}

llvm::Value* FloatOperations::lte(llvm::Value* lhs, llvm::Value* rhs) {
  return builder.CreateFCmpOLE(lhs, rhs, "fletmp");
}
