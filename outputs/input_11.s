.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
r: .float 0.0
u: .long 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movl $50, %eax
 movl %eax, u(%rip)
 movl u(%rip), %eax
 cvtsi2ss %rax, %xmm0
 movss %xmm0, -12(%rbp)
 movl $3, %eax
 cvtsi2ss %rax, %xmm0
 movss %xmm0, -16(%rbp)
 movss -12(%rbp), %xmm1
 movss -16(%rbp), %xmm0
 divss %xmm0, %xmm1
 movss %xmm1, %xmm0
 movss %xmm0, r(%rip)
 movss r(%rip), %xmm0
 cvtss2sd %xmm0, %xmm0
 leaq printf_fmt_float(%rip), %rdi
 movl $1, %eax
 call printf@PLT
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
