			# Code généré automatiquement par MonCompilateur
	.extern printf
a:	.quad 0
b:	.quad 0
	.text
	.globl main
main:
	movq %rsp, %rbp
	push $1
	pop a
	push $2
	pop b
	push a
	pop %rsi
	movq $FormatString1, %rdi
	movl $0, %eax
	call printf@PLT
	push b
	pop %rsi
	movq $FormatString1, %rdi
	movl $0, %eax
	call printf@PLT
