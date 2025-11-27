.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
r: .long 0
.text
.globl suma
suma:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi, -8(%rbp)
 movq %rsi, -16(%rbp)
 subq $16, %rsp
 movl -8(%rbp), %eax
 pushq %rax
 movl -16(%rbp), %eax
 pushq %rax
 movl $2, %eax
 movl %eax, %ecx
 popq %rax
 imull %ecx, %eax
 movl %eax, %ecx
 popq %rax
 addl %ecx, %eax
.end_suma:
 leave
 ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movl $4, %eax
 movq %rax, -16(%rbp)
 movl $6, %eax
 movq %rax, -24(%rbp)
 movq -16(%rbp), %rdi
 movq -24(%rbp), %rsi
 call suma
 movl %eax, r(%rip)
 movl r(%rip), %eax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
