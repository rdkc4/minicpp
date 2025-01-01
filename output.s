.global _start
.text

_start:
	jmp main

fib:
	push %rbp
	mov %rsp, %rbp
	sub $24, %rsp

	movq $0, -8(%rbp)
	movq $0, -16(%rbp)
	movq $0, -24(%rbp)

	push $0
	pop %rax
	mov %rax, -8(%rbp)

	push $1
	pop %rax
	mov %rax, -16(%rbp)

	push $1
	pop %rax
	mov %rax, -24(%rbp)

if0:
	movq 16(%rbp), %r8
	push %r8
	push $0
	pop %rdx
	pop %rcx
	cmp %rdx, %rcx
	jge elif0_0
	push $0
	pop %rax
	jmp fib_end

	jmp if0_end

elif0_0:
	movq 16(%rbp), %r8
	push %r8
	push $3
	pop %rdx
	pop %rcx
	cmp %rdx, %rcx
	jge if0_end
	push $1
	pop %rax
	jmp fib_end

	jmp if0_end
if0_end:

while1:
	movq -8(%rbp), %r8
	push %r8
	movq 16(%rbp), %r8
	push %r8
	push $2
	pop %rbx
	pop %rax
	sub %rbx, %rax
	push %rax
	pop %rdx
	pop %rcx
	cmp %rdx, %rcx
	jge while1_end

if2:
	movq -16(%rbp), %r8
	push %r8
	movq -24(%rbp), %r8
	push %r8
	pop %rdx
	pop %rcx
	cmp %rdx, %rcx
	jle else2
	movq -16(%rbp), %r8
	push %r8
	movq -24(%rbp), %r8
	push %r8
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -24(%rbp)

	jmp if2_end

else2:
	movq -16(%rbp), %r8
	push %r8
	movq -24(%rbp), %r8
	push %r8
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -16(%rbp)

if2_end:

	movq -8(%rbp), %r8
	push %r8
	push $1
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -8(%rbp)

	jmp while1

while1_end:

if3:
	movq -16(%rbp), %r8
	push %r8
	movq -24(%rbp), %r8
	push %r8
	pop %rdx
	pop %rcx
	cmp %rdx, %rcx
	jle else3
	movq -16(%rbp), %r8
	push %r8
	pop %rax
	jmp fib_end

	jmp if3_end

else3:
	movq -24(%rbp), %r8
	push %r8
	pop %rax
	jmp fib_end

if3_end:

fib_end:
	add $24, %rsp
	mov %rbp, %rsp
	pop %rbp
	ret

fun:
	push %rbp
	mov %rsp, %rbp
	sub $8, %rsp

	movq $0, -8(%rbp)

	movq 16(%rbp), %r8
	push %r8
	movq 24(%rbp), %r8
	push %r8
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -8(%rbp)

switch4:
switch4_case0:
	movq -8(%rbp), %rcx
	movq $5, %rdx
	cmp %rcx, %rdx
	jne switch4_case1
	movq -8(%rbp), %r8
	push %r8
	pop %rax
	jmp fun_end

switch4_case1:
	movq -8(%rbp), %rcx
	movq $10, %rdx
	cmp %rcx, %rdx
	jne switch4_case2
	movq 16(%rbp), %r8
	push %r8
	pop %rax
	jmp fun_end

switch4_case2:
	movq -8(%rbp), %rcx
	movq $15, %rdx
	cmp %rcx, %rdx
	jne switch4_default
	movq 24(%rbp), %r8
	push %r8
	pop %rax
	jmp fun_end

switch4_default:
	push $0
	pop %rax
	jmp fun_end

switch4_end:

fun_end:
	add $8, %rsp
	mov %rbp, %rsp
	pop %rbp
	ret

main:
	push %rbp
	mov %rsp, %rbp
	sub $48, %rsp

	movq $0, -8(%rbp)
	movq $0, -16(%rbp)
	movq $0, -24(%rbp)
	movq $0, -32(%rbp)
	movq $0, -40(%rbp)
	movq $0, -48(%rbp)

	push $10
	pop %rax
	mov %rax, -16(%rbp)

	movq -16(%rbp), %r8
	push %r8
	call fib
	pop %r9
	push %rax
	pop %rax
	mov %rax, -8(%rbp)

	push $5
	pop %rax
	mov %rax, -40(%rbp)

	push $3
	push $5
	movq -40(%rbp), %r8
	push %r8
	push $2
	pop %rbx
	pop %rax
	xor %rdx, %rdx
	mul %rbx
	push %rax
	push $3
	pop %rbx
	pop %rax
	xor %rdx, %rdx
	div %rbx
	push %rax
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rbx
	pop %rax
	and %rbx, %rax
	push %rax
	push $9
	pop %rbx
	pop %rax
	or %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -48(%rbp)

	push $0
	pop %rax
	mov %rax, -24(%rbp)

for5:
	movq -24(%rbp), %r8
	push %r8
	push $10
	pop %rdx
	pop %rcx
	cmp %rdx, %rcx
	jge for5_end

	movq -32(%rbp), %r8
	push %r8
	push $2
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -32(%rbp)

	movq -24(%rbp), %r8
	push %r8
	push $1
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -24(%rbp)
	jmp for5

for5_end:

do_while6:
	movq -32(%rbp), %r8
	push %r8
	push $5
	pop %rbx
	pop %rax
	sub %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -32(%rbp)

	movq -32(%rbp), %r8
	push %r8
	push $10
	pop %rdx
	pop %rcx
	cmp %rdx, %rcx
	jge do_while6

	movq -32(%rbp), %r8
	push %r8
	movq -24(%rbp), %r8
	push %r8
	call fun
	pop %r9
	pop %r9
	push %rax
	pop %rax
	mov %rax, -8(%rbp)

	movq -8(%rbp), %r8
	push %r8
	push $2
	pop %rbx
	pop %rax
	xor %rdx, %rdx
	imul %rbx
	push %rax
	movq -32(%rbp), %r8
	push %r8
	push $2
	pop %rbx
	pop %rax
	xor %rdx, %rdx
	idiv %rbx
	push %rax
	pop %rbx
	pop %rax
	sub %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -8(%rbp)

if7:
	movq -40(%rbp), %r8
	push %r8
	movq -48(%rbp), %r8
	push %r8
	pop %rdx
	pop %rcx
	cmp %rdx, %rcx
	jbe else7
	push $-1
	pop %rax
	jmp main_end

	jmp if7_end

else7:
	movq -32(%rbp), %r8
	push %r8
	movq -24(%rbp), %r8
	push %r8
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	movq -8(%rbp), %r8
	push %r8
	pop %rbx
	pop %rax
	sub %rbx, %rax
	push %rax
	pop %rax
	jmp main_end

if7_end:

main_end:
	add $48, %rsp
	mov %rbp, %rsp
	pop %rbp
	mov %rax, %rdi
	movq $60, %rax
	syscall
