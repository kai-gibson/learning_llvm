# Compiler for the B2 Language

After 2 years of working on the specification for the language [void](https://github.com/kai-gibson/void_compiler) I mocked up a working example of its compiler, working with claude - but I felt too constrained by making the perfect decisions to progress it. B2 is a toy language I've decided to write in order to break that habit - and, importantly, I'm not using AI to write code in this project to learn the depths of compiler design and LLVM.

The rule for B2 is to make the first decision that comes to mind, and not worry about designing perfectly from the beginning.

## Language
```b2
func b()
  x = 1234
  show x + 1
  show x + 2

  return 10
end

func main() 
  x = 123  
  y = x + 12

  show y

  show b()
  return 1
end
```

B2 is a block-end style language, similar to lua. Variables are declared like `var_name = 123`, and are mutated with the `set` statement - taken from the language [vale](https://verdagon.dev/blog/linear-types-borrowing).

Using `set` allows for the simplest possible declaration syntax - a subtle nudge to the developer to prefer immutability.

## Design
The compiler has a few stages implemented. First the `Lexer` class turns the `*.b` text file into a stream of tokens like `[Identifier("x"), Assignment("="), IntLiteral(123)]`

Then the parser iterates through the tokens generating AST nodes in recursive descent style, which results in something like: 

```
(Program
    (FunctionDeclaration "main"
        (VariableDeclarationStatement "x"
            (BinaryExpression Plus
                (IntLiteralExpression 1
                (IntLiteralExpression 2
        (Show
            (VariableExpression "x"
        (ReturnStatement
            (VariableExpression "x"))))
```

Operator precedence is implemented using [Pratt parsing](https://matklad.github.io/2020/04/13/simple-but-powerful-pratt-parsing.html) with reference to [C++ operator precedence](https://en.cppreference.com/cpp/language/operator_precedence).

After AST generation there's a few passes implemented with the visitor pattern. First we
traverse the tree attempting to resolve types in the `TypeCheckVisitor`. This works its
way through the AST attempting to infer types and validate that there's no misuse,
resulting in an annotated tree: 
```
(Program
    (FunctionDeclaration "main"
        (ResolvedType Int32)
        (VariableDeclarationStatement "x"
            (ResolvedType Int32)
            (BinaryExpression Plus
                (ResolvedType Int32)
                (IntLiteralExpression 1
                    (ResolvedType Int32))
                (IntLiteralExpression 2
                    (ResolvedType Int32))))
        (Show
            (VariableExpression "x"
                (ResolvedType Int32)))
        (ReturnStatement
            (VariableExpression "x"
                (ResolvedType Int32)))))
```

There's no IR for B2 currently - I don't have a need as it currently stands. From here I just lower this tree directly to LLVM IR with the `CodegenVisitor`.

Codegen simply visits the annotated AST and calls LLVM API's to generate the required IR - like this:
```cpp
  auto* func_type =
      llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvm_context), false);
  auto* func =
      llvm::Function::Create(func_type, llvm::Function::ExternalLinkage,
                             func_declaration.name, *llvm_module);
  auto* entry = llvm::BasicBlock::Create(*llvm_context, "entry", func);
  llvm_builder->SetInsertPoint(entry);

  for (auto& statement : func_declaration.statements) result = emit(*statement);
```

For the AST above that could look like:
```llvm
; ModuleID = 'my_module'
source_filename = "my_module"

; currently using null terminated strings - I'll write my own printf once I've implemented slice types for strings...
@fmt = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1 

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 3, ptr %x, align 4 ; Int literals default to Int32
  %x1 = load i32, ptr %x, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @fmt, i32 %x1)
  %x2 = load i32, ptr %x, align 4
  ret i32 %x2 ; main implictly returns int - something to be fixed later
}
```

## Fun notes
### Compile time maps
With some C++ template magic I was able to make a general purpose map type with `find`
and `contains` methods. The exciting part of this is that since many C++23 ranges
functions are now `constexpr`, this construct totally disappears when it's possible to
resolve at compile time.

This:
```cpp
constexpr auto KEYWORDS = make_static_map<std::string_view, std::string_view>({
    {"end", "TokenType::End"},
    {"if", "TokenType::If"},
});

int main(int argc, char** argv) {
    return KEYWORDS.find("end")->size();
}
```

Emits this assembly:
```asm
"main":
        mov     eax, 14
        ret
```

### RAII based AST-to-s-expression marshalling
Due to difficulty to read the AST and maintain a print visitor for the AST, I made a
small helper called `PrettyPrinter` to print the AST in a lisp style syntax - the fun
part is I can't forget to add a closing brace since the helper returns an object whose
destructor closes the parens:

```cpp
struct Closer {
    PrettyPrinter& printer;

    ~Closer() { printer.close_node(); }
  };

  auto add_node(std::string_view name) -> Closer {
    if (depth) {
      output += std::format("\n{}", std::string(depth * 4, ' '));
    }

    output += std::format("({}", name);
    depth += 1;

    return Closer{*this};
  }
```

### Testing
Testing uses GTest - the layers of the compiler are fairly well separated, so unit testing them isn't particularly difficult. For the parser I was desperate to avoid snapshot style testing, it's far too brittle and annoying to keep up to date.

Instead I made a series of helper macros and functions to be able to unit test directly on the resulting AST.
```cpp
TEST(ParserTest, ParsesDivision) {
  PARSE_EXPR(expr, "x = 10 / 20");

  auto assign = cast_node<VariableDeclarationStatement>(expr);

  auto div = cast_node<BinaryExpression>(assign->value);
  ASSERT_EQ(div->op, TokenType::ForwardSlash);
  auto left = cast_node<IntLiteralExpression>(div->lhs);
  auto right = cast_node<IntLiteralExpression>(div->rhs);

  ASSERT_EQ(assign->name, "x");
  EXPECT_EQ(left->value, 10);
  EXPECT_EQ(right->value, 20);
}
```
