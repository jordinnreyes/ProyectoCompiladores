.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
x: .long 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movl $0, %eax
 movl %eax, x(%rip)
 movl $1, %eax
 cmpq $0, %rax
 je else_0
 movl x(%rip), %eax
 pushq %rax
 movl $10, %eax
 movl %eax, %ecx
 popq %rax
 addl %ecx, %eax
 movl %eax, x(%rip)
 jmp endif_0
else_0:
endif_0:
 movl x(%rip), %eax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
