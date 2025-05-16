			# Code généré automatiquement par MonCompilateur
	.extern printf
a:	.quad 0
b:	.quad 0
c:	.quad 0
x:	.quad 0
y:	.quad 0
z:	.quad 0
	.text
	.globl main
main:
	movq %rsp, %rbp
	push $5
	push $6
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop x
	push x
	push $2
	pop %rbx
	pop %rax
	subq	%rbx, %rax	# SUB
	push %rax
	pop y
	push x
	push y
	pop %rbx
	pop %rax
	mulq	%rbx
	push %rax	# MUL
	pop z
	push $1
	pop a
	push $0
	pop b
	push x
	pop %rsi
	movq $FormatString1, %rdi
	movl $0, %eax
	call printf@PLT
	push y
	pop %rsi
	movq $FormatString1, %rdi
	movl $0, %eax
	call printf@PLT
	push z
	pop %rsi
	movq $FormatString1, %rdi
	movl $0, %eax
	call printf@PLT
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
DisplayMsg:	.string "Résultat : %llu\n"	# affichage entier 64 bits
FormatString1:	.string "%llu\n"	# affichage brut sans message
TrueString:	.string "TRUE\n"
FalseString:	.string "FALSE\n"
	.section .note.GNU-stack,"",@progbits
