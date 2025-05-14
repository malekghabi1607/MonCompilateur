			# Code généré automatiquement par MonCompilateur
	.extern printf
	.data
	.align 8
a:	.quad 0
b:	.quad 0
c:	.quad 0
z:	.quad 0
	.text
	.globl main
main:
	movq %rsp, %rbp
	push $10
	pop a
	push $20
	pop b
	push a
	push b
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop c
	push c
	push $25
	pop %rax
	pop %rbx
	cmpq %rax, %rbx
	ja Vrai2
	push $0
	jmp Suite2
Vrai2:	push $-1
Suite2:
	pop %rax
	cmpq $0, %rax
	je ELSE1
	push $1
	pop z
	jmp FINIF1
ELSE1:
	push $0
	pop z
FINIF1:
	movq a, %rsi
	leaq msg_a(%rip), %rdi
	xor %rax, %rax
	call printf
	movq b, %rsi
	leaq msg_b(%rip), %rdi
	xor %rax, %rax
	call printf
	movq c, %rsi
	leaq msg_c(%rip), %rdi
	xor %rax, %rax
	call printf
	movq z, %rsi
	leaq msg_z(%rip), %rdi
	xor %rax, %rax
	call printf
	movq %rbp, %rsp
	ret
	.section .rodata
msg_a:	.string "Valeur de a : %ld\n"
msg_b:	.string "Valeur de b : %ld\n"
msg_c:	.string "Valeur de c : %ld\n"
msg_z:	.string "Valeur de z : %ld\n"
	.section .note.GNU-stack,"",@progbits
