			# Code généré automatiquement par MonCompilateur
	.extern printf
	.data
	.align 8
a:	.quad 0
b:	.quad 0
	.text
	.globl main
main:
	movq %rsp, %rbp
	push $4
	push $3
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop a
	push a
	push $2
	pop %rbx
	pop %rax
	mulq	%rbx
	push %rax	# MUL
	pop b
	movq a, %rsi
	leaq msg_a(%rip), %rdi
	xor %rax, %rax
	call printf
	movq b, %rsi
	leaq msg_b(%rip), %rdi
	xor %rax, %rax
	call printf
	movq %rbp, %rsp
	ret
	.section .rodata
msg_a:	.string "Valeur de a : %ld\n"
msg_b:	.string "Valeur de b : %ld\n"
	.section .note.GNU-stack,"",@progbits
	.section .note.GNU-stack,"",@progbits
