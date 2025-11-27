.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
a: .long 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movl $14, %eax
 movl %eax, a(%rip)
 movl a(%rip), %eax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
