.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
resultado: .long 0
.text
.globl max
max:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi, -8(%rbp)
 movq %rsi, -16(%rbp)
 subq $16, %rsp
 movl -8(%rbp), %eax
 pushq %rax
 movl -16(%rbp), %eax
 movl %eax, %ecx
 popq %rax
 cmpl %ecx, %eax
 movl $0, %eax
 setg %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_0
 movl -8(%rbp), %eax
 jmp endif_0
else_0:
 movl -16(%rbp), %eax
endif_0:
.end_max:
 leave
 ret
.globl doble
doble:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi, -8(%rbp)
 subq $16, %rsp
 movl -8(%rbp), %eax
 pushq %rax
 movl $2, %eax
 movl %eax, %ecx
 popq %rax
 imull %ecx, %eax
.end_doble:
 leave
 ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movl $10, %eax
 movq %rax, -16(%rbp)
 movl $7, %eax
 movq %rax, -24(%rbp)
 movq -16(%rbp), %rdi
 movq -24(%rbp), %rsi
 call max
 movq %rax, -32(%rbp)
 movq -32(%rbp), %rdi
 call doble
 movl %eax, resultado(%rip)
 movl resultado(%rip), %eax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
