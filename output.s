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

	push -16(%rbp)
	call fib
	pop %r9
	push %rax
	pop %rax
	mov %rax, -8(%rbp)

	push $5
	pop %rax
	mov %rax, -40(%rbp)

	push $3
	push -40(%rbp)
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
	mov %rax, -48(%rbp)

	push $0
	pop %rax
	mov %rax, -24(%rbp)

for4:
	push -24(%rbp)
	push $10
	pop %rdx
	pop %rcx
	cmp %rdx, %rcx
	jge for4_end

	push -32(%rbp)
	push $2
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -32(%rbp)

	push -24(%rbp)
	push $1
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	mov %rax, -24(%rbp)
	jmp for4

for4_end:

if5:
	push -40(%rbp)
	push -48(%rbp)
	pop %rdx
	pop %rcx
	cmp %rdx, %rcx
	jbe else5
	push $-1
	pop %rax
	jmp main_end

	jmp if5_end

else5:
	push -32(%rbp)
	push -24(%rbp)
	pop %rbx
	pop %rax
	add %rbx, %rax
	push %rax
	pop %rax
	jmp main_end

if5_end:

main_end:
	add $48, %rsp
	mov %rbp, %rsp
	pop %rbp
	mov %rax, %rdi
	movq $60, %rax
	syscall
