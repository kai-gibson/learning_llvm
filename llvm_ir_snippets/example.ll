declare i32 @printf(ptr, ...)


@fmt = private unnamed_addr constant [6 x i8] c"%.2f\0A\00"
define i32 @main() {
entry:
  %0 = alloca i32 
  %res = fadd double 1.0, 12.2
  %1 = call i32 (ptr, ...) @printf(ptr @fmt, double %res)

  ret i32 2
}
