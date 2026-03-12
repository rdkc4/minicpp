.globl print_i
.type print_i, @function

print_i:
    push %rbp
    mov %rsp, %rbp
    sub $32, %rsp

    mov 16(%rbp), %r8
    mov $0, %r9
    mov $0, %rbx

    cmp $0, %r8
    jge .print_i_conversion_start

    mov $1, %r9
    neg %r8

.print_i_conversion_start:
    lea 31(%rsp), %rsi
    mov $10, %rcx

    dec %rsi
    movb $'\n', (%rsi)
    inc %rbx

    cmp $0, %r8
    jne .print_i_loop
    movb $'0', -1(%rsi)
    dec %rsi
    inc %rbx
    jmp .print_i_add_sign

.print_i_loop:
    xor %rdx, %rdx
    mov %r8, %rax
    div %rcx

    add $'0', %dl
    dec %rsi
    mov %dl, (%rsi)
    inc %rbx
    mov %rax, %r8

    test %rax, %rax
    jnz .print_i_loop

.print_i_add_sign:
    cmp $1, %r9
    jne .print_i_done
    dec %rsi
    movb $'-', (%rsi)
    inc %rbx

.print_i_done:
    mov $1, %rax
    mov $1, %rdi
    mov %rbx, %rdx
    syscall

.print_i_ret:
    mov %rbp, %rsp
    pop %rbp
    ret

.globl print_u
.type print_u, @function
print_u:
    push %rbp
    mov %rsp, %rbp
    sub $32, %rsp

    mov 16(%rbp), %r8
    mov $0, %rbx

    lea 31(%rsp), %rsi
    mov $10, %rcx

    dec %rsi
    movb $'\n', (%rsi)
    inc %rbx

    cmp $0, %r8
    jne .print_u_loop

    movb $'0', -1(%rsi)
    dec %rsi
    inc %rbx
    jmp .print_u_done

.print_u_loop:
    xor %rdx, %rdx
    mov %r8, %rax
    div %rcx

    add $'0', %dl
    dec %rsi
    mov %dl, (%rsi)
    inc %rbx

    mov %rax, %r8
    test %rax, %rax
    jnz .print_u_loop

.print_u_done:
    mov $1, %rax
    mov $1, %rdi
    mov %rbx, %rdx
    syscall

    mov %rbp, %rsp
    pop %rbp
    ret

.section .note.GNU-stack,"",@progbits