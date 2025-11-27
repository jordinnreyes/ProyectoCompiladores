.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
n: .long 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movl $10, %eax
 movl %eax, n(%rip)
 movl n(%rip), %eax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
