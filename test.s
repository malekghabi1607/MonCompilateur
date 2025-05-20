			# Code généré automatiquement par MonCompilateur
	.extern printf
DEBUG VarDeclarationPart: current token = x
x:	.double 0.0
y:	.double 0.0
DEBUG VarDeclarationPart loop: current token = c
c:	.byte 0
DEBUG VarDeclarationPart end: current token = BEGIN
	.text
	.globl main
main:
	movq %rsp, %rbp
DEBUG BlockStatement : current token = 3, texte = 'x'
	push $3
	pop x
DEBUG avant END check : current token = 11, texte = '.'
