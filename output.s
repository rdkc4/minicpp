.global _start
.text

_start:
	jmp main

fun:
	push %rbp
	mov %rsp, %rbp
	push 16(%rbp)
	push 16(%rbp)
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	jmp fun_end

fun_end:
	mov %rbp, %rsp
	pop %rbp
	ret

xyz:
	push %rbp
	mov %rsp, %rbp
	push 16(%rbp)
	push 24(%rbp)
	push 32(%rbp)
	pop %rbx
	pop %rax
	sub %rbx, %rax
	push %rax
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	jmp xyz_end

xyz_end:
	mov %rbp, %rsp
	pop %rbp
	ret

afa:
	push %rbp
	mov %rsp, %rbp
	push $3
	pop %rax
	jmp afa_end

afa_end:
	mov %rbp, %rsp
	pop %rbp
	ret

main:
	push %rbp
	mov %rsp, %rbp
	sub $56, %rsp

	movq $0, -8(%rbp)
	movq $0, -16(%rbp)
	movq $0, -24(%rbp)
	movq $0, -32(%rbp)
	movq $0, -40(%rbp)
	movq $0, -48(%rbp)
	movq $0, -56(%rbp)

if0:
	push -8(%rbp)
	push -16(%rbp)
	pop %rdx
	pop %rcx
	cmp %rcx, %rdx
	jle else0
	push $3
	pop %rax
	mov %rax, -16(%rbp)


	jmp if0_end

else0:
	push $0
	pop %rax
	mov %rax, -16(%rbp)

	push $1
	push $1
	call afa
	pop %r9
	push %rax
	push $3
	call xyz
	pop %r9
	pop %r9
	pop %r9
	push %rax
	pop %rax
	mov %rax, -8(%rbp)

	push $1
	pop %rax
	mov %rax, -8(%rbp)

	push -8(%rbp)
	pop %rax
	mov %rax, -16(%rbp)


if0_end:

	push $3
	push -48(%rbp)
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	push $3
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -40(%rbp)

	push $8
	pop %rax
	mov %rax, -56(%rbp)

	push -40(%rbp)
	push -56(%rbp)
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -48(%rbp)

	push -40(%rbp)
	push -56(%rbp)
	call fun
	pop %r9
	push %rax
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	push -56(%rbp)
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -48(%rbp)

if1:
	push -8(%rbp)
	push -16(%rbp)
	pop %rdx
	pop %rcx
	cmp %rcx, %rdx
	jle else1
if2:
	push -24(%rbp)
	push -32(%rbp)
	pop %rdx
	pop %rcx
	cmp %rcx, %rdx
	jle else2
	push -8(%rbp)
	pop %rax
	jmp main_end

	jmp if2_end

else2:
	push -8(%rbp)
	pop %rax
	jmp main_end

if2_end:

	jmp if1_end

else1:
	push -8(%rbp)
	push $1
	pop %rbx
	pop %rax
	sub %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -8(%rbp)

	push $3
	pop %rax
	mov %rax, -32(%rbp)


if1_end:

	push $1
	pop %rax
	jmp main_end

main_end:
	add $56, %rsp
	mov %rbp, %rsp
	pop %rbp
	mov %rax, %rdi
	movq $60, %rax
	syscall
