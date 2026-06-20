declare i32 @printf(ptr, ...)

@fmt = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@sfmt = private unnamed_addr constant [4 x i8] c"%s\0A\00"

%Slice = type {ptr, i32}

define void @print_string(ptr %slice) {
entry:
  %len = getelementptr %Slice, ptr %slice, i32 0, i32 0
  %val = load ptr, ptr %len
  %1 = call i32 (ptr, ...) @printf(ptr @sfmt, ptr %val) 

  ret void
}

@index_fmt = private unnamed_addr constant [4 x i8] c"%d,\00"
@newline  = private unnamed_addr constant [2 x i8] c"\0A\00"

define void @print_i32_array(ptr %slice, i32 %len) {
entry:
  %arr = getelementptr %Slice, ptr %slice, i32 0, i32 0
  br label %loop.cond

loop.cond:
  %i = phi i32 [0, %entry], [%next_i, %loop.body]

  %cond = icmp slt i32 %i, %len
  br i1 %cond, label %loop.body, label %loop.end
  
loop.body:
  %elem_ptr = getelementptr i32, ptr %arr, i32 %i 
  %elem = load i32, ptr %elem_ptr

  call i32 (ptr, ...) @printf(ptr @index_fmt, i32 %elem)

  %next_i = add i32 %i, 1
  br label %loop.cond

loop.end:
  call i32 (ptr, ...) @printf(ptr @newline)
  ret void
}

@name = private constant [4 x i8] c"Kai\00"

define i32 @main() {
entry:
  %arr = alloca [4 x i32]
  %elem = getelementptr [4 x i32], ptr %arr, i32 0
  store i32 10, ptr %elem
  %0 = load i32, ptr %elem

  %1 = call i32 (ptr, ...) @printf(ptr @fmt, i32 %0) 

  %s = alloca %Slice
  %arr_ptr = getelementptr %Slice, ptr %s, i32 0, i32 0
  %str_ptr = getelementptr [3 x i8], ptr @name, i32 0, i32 0
  store ptr %str_ptr, ptr %arr_ptr
  
  %len_ptr = getelementptr %Slice, ptr %s, i32 0, i32 1
  store i32 3, ptr %len_ptr
  
  call void @print_string(ptr %s)

  %a = alloca i32, i32 10
  %first = getelementptr i32, ptr %a, i32 0
  store i32 10, ptr %first

  %second = getelementptr i32, ptr %a, i32 1
  store i32 20, ptr %second

  %2 = load i32, ptr %first
  %3 = load i32, ptr %second

  %4 = call i32 (ptr, ...) @printf(ptr @fmt, i32 %2) 
  %5 = call i32 (ptr, ...) @printf(ptr @fmt, i32 %3) 


  call void @print_i32_array(ptr %a, i32 2)

  ret i32 1
}
