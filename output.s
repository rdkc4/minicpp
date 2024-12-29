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
	push 16(%rbp)
	push $0
	pop %rdx
	pop %rcx
	cmp %rdx, %rcx
	jge else0
	push $0
	pop %rax
	jmp fib_end

	jmp if0_end

else0:
if0_end:

while1:
	push -8(%rbp)
	push 16(%rbp)
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
	push -16(%rbp)
	push -24(%rbp)
	pop %rdx
	pop %rcx
	cmp %rdx, %rcx
	jle else2
	push -16(%rbp)
	push -24(%rbp)
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -24(%rbp)

	jmp if2_end

else2:
	push -16(%rbp)
	push -24(%rbp)
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -16(%rbp)

if2_end:

	push -8(%rbp)
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
	push -16(%rbp)
	push -24(%rbp)
	pop %rdx
	pop %rcx
	cmp %rdx, %rcx
	jle else3
	push -16(%rbp)
	pop %rax
	jmp fib_end

	jmp if3_end

else3:
	push -24(%rbp)
	pop %rax
	jmp fib_end

if3_end:

fib_end:
	add $24, %rsp
	mov %rbp, %rsp
	pop %rbp
	ret

main:
	push %rbp
	mov %rsp, %rbp
	sub $32, %rsp

	movq $0, -8(%rbp)
	movq $0, -16(%rbp)
	movq $0, -24(%rbp)
	movq $0, -32(%rbp)

	push $10
	pop %rax
	mov %rax, -16(%rbp)

	push -16(%rbp)
	call fib
	pop %r9
	push %rax
	pop %rax
	mov %rax, -8(%rbp)

	push $5
	pop %rax
	mov %rax, -24(%rbp)

	push $3
	push -24(%rbp)
	push $2
	pop %rbx
	pop %rax
	sub %rbx, %rax
	push %rax
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -32(%rbp)

if4:
	push -24(%rbp)
	push -32(%rbp)
	pop %rdx
	pop %rcx
	cmp %rdx, %rcx
	jbe else4
	push $-1
	pop %rax
	jmp main_end

	jmp if4_end

else4:
	push -8(%rbp)
	pop %rax
	jmp main_end

if4_end:

main_end:
	add $32, %rsp
	mov %rbp, %rsp
	pop %rbp
	mov %rax, %rdi
	movq $60, %rax
	syscall
