	.LC0:
		.string "Q "
	.LC1:
		.string ". "
	.LC2:
		.string "\n"
	.LC3:
		.string "\n\n"
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
	.global printmsg
printmsg:
	pushq %rbp
	movq %rsp, %rbp
	movq %rdi, -8(%rbp)
	subq $8, %rsp
	movq  -8(%rbp) , %r10
	movq %r10, %rdi
	movq $0,%rax
	   call printstr
	movq  %rax, %r10
	movq $0,%rax
	leave
	ret
	addq $8, %rsp
	leave
	ret
	.global print_board
print_board:
	pushq %rbp
	movq %rsp, %rbp
	movq %rdi, -8(%rbp)
	subq $8, %rsp
	subq $4, %rsp
	movq $0, %r10
	movl %r10d, -12(%rbp)  
	movl  -12(%rbp) , %r10d
	movq $10, %r11
	cmp %r11d, %r10d
	setl %r10b
	movzbq %r10b, %r10
	cmp $0,%r10
	jz .L2
	jmp .L1
.L1:
	subq $0, %rsp
	subq $4, %rsp
	movq $0, %r10
	movl %r10d, -16(%rbp)  
	movl  -16(%rbp) , %r10d
	movq $10, %r11
	cmp %r11d, %r10d
	setl %r10b
	movzbq %r10b, %r10
	cmp $0,%r10
	jz .L4
	jmp .L3
.L3:
	movq  -8(%rbp) , %r10
	movl  -12(%rbp) , %r11d
	imulq $40,%r11
	addq %r11, %r10
	movl  -16(%rbp) , %r11d
	imulq $4,%r11
	addq %r11, %r10
	movq (%r10), %r10
	movq $0, %r11
	cmp %r11d, %r10d
	setne %r10b
	movzbq %r10b, %r10
	cmp $0, %r10
	jz .L5
	leaq .LC0(%rip) ,%r10
	movq %r10, %rdi
	movq $0,%rax
	   call printmsg
	movq  %rax, %r10
	jmp .L6
.L5:
	leaq .LC1(%rip) ,%r10
	movq %r10, %rdi
	movq $0,%rax
	   call printmsg
	movq  %rax, %r10
.L6:
	leaq  -16(%rbp) , %r10
	movl  -16(%rbp) , %r11d
	addq $1, %r11
	movl %r11d, (%r10)
	movl  -16(%rbp) , %r10d
	movq $10, %r11
	cmp %r11d, %r10d
	setl %r10b
	movzbq %r10b, %r10
	cmp $0,%r10
	jz .L4
	jmp .L3
.L4:
	addq $4, %rsp
	leaq .LC2(%rip) ,%r10
	movq %r10, %rdi
	movq $0,%rax
	   call printmsg
	movq  %rax, %r10
	leaq  -12(%rbp) , %r10
	movl  -12(%rbp) , %r11d
	addq $1, %r11
	movl %r11d, (%r10)
	movl  -12(%rbp) , %r10d
	movq $10, %r11
	cmp %r11d, %r10d
	setl %r10b
	movzbq %r10b, %r10
	cmp $0,%r10
	jz .L2
	jmp .L1
.L2:
	addq $4, %rsp
	leaq .LC3(%rip) ,%r10
	movq %r10, %rdi
	movq $0,%rax
	   call printmsg
	movq  %rax, %r10
	movq $0, %r10
	movq %r10, %rax
	leave
	ret
	addq $8, %rsp
	.global conflict
conflict:
	pushq %rbp
	movq %rsp, %rbp
	movq %rdi, -8(%rbp)
	movl %esi, -12(%rbp)
	movl %edx, -16(%rbp)
	subq $16, %rsp
	subq $4, %rsp
	movq $0, %r10
	movl %r10d, -20(%rbp)  
	movl  -20(%rbp) , %r10d
	movl  -12(%rbp) , %r11d
	cmp %r11d, %r10d
	setl %r10b
	movzbq %r10b, %r10
	cmp $0,%r10
	jz .L8
	jmp .L7
.L7:
	subq $4, %rsp
	movq  -8(%rbp) , %r10
	movl  -20(%rbp) , %r11d
	imulq $40,%r11
	addq %r11, %r10
	movl  -16(%rbp) , %r11d
	imulq $4,%r11
	addq %r11, %r10
	movq (%r10), %r10
	movq $0, %r11
	cmp %r11d, %r10d
	setne %r10b
	movzbq %r10b, %r10
	cmp $0, %r10
	jz .L9
	movq $1, %r10
	movq %r10, %rax
	leave
	ret
	jmp .L10
.L9:
.L10:
	movl  -12(%rbp) , %r10d
	movl  -20(%rbp) , %r11d
	subq %r11, %r10
	movl %r10d, -24(%rbp)  
	movq $0, %r10
	movl  -16(%rbp) , %r11d
	movl  -24(%rbp) , %r12d
	subq %r12, %r11
	movq $1, %r12
	addq %r12, %r11
	cmp %r11d, %r10d
	setl %r10b
	movzbq %r10b, %r10
	movq  -8(%rbp) , %r11
	movl  -20(%rbp) , %r12d
	imulq $40,%r12
	addq %r12, %r11
	movl  -16(%rbp) , %r12d
	movl  -24(%rbp) , %r13d
	subq %r13, %r12
	imulq $4,%r12
	addq %r12, %r11
	movq (%r11), %r11
	movq $0, %r12
	cmp %r12d, %r11d
	setne %r11b
	movzbq %r11b, %r11
	cmp  $0, %r11
	je .L11
	cmp  $0, %r10
	je .L11
	movq $1, %r12
	jmp .L12
.L11:
	movq $0, %r10
.L12:
	cmp $0, %r10
	jz .L13
	movq $1, %r10
	movq %r10, %rax
	leave
	ret
	jmp .L14
.L13:
.L14:
	movl  -16(%rbp) , %r10d
	movl  -24(%rbp) , %r11d
	addq %r11, %r10
	movq $10, %r11
	cmp %r11d, %r10d
	setl %r10b
	movzbq %r10b, %r10
	movq  -8(%rbp) , %r11
	movl  -20(%rbp) , %r12d
	imulq $40,%r12
	addq %r12, %r11
	movl  -16(%rbp) , %r12d
	movl  -24(%rbp) , %r13d
	addq %r13, %r12
	imulq $4,%r12
	addq %r12, %r11
	movq (%r11), %r11
	movq $0, %r12
	cmp %r12d, %r11d
	setne %r11b
	movzbq %r11b, %r11
	cmp  $0, %r11
	je .L15
	cmp  $0, %r10
	je .L15
	movq $1, %r12
	jmp .L16
.L15:
	movq $0, %r10
.L16:
	cmp $0, %r10
	jz .L17
	movq $1, %r10
	movq %r10, %rax
	leave
	ret
	jmp .L18
.L17:
.L18:
	addq $4, %rsp
	leaq  -20(%rbp) , %r10
	movl  -20(%rbp) , %r11d
	addq $1, %r11
	movl %r11d, (%r10)
	movl  -20(%rbp) , %r10d
	movl  -12(%rbp) , %r11d
	cmp %r11d, %r10d
	setl %r10b
	movzbq %r10b, %r10
	cmp $0,%r10
	jz .L8
	jmp .L7
.L8:
	addq $4, %rsp
	movq $0, %r10
	movq %r10, %rax
	leave
	ret
	addq $16, %rsp
	.global solve
solve:
	pushq %rbp
	movq %rsp, %rbp
	movq %rdi, -8(%rbp)
	movl %esi, -12(%rbp)
	subq $12, %rsp
	movl  -12(%rbp) , %r10d
	movq $9, %r11
	cmp %r11d, %r10d
	setg %r10b
	movzbq %r10b, %r10
	cmp $0, %r10
	jz .L19
	subq $0, %rsp
	movq  -8(%rbp) , %r10
	movq %r10, %rdi
	movq $0,%rax
	   call print_board
	movq  %rax, %r10
	movq $0, %r10
	movq %r10, %rax
	leave
	ret
	jmp .L20
.L19:
.L20:
	subq $4, %rsp
	movq $0, %r10
	movl %r10d, -16(%rbp)  
	movl  -16(%rbp) , %r10d
	movq $10, %r11
	cmp %r11d, %r10d
	setl %r10b
	movzbq %r10b, %r10
	cmp $0,%r10
	jz .L22
	jmp .L21
.L21:
	subq $0, %rsp
	movq  -8(%rbp) , %r10
	movq %r10, %rdi
	movl  -12(%rbp) , %r10d
	movq %r10, %rsi
	movl  -16(%rbp) , %r10d
	movq %r10, %rdx
	movq $0,%rax
	   call conflict
	movq  %rax, %r10
	movq $0, %r11
	cmp %r11d, %r10d
	sete %r10b
	movzbq %r10b, %r10
	cmp $0, %r10
	jz .L23
	subq $0, %rsp
	movq  -8(%rbp) , %r10
	movl  -12(%rbp) , %r11d
	imulq $40,%r11
	addq %r11, %r10
	movl  -16(%rbp) , %r11d
	imulq $4,%r11
	addq %r11, %r10
	movq $1, %r11
	movl %r11d, (%r10)
	movq  -8(%rbp) , %r10
	movq %r10, %rdi
	movl  -12(%rbp) , %r10d
	movq $1, %r11
	addq %r11, %r10
	movq %r10, %rsi
	movq $0,%rax
	   call solve
	movq  %rax, %r10
	movq  -8(%rbp) , %r10
	movl  -12(%rbp) , %r11d
	imulq $40,%r11
	addq %r11, %r10
	movl  -16(%rbp) , %r11d
	imulq $4,%r11
	addq %r11, %r10
	movq $0, %r11
	movl %r11d, (%r10)
	jmp .L24
.L23:
.L24:
	leaq  -16(%rbp) , %r10
	movl  -16(%rbp) , %r11d
	addq $1, %r11
	movl %r11d, (%r10)
	movl  -16(%rbp) , %r10d
	movq $10, %r11
	cmp %r11d, %r10d
	setl %r10b
	movzbq %r10b, %r10
	cmp $0,%r10
	jz .L22
	jmp .L21
.L22:
	addq $4, %rsp
	movq $2, %r10
	movq %r10, %rax
	leave
	ret
	addq $12, %rsp
	.global main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $400, %rsp
	subq $4, %rsp
	movq $0, %r10
	movl %r10d, -404(%rbp)  
	movl  -404(%rbp) , %r10d
	movq $10, %r11
	cmp %r11d, %r10d
	setl %r10b
	movzbq %r10b, %r10
	cmp $0,%r10
	jz .L26
	jmp .L25
.L25:
	subq $4, %rsp
	movq $0, %r10
	movl %r10d, -408(%rbp)  
	movl  -408(%rbp) , %r10d
	movq $10, %r11
	cmp %r11d, %r10d
	setl %r10b
	movzbq %r10b, %r10
	cmp $0,%r10
	jz .L28
	jmp .L27
.L27:
	leaq  -400(%rbp) , %r10
	movl  -404(%rbp) , %r11d
	imulq $40,%r11
	addq %r11, %r10
	movl  -408(%rbp) , %r11d
	imulq $4,%r11
	addq %r11, %r10
	movq $0, %r11
	movl %r11d, (%r10)
	leaq  -408(%rbp) , %r10
	movl  -408(%rbp) , %r11d
	addq $1, %r11
	movl %r11d, (%r10)
	movl  -408(%rbp) , %r10d
	movq $10, %r11
	cmp %r11d, %r10d
	setl %r10b
	movzbq %r10b, %r10
	cmp $0,%r10
	jz .L28
	jmp .L27
.L28:
	addq $4, %rsp
	leaq  -404(%rbp) , %r10
	movl  -404(%rbp) , %r11d
	addq $1, %r11
	movl %r11d, (%r10)
	movl  -404(%rbp) , %r10d
	movq $10, %r11
	cmp %r11d, %r10d
	setl %r10b
	movzbq %r10b, %r10
	cmp $0,%r10
	jz .L26
	jmp .L25
.L26:
	addq $4, %rsp
	leaq  -400(%rbp) , %r10
	movq %r10, %rdi
	movq $0, %r10
	movq %r10, %rsi
	movq $0,%rax
	   call solve
	movq  %rax, %r10
	movq $0, %r10
	movq %r10, %rax
	leave
	ret
	addq $400, %rsp
