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

	movq $0, %r8
	movq %r8, -8(%rbp)

	movq $1, %r8
	movq %r8, -16(%rbp)

	movq $1, %r8
	movq %r8, -24(%rbp)

if0:
	movq 16(%rbp), %r8
	movq $0, %r9
	movq %r9, %rdx
	movq %r8, %rcx
	cmp %rdx, %rcx

	jge elif0_0
	movq $0, %r8
	movq %r8, %rax
	jmp fib_end

	jmp if0_end

elif0_0:
	movq 16(%rbp), %r8
	movq $3, %r9
	movq %r9, %rdx
	movq %r8, %rcx
	cmp %rdx, %rcx
	jge if0_end
	movq $1, %r8
	movq %r8, %rax
	jmp fib_end

	jmp if0_end
if0_end:

while1:
	movq -8(%rbp), %r8
	movq 16(%rbp), %r9
	movq $2, %r10
	movq %r10, %rbx
	movq %r9, %rax
	sub %rbx, %rax
	movq %rax, %r9

	movq %r9, %rdx
	movq %r8, %rcx
	cmp %rdx, %rcx
	jge while1_end

if2:
	movq -16(%rbp), %r8
	movq -24(%rbp), %r9
	movq %r9, %rdx
	movq %r8, %rcx
	cmp %rdx, %rcx

	jle else2
	movq -16(%rbp), %r8
	movq -24(%rbp), %r9
	movq %r9, %rbx
	movq %r8, %rax
	add %rbx, %rax
	movq %rax, %r8

	movq %r8, -24(%rbp)

	jmp if2_end

else2:
	movq -16(%rbp), %r8
	movq -24(%rbp), %r9
	movq %r9, %rbx
	movq %r8, %rax
	add %rbx, %rax
	movq %rax, %r8

	movq %r8, -16(%rbp)

if2_end:

	movq -8(%rbp), %r8
	movq $1, %r9
	movq %r9, %rbx
	movq %r8, %rax
	add %rbx, %rax
	movq %rax, %r8

	movq %r8, -8(%rbp)

	jmp while1

while1_end:

if3:
	movq -16(%rbp), %r8
	movq -24(%rbp), %r9
	movq %r9, %rdx
	movq %r8, %rcx
	cmp %rdx, %rcx

	jle else3
	movq -16(%rbp), %r8
	movq %r8, %rax
	jmp fib_end

	jmp if3_end

else3:
	movq -24(%rbp), %r8
	movq %r8, %rax
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
	movq 24(%rbp), %r9
	movq %r9, %rbx
	movq %r8, %rax
	add %rbx, %rax
	movq %rax, %r8

	movq %r8, -8(%rbp)

switch4:
switch4_case0:
	movq -8(%rbp), %rcx
	movq $5, %rdx
	cmp %rcx, %rdx
	jne switch4_case1
	movq -8(%rbp), %r8
	movq %r8, %rax
	jmp fun_end

switch4_case1:
	movq -8(%rbp), %rcx
	movq $10, %rdx
	cmp %rcx, %rdx
	jne switch4_case2
	movq 16(%rbp), %r8
	movq %r8, %rax
	jmp fun_end

switch4_case2:
	movq -8(%rbp), %rcx
	movq $15, %rdx
	cmp %rcx, %rdx
	jne switch4_default
	movq 24(%rbp), %r8
	movq %r8, %rax
	jmp fun_end

switch4_default:
	movq $0, %r8
	movq %r8, %rax
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

	movq $10, %r8
	movq %r8, -16(%rbp)

	movq -16(%rbp), %r8
	push %r8
	call fib
	pop %r8
	movq %rax, %r8
	movq %r8, -8(%rbp)

	movq $5, %r8
	movq %r8, -40(%rbp)

	movq $3, %r8
	movq $5, %r9
	movq -40(%rbp), %r10
	movq $2, %r11
	movq %r11, %rbx
	movq %r10, %rax
	xor %rdx, %rdx
	mul %rbx
	movq %rax, %r10

	movq $3, %r11
	movq %r11, %rbx
	movq %r10, %rax
	xor %rdx, %rdx
	div %rbx
	movq %rax, %r10

	movq %r10, %rbx
	movq %r9, %rax
	add %rbx, %rax
	movq %rax, %r9

	movq %r9, %rbx
	movq %r8, %rax
	and %rbx, %rax
	movq %rax, %r8

	movq $9, %r9
	movq %r9, %rbx
	movq %r8, %rax
	or %rbx, %rax
	movq %rax, %r8

	movq %r8, -48(%rbp)

	movq $0, %r8
	movq %r8, -24(%rbp)

for5:
	movq -24(%rbp), %r8
	movq $10, %r9
	movq %r9, %rdx
	movq %r8, %rcx
	cmp %rdx, %rcx
	jge for5_end

	movq -32(%rbp), %r8
	movq $2, %r9
	movq %r9, %rbx
	movq %r8, %rax
	add %rbx, %rax
	movq %rax, %r8

	movq %r8, -32(%rbp)

	movq -24(%rbp), %r8
	movq $1, %r9
	movq %r9, %rbx
	movq %r8, %rax
	add %rbx, %rax
	movq %rax, %r8

	movq %r8, -24(%rbp)
	jmp for5

for5_end:

do_while6:
	movq -32(%rbp), %r8
	movq $5, %r9
	movq %r9, %rbx
	movq %r8, %rax
	sub %rbx, %rax
	movq %rax, %r8

	movq %r8, -32(%rbp)

	movq -32(%rbp), %r8
	movq $10, %r9
	movq %r9, %rdx
	movq %r8, %rcx
	cmp %rdx, %rcx
	jge do_while6

	movq -32(%rbp), %r8
	push %r8
	movq -24(%rbp), %r8
	push %r8
	call fun
	pop %r8
	pop %r8
	movq %rax, %r8
	movq %r8, -8(%rbp)

	movq -8(%rbp), %r8
	movq $2, %r9
	movq %r9, %rbx
	movq %r8, %rax
	xor %rdx, %rdx
	imul %rbx
	movq %rax, %r8

	movq -32(%rbp), %r9
	movq $2, %r10
	movq %r10, %rbx
	movq %r9, %rax
	xor %rdx, %rdx
	idiv %rbx
	movq %rax, %r9

	movq %r9, %rbx
	movq %r8, %rax
	sub %rbx, %rax
	movq %rax, %r8

	movq %r8, -8(%rbp)

if7:
	movq -40(%rbp), %r8
	movq -48(%rbp), %r9
	movq %r9, %rdx
	movq %r8, %rcx
	cmp %rdx, %rcx

	jbe else7
	movq $-1, %r8
	movq %r8, %rax
	jmp main_end

	jmp if7_end

else7:
	movq -32(%rbp), %r8
	movq -24(%rbp), %r9
	movq %r9, %rbx
	movq %r8, %rax
	add %rbx, %rax
	movq %rax, %r8

	movq -8(%rbp), %r9
	movq %r9, %rbx
	movq %r8, %rax
	sub %rbx, %rax
	movq %rax, %r8

	movq %r8, %rax
	jmp main_end

if7_end:

main_end:
	add $48, %rsp
	mov %rbp, %rsp
	pop %rbp
	mov %rax, %rdi
	movq $60, %rax
	syscall
