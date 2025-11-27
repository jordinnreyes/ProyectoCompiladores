.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
b: .long 0
a: .long 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movl $7, %eax
 movl %eax, a(%rip)
 movl a(%rip), %eax
 pushq %rax
 movl $3, %eax
 movl %eax, %ecx
 popq %rax
 imull %ecx, %eax
 movl %eax, b(%rip)
 movl b(%rip), %eax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
