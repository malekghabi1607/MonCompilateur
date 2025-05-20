			# Code généré automatiquement par MonCompilateur
	.extern printf
a:	.quad 0
b:	.quad 0
c:	.quad 0
	.text
	.globl main
main:
	movq %rsp, %rbp
DEBUG BlockStatement : current token = 3, texte = 'a'
	push $10
	pop a
	push $20
	pop b
	movq a(%rip), %rax
	push %rax
	movq b(%rip), %rax
	push %rax
	pop %rax
	pop %rbx
	cmpq %rax, %rbx
	jb Vrai1
	push $0
	jmp Suite1
Vrai1:	push $-1
Suite1:
	pop c
	movq a(%rip), %rax
	push %rax
	pop %rsi	# valeur à afficher
	movq $FormatString1, %rdi	# format %llu\n
	movl $0, %eax			# nombre d'arguments flottants
	call printf@PLT
	movq b(%rip), %rax
	push %rax
	pop %rsi	# valeur à afficher
	movq $FormatString1, %rdi	# format %llu\n
	movl $0, %eax			# nombre d'arguments flottants
	call printf@PLT
	movq c(%rip), %rax
	push %rax
	pop %rsi	# valeur booléenne à afficher
	cmpq $0, %rsi
	je BoolFalse4
	movq $TrueString, %rdi	# chaîne TRUE
	jmp BoolEnd4
BoolFalse4:
	movq $FalseString, %rdi	# chaîne FALSE
BoolEnd4:
	call puts@PLT
