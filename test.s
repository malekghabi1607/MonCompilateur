			# Code généré automatiquement par MonCompilateur
	.extern printf
	.data
	.align 8
a:	.quad 0
b:	.quad 0
c:	.quad 0
z:	.quad 0
i:	.quad 0
	.text
	.globl main
main:
	movq %rsp, %rbp
	push $2
	pop a
	push $10
	pop b
	push $0
	pop z
	push a
	push b
	pop %rax
	pop %rbx
	cmpq %rax, %rbx
	jb Vrai2
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
	push $2
	pop z
FINIF1:
DEBUTWHILE3:
	push a
	push b
	pop %rax
	pop %rbx
	cmpq %rax, %rbx
	jb Vrai4
	push $0
	jmp Suite4
Vrai4:	push $-1
Suite4:
	pop %rax
	cmpq $0, %rax
	je FINWHILE3
	push a
	push $1
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop a
	jmp DEBUTWHILE3
FINWHILE3:
	push $0
	pop i
DEBUTFOR5:
	push $3
	pop %rax
	cmpq %rax, i
	ja FINFOR5
	push b
	push $1
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop b
	incq i
	jmp DEBUTFOR5
FINFOR5:
	push $5
	pop c
	push z
	push c
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop z
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
