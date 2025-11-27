.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
edad: .long 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movl $16, %eax
 movl %eax, edad(%rip)
 movl edad(%rip), %eax
 pushq %rax
 movl $18, %eax
 movl %eax, %ecx
 popq %rax
 cmpl %ecx, %eax
 movl $0, %eax
 setge %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_0
 movl $1, %eax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 jmp endif_0
else_0:
 movl $0, %eax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
endif_0:
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
