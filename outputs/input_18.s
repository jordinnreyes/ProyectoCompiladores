.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
b: .long 0
a: .long 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movl $20, %eax
 movl %eax, a(%rip)
 movl a(%rip), %eax
 pushq %rax
 movl $0, %eax
 movl %eax, %ecx
 popq %rax
 addl %ecx, %eax
 movl %eax, b(%rip)
 movl a(%rip), %eax
 pushq %rax
 movl $20, %eax
 movl %eax, %ecx
 popq %rax
 subl %ecx, %eax
 cmpq $0, %rax
 je else_0
 movl $999, %eax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 jmp endif_0
else_0:
endif_0:
while_1:
 movl a(%rip), %eax
 pushq %rax
 movl $20, %eax
 movl %eax, %ecx
 popq %rax
 subl %ecx, %eax
 cmpq $0, %rax
 je endwhile_1
 movl a(%rip), %eax
 pushq %rax
 movl $1, %eax
 movl %eax, %ecx
 popq %rax
 addl %ecx, %eax
 movl %eax, a(%rip)
 jmp while_1
endwhile_1:
 movl b(%rip), %eax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
