#ifndef OPERATOR_MAP_H
#define OPERATOR_MAP_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include "ast.h"

class TypeOperations {
 public:
  virtual ~TypeOperations() = default;
  virtual llvm::Value* add(llvm::Value* lhs, llvm::Value* rhs) = 0;
  virtual llvm::Value* subtract(llvm::Value* lhs, llvm::Value* rhs) = 0;
  virtual llvm::Value* multiply(llvm::Value* lhs, llvm::Value* rhs) = 0;
  virtual llvm::Value* divide(llvm::Value* lhs, llvm::Value* rhs) = 0;
  virtual llvm::Value* gt(llvm::Value* lhs, llvm::Value* rhs) = 0;
  virtual llvm::Value* lt(llvm::Value* lhs, llvm::Value* rhs) = 0;
  virtual llvm::Value* gte(llvm::Value* lhs, llvm::Value* rhs) = 0;
  virtual llvm::Value* lte(llvm::Value* lhs, llvm::Value* rhs) = 0;

  llvm::Value* apply(TokenType token_type, llvm::Value* lhs, llvm::Value* rhs);
};

class IntOperations : public TypeOperations {
 public:
  IntOperations(llvm::IRBuilder<>& builder, bool is_signed = true)
      : builder(builder), is_signed(is_signed) {}
  llvm::Value* add(llvm::Value* lhs, llvm::Value* rhs) override;
  llvm::Value* subtract(llvm::Value* lhs, llvm::Value* rhs) override;
  llvm::Value* multiply(llvm::Value* lhs, llvm::Value* rhs) override;
  llvm::Value* divide(llvm::Value* lhs, llvm::Value* rhs) override;
  llvm::Value* gt(llvm::Value* lhs, llvm::Value* rhs) override;
  llvm::Value* lt(llvm::Value* lhs, llvm::Value* rhs) override;
  llvm::Value* gte(llvm::Value* lhs, llvm::Value* rhs) override;
  llvm::Value* lte(llvm::Value* lhs, llvm::Value* rhs) override;

 private:
  llvm::IRBuilder<>& builder;
  bool is_signed;
};

class FloatOperations : public TypeOperations {
 public:
  FloatOperations(llvm::IRBuilder<>& builder) : builder(builder) {}
  llvm::Value* add(llvm::Value* lhs, llvm::Value* rhs) override;
  llvm::Value* subtract(llvm::Value* lhs, llvm::Value* rhs) override;
  llvm::Value* multiply(llvm::Value* lhs, llvm::Value* rhs) override;
  llvm::Value* divide(llvm::Value* lhs, llvm::Value* rhs) override;
  llvm::Value* gt(llvm::Value* lhs, llvm::Value* rhs) override;
  llvm::Value* lt(llvm::Value* lhs, llvm::Value* rhs) override;
  llvm::Value* gte(llvm::Value* lhs, llvm::Value* rhs) override;
  llvm::Value* lte(llvm::Value* lhs, llvm::Value* rhs) override;

 private:
  llvm::IRBuilder<>& builder;
};

std::unique_ptr<TypeOperations> build_type_operations(
    Type type, llvm::IRBuilder<>& builder);

#endif  // OPERATOR_MAP_H
