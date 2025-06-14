.global _start
.text

_start:
	jmp main

fib:
	push %rbp
	mov %rsp, %rbp
	sub $16, %rsp

if0:
	movq 16(%rbp), %r8
	movq $0, %r9
	cmp %r9, %r8
	jne elif0_0

	movq $0, %r8
	movq %r8, %rax
	jmp fib_end

	jmp if0_end

elif0_0:
	movq 16(%rbp), %r8
	movq $1, %r9
	cmp %r9, %r8
	jne else0
	movq $1, %r8
	movq %r8, %rax
	jmp fib_end

	jmp if0_end
else0:
	movq 16(%rbp), %r8
	movq $1, %r9
	sub %r9, %r8
	push %r8
	call fib
	pop %rbx
	movq %rax, %r8
	movq %r8, -8(%rbp)

	movq 16(%rbp), %r8
	movq $2, %r9
	sub %r9, %r8
	push %r8
	call fib
	pop %rbx
	movq %rax, %r8
	movq %r8, -16(%rbp)

	movq -8(%rbp), %r8
	movq -16(%rbp), %r9
	add %r9, %r8
	movq %r8, %rax
	jmp fib_end

if0_end:

fib_end:
	add $16, %rsp
	mov %rbp, %rsp
	pop %rbp
	ret

fun:
	push %rbp
	mov %rsp, %rbp
	sub $8, %rsp

	movq 16(%rbp), %r8
	movq 24(%rbp), %r9
	add %r9, %r8
	movq %r8, -8(%rbp)

switch1:
switch1_case0:
	movq -8(%rbp), %rcx
	movq $5, %rdx
	cmp %rcx, %rdx
	jne switch1_case1
	movq -8(%rbp), %r8
	movq %r8, %rax
	jmp fun_end

switch1_case1:
	movq -8(%rbp), %rcx
	movq $10, %rdx
	cmp %rcx, %rdx
	jne switch1_case2
	movq 16(%rbp), %r8
	movq %r8, %rax
	jmp fun_end

switch1_case2:
	movq -8(%rbp), %rcx
	movq $15, %rdx
	cmp %rcx, %rdx
	jne switch1_default
	movq 24(%rbp), %r8
	movq %r8, %rax
	jmp fun_end

switch1_default:
	movq $0, %r8
	movq %r8, %rax
	jmp fun_end

switch1_end:

fun_end:
	add $8, %rsp
	mov %rbp, %rsp
	pop %rbp
	ret

main:
	push %rbp
	mov %rsp, %rbp
	sub $96, %rsp

	movq $10, %r8
	movq %r8, -8(%rbp)

	movq -8(%rbp), %r8
	push %r8
	call fib
	pop %rbx
	movq %rax, %r8
	movq %r8, -24(%rbp)

	movq -24(%rbp), %r8
	movq %r8, -16(%rbp)

	movq -16(%rbp), %r8
	movq %r8, %rax
	call _printf

	movq $0, -32(%rbp)

	movq $0, -40(%rbp)

	movq $5, %r8
	movq %r8, -48(%rbp)

	movq $3, %r8
	movq $5, %r9
	movq -48(%rbp), %r10
	movq $2, %r11
	xor %rdx, %rdx
	movq %r10, %rax
	mul %r11
	movq %rax, %r10

	movq $3, %r11
	xor %rdx, %rdx
	movq %r10, %rax
	div %r11
	movq %rax, %r10

	add %r10, %r9
	and %r9, %r8
	movq $9, %r9
	or %r9, %r8
	movq %r8, -56(%rbp)

	movq -56(%rbp), %r8
	movq %r8, %rax
	call _printf

	movq $0, %r8
	movq %r8, -32(%rbp)

for2:
	movq -32(%rbp), %r8
	movq $10, %r9
	cmp %r9, %r8
	jge for2_end

	movq -40(%rbp), %r8
	movq $2, %r9
	add %r9, %r8
	movq %r8, -40(%rbp)

	movq -32(%rbp), %r8
	movq $1, %r9
	add %r9, %r8
	movq %r8, -32(%rbp)
	jmp for2

for2_end:

do_while3:
	movq -40(%rbp), %r8
	movq $5, %r9
	sub %r9, %r8
	movq %r8, -40(%rbp)

	movq -40(%rbp), %r8
	movq $10, %r9
	cmp %r9, %r8
	jge do_while3

	movq -40(%rbp), %r8
	push %r8
	movq -32(%rbp), %r8
	push %r8
	call fun
	pop %rbx
	pop %rbx
	movq %rax, %r8
	movq %r8, -64(%rbp)

	movq -64(%rbp), %r8
	movq %r8, -16(%rbp)

	movq -16(%rbp), %r8
	movq %r8, %rax
	call _printf

	movq $3, %r8
	movq %r8, -72(%rbp)

	movq $1, %r8
	movq %r8, -80(%rbp)

	movq -16(%rbp), %r8
	movq $1, %r9
	movq %r9, %rcx
	sal %rcx, %r8
	movq -40(%rbp), %r9
	movq $2, %r10
	xor %rdx, %rdx
	movq %r9, %rax
	idiv %r10
	movq %rax, %r9

	sub %r9, %r8
	movq %r8, -16(%rbp)

	movq -16(%rbp), %r8
	movq %r8, %rax
	call _printf

	movq -8(%rbp), %r8
	movq -80(%rbp), %r9
	add %r9, %r8
	push %r8
	call fib
	pop %rbx
	movq %rax, %r8
	movq %r8, -96(%rbp)

	movq -96(%rbp), %r8
	movq %r8, -88(%rbp)

	movq -88(%rbp), %r8
	movq %r8, %rax
	call _printf

	movq -88(%rbp), %r8
	movq %r8, %rax
	jmp main_end

main_end:
	add $96, %rsp
	mov %rbp, %rsp
	pop %rbp
	mov %rax, %rdi
	movq $60, %rax
	syscall

_printf:
	push %rbp
	mov %rsp, %rbp
	sub $32, %rsp
	lea 31(%rsp), %rsi

	mov %rax, %r9
	mov $10, %rcx
	mov $0, %rbx

	cmp $0, %r9
	jne _printf_loop

	movb $'0', -1(%rsi)
	dec %rsi
	inc %rbx
	jmp _printf_done

_printf_loop:
	xor %rdx, %rdx
	mov %r9, %rax
	div %rcx
	add $'0', %dl
	dec %rsi
	mov %dl, (%rsi)
	inc %rbx
	mov %rax, %r9
	test %rax, %rax
	jnz _printf_loop

_printf_done:
	dec %rsi
	movb $'\n', (%rsi)
	inc %rbx

	mov $1, %rax
	mov $1, %rdi
	mov %rbx, %rdx
	syscall

	mov %rbp, %rsp
	pop %rbp
	ret
