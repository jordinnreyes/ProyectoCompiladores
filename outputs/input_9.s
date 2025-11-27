.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
c: .long 0
b: .float 0.0
a: .long 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movl $10, %eax
 movl %eax, a(%rip)
 movl a(%rip), %eax
 cvtsi2ss %rax, %xmm0
 movss %xmm0, -12(%rbp)
 movss ._CF0(%rip), %xmm0
 movss %xmm0, -16(%rbp)
 movss -12(%rbp), %xmm1
 movss -16(%rbp), %xmm0
 addss %xmm0, %xmm1
 movss %xmm1, %xmm0
 movss %xmm0, b(%rip)
 movl a(%rip), %eax
 pushq %rax
 movl $20, %eax
 movl %eax, %ecx
 popq %rax
 addl %ecx, %eax
 movl %eax, c(%rip)
 movss b(%rip), %xmm0
 cvtss2sd %xmm0, %xmm0
 leaq printf_fmt_float(%rip), %rdi
 movl $1, %eax
 call printf@PLT
 movl c(%rip), %eax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
 leave
 ret

# Constantes de punto flotante (float 32 bits)
._CF0: .float 2.5
.section .note.GNU-stack,"",@progbits
