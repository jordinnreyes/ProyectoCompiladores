.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
x: .float 0.0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movss ._CF0(%rip), %xmm0
 movss %xmm0, x(%rip)
 movss x(%rip), %xmm0
 cvtss2sd %xmm0, %xmm0
 leaq printf_fmt_float(%rip), %rdi
 movl $1, %eax
 call printf@PLT
.end_main:
 leave
 ret

# Constantes de punto flotante (float 32 bits)
._CF0: .float 3
.section .note.GNU-stack,"",@progbits
