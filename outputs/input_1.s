.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
x: .long 0
.text
.globl foo
foo:
 pushq %rbp
 movq %rsp, %rbp
 movl $8, %eax
.end_foo:
 leave
 ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 call foo
 movl %eax, x(%rip)
 movl x(%rip), %eax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
