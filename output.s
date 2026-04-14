.global _start
.text

_start:
	jmp main
fib:
	push %rbp
	mov %rsp, %rbp
	sub $16, %rsp
_if0_0:
	movq 16(%rbp), %r8
	movq $0, %r9
	cmp %r9, %r8
	je _if0_body1
	jne _orl1_mid
_orl1_mid:
	movq 16(%rbp), %r8
	movq $1, %r9
	cmp %r9, %r8
	je _if0_body1
	jne _else0
_if0_body1:
	movq 16(%rbp), %r8
	movq %r8, %rax
	jmp _fib_end
	jmp _if0_end
_else0:
	movq 16(%rbp), %r8
	movq $1, %r9
	sub %r9, %r8
	push %r8
	call fib
	add $8, %rsp
	movq %rax, %r8
	movq %r8, -8(%rbp)
	movq 16(%rbp), %r8
	movq $2, %r9
	sub %r9, %r8
	push %r8
	call fib
	add $8, %rsp
	movq %rax, %r8
	movq %r8, -16(%rbp)
	movq -16(%rbp), %r8
	movq -8(%rbp), %r9
	add %r9, %r8
	movq %r8, %rax
	jmp _fib_end
_if0_end:
_fib_end:
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
_switch4:
_switch4_case0:
	movq -8(%rbp), %rcx
	movq $5, %rdx
	cmp %rcx, %rdx
	jne _switch4_case1
	movq -8(%rbp), %r8
	movq %r8, %rax
	jmp _fun_end
_switch4_case1:
	movq -8(%rbp), %rcx
	movq $10, %rdx
	cmp %rcx, %rdx
	jne _switch4_case2
	movq 16(%rbp), %r8
	movq %r8, %rax
	jmp _fun_end
_switch4_case2:
	movq -8(%rbp), %rcx
	movq $15, %rdx
	cmp %rcx, %rdx
	jne _switch4_default
	movq 24(%rbp), %r8
	movq %r8, %rax
	jmp _fun_end
_switch4_default:
	movq $0, %r8
	movq %r8, %rax
	jmp _fun_end
_switch4_end:
_fun_end:
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
	add $8, %rsp
	movq %rax, %r8
	movq %r8, -24(%rbp)
	movq -24(%rbp), %r8
	movq %r8, -16(%rbp)
	movq -16(%rbp), %r8
	push %r8
	call print_i
	add $8, %rsp
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
	push %r8
	call print_u
	add $8, %rsp
	movq $0, %r8
	movq %r8, -32(%rbp)
_for5:
	movq -32(%rbp), %r8
	movq $10, %r9
	cmp %r9, %r8
	jl _for5_body
	jge _for5_end
_for5_body:
	movq -40(%rbp), %r8
	movq $2, %r9
	add %r9, %r8
	movq %r8, -40(%rbp)
	movq -32(%rbp), %r8
	movq $1, %r9
	add %r9, %r8
	movq %r8, -32(%rbp)
	jmp _for5
_for5_end:
_do_while7:
	movq -40(%rbp), %r8
	movq $5, %r9
	sub %r9, %r8
	movq %r8, -40(%rbp)
	movq -40(%rbp), %r8
	movq $10, %r9
	cmp %r9, %r8
	jge _do_while7
	jl _do_while7_end
_do_while7_end:
	movq -40(%rbp), %r8
	push %r8
	movq -32(%rbp), %r8
	push %r8
	call fun
	add $16, %rsp
	movq %rax, %r8
	movq %r8, -64(%rbp)
	movq -64(%rbp), %r8
	movq %r8, -16(%rbp)
	movq -16(%rbp), %r8
	push %r8
	call print_i
	add $8, %rsp
	movq $3, %r8
	movq %r8, -72(%rbp)
	movq $1, %r8
	movq %r8, -80(%rbp)
	movq -16(%rbp), %r8
	movq $1, %r9
	movq %r9, %rcx
	sal %cl, %r8
	movq -40(%rbp), %r9
	movq $2, %r10
	xor %rdx, %rdx
	movq %r9, %rax
	idiv %r10
	movq %rax, %r9
	sub %r9, %r8
	movq %r8, -16(%rbp)
	movq -16(%rbp), %r8
	push %r8
	call print_i
	add $8, %rsp
	movq -8(%rbp), %r8
	movq -80(%rbp), %r9
	add %r9, %r8
	push %r8
	call fib
	add $8, %rsp
	movq %rax, %r8
	movq %r8, -96(%rbp)
	movq -96(%rbp), %r8
	movq %r8, -88(%rbp)
	movq -88(%rbp), %r8
	push %r8
	call print_i
	add $8, %rsp
	movq -88(%rbp), %r8
	movq %r8, %rax
	jmp _main_end
_main_end:
	add $96, %rsp
	mov %rbp, %rsp
	pop %rbp
	mov %rax, %rdi
	movq $60, %rax
	syscall
