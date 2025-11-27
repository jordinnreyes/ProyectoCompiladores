.data
print_fmt: .string "%ld \n"
printf_fmt_float: .string "%f \n"
n: .long 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movl $5, %eax
 movl %eax, n(%rip)
while_0:
 movl n(%rip), %eax
 pushq %rax
 movl $0, %eax
 movl %eax, %ecx
 popq %rax
 cmpl %ecx, %eax
 movl $0, %eax
 setg %al
 movzbq %al, %rax
 cmpq $0, %rax
 je endwhile_0
 movl n(%rip), %eax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl n(%rip), %eax
 pushq %rax
 movl $1, %eax
 movl %eax, %ecx
 popq %rax
 subl %ecx, %eax
 movl %eax, n(%rip)
 jmp while_0
endwhile_0:
.end_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits
