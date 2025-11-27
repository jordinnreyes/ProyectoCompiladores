.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
c: .long 0
b: .long 0
a: .long 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movl $2, %eax
 movl %eax, a(%rip)
 movl $3, %eax
 movl %eax, b(%rip)
 movl a(%rip), %eax
 pushq %rax
 movl b(%rip), %eax
 movl %eax, %ecx
 popq %rax
 imull %ecx, %eax
 pushq %rax
 movl $5, %eax
 movl %eax, %ecx
 popq %rax
 addl %ecx, %eax
 movl %eax, c(%rip)
 movl c(%rip), %eax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
