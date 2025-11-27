.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
x: .long 0
.text
.globl factorial
factorial:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi, -8(%rbp)
 subq $20, %rsp
 movl -8(%rbp), %eax
 pushq %rax
 movl $2, %eax
 movl %eax, %ecx
 popq %rax
 cmpl %ecx, %eax
 movl $0, %eax
 setl %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_0
 movl $1, %eax
 jmp endif_0
else_0:
 movl -8(%rbp), %eax
 pushq %rax
 movl -8(%rbp), %eax
 pushq %rax
 movl $1, %eax
 movl %eax, %ecx
 popq %rax
 subl %ecx, %eax
 movq %rax, -28(%rbp)
 movq -28(%rbp), %rdi
 call factorial
 movl %eax, %ecx
 popq %rax
 imull %ecx, %eax
endif_0:
.end_factorial:
 leave
 ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movl $5, %eax
 movl %eax, x(%rip)
 movl x(%rip), %eax
 movq %rax, -16(%rbp)
 movq -16(%rbp), %rdi
 call factorial
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
