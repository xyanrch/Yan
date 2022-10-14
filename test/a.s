	.LC10:
		.string "%d\n"
	.text
	.global print
	print:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq %rdi, -8(%rbp)
	movq -8(%rbp), %rax
	movq %rax, %rsi
	leaq .LC10(%rip), %rdi
	movq $0,%rax
	call printf@PLT
	nop
	leave
	ret
	.LC11:
		.string "%s"
	.text
	.global printstr
	printstr:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq %rdi, -8(%rbp)
	movq -8(%rbp), %rax
	movq %rax, %rsi
	leaq .LC11(%rip), %rdi
	movq $0,%rax
	call printf@PLT
	nop
	leave
	ret
	.global main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $8, %rsp
	movq $2, %r10
	movl %r10d, -4(%rbp)
	movl -4(%rbp), %r10d
	cmp $1, %r10
	jz case_lable_0
	jmp case_lable_1
case_lable_0:
	leaq  -8(%rbp) , %r10
	movq $1, %r11
	movl %r11d, (%r10)
	jmp .L1
	movl -4(%rbp), %r10d
	cmp $2, %r10
	jz case_lable_1
	jmp case_lable_2
case_lable_1:
	leaq  -8(%rbp) , %r10
	movq $2, %r11
	movl %r11d, (%r10)
	jmp .L1
	movl -4(%rbp), %r10d
	cmp $0, %r10
	jz case_lable_2
	jmp .L1
case_lable_2:
	leaq  -8(%rbp) , %r10
	movq $88, %r11
	movl %r11d, (%r10)
.L1:
	movl -8(%rbp), %r10d
	movq %r10, %rdi
	movq $0,%rax
	   call print
	movq  %rax, %r10
	addq $8, %rsp
	leave
	ret
