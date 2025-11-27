.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
.text
.globl calcular
calcular:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi, -8(%rbp)
 movss %xmm0, -16(%rbp)
 movq %rsi, -24(%rbp)
 subq $32, %rsp
 movq -8(%rbp), %rax
 cvtsi2ss %rax, %xmm0
 movss %xmm0, -36(%rbp)
 movss -16(%rbp), %xmm0
 movss %xmm0, -40(%rbp)
 movss -36(%rbp), %xmm1
 movss -40(%rbp), %xmm0
 addss %xmm0, %xmm1
 movss %xmm1, %xmm0
 movss %xmm0, -44(%rbp)
 movl -24(%rbp), %eax
 cvtsi2ss %rax, %xmm0
 movss %xmm0, -48(%rbp)
 movss -44(%rbp), %xmm1
 movss -48(%rbp), %xmm0
 addss %xmm0, %xmm1
 movss %xmm1, %xmm0
.end_calcular:
 leave
 ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $1000, %rax
 movq %rax, -16(%rbp)
 movss ._CF0(%rip), %xmm0
 movss %xmm0, -20(%rbp)
 movl $10, %eax
 movl %eax, -24(%rbp)
 movq -16(%rbp), %rax
 movq %rax, -32(%rbp)
 movss -20(%rbp), %xmm0
 movss %xmm0, -40(%rbp)
 movl -24(%rbp), %eax
 movq %rax, -48(%rbp)
 movq -32(%rbp), %rdi
 movss -40(%rbp), %xmm0
 movq -48(%rbp), %rsi
 call calcular
 cvtss2sd %xmm0, %xmm0
 leaq printf_fmt_float(%rip), %rdi
 movl $1, %eax
 call printf@PLT
.end_main:
 leave
 ret

# Constantes de punto flotante (float 32 bits)
._CF0: .float 2.5
.section .note.GNU-stack,"",@progbits
