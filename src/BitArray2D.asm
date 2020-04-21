BitArray2D.asm:
        global  set_bit_elem
        global  get_bit_elem
        section .text

set_bit_elem:
        push rbp                   ; save the base pointer on the stack (at rsp+8)
        mov rbp, rsp        ; set up the rbp for the bottom of this frame

        ; rdi contains array pointer
        ; rsi contains row width
        ; rdx contains row
        ; rcx contains col

        mov rbx, rdx    ; index = row
        imul rbx, rsi   ; index = row * width
        add rbx, rcx   ; index = (row * width) + column

        mov r13, rbx    ; byte_offset = index
        sar r13, 3      ; byte_offset = index / 8

        mov r8, rbx     ; bit_offset = index
        mov r9, r13     ; temp = byte_offset
        imul r9, 8      ; temp = byte_offset * 8
        sub r8, r9      ; bit_offset = index - temp

        mov r10, 8      ; mask = 8
        sub r10, r8     ; mask = 8 - bit_offset
        dec r10         ; mask = 8 - bit_offset - 1
        mov r11, 0      ; cntr start at 0
        mov r12, 1      ; r12 holds masked byte
        jmp l1          ; enter loop

l1:
        cmp r11, r10     ; r11 (cntr) r10 (mask value)
        jge end           ; jump out if r11 >= r10
        inc r11          ; enter loop body, increment counter
        sal r12, 1       ; pad right with 0
        jmp l1

end:
        imul rdx, 10    ; multiply row by 10
        add rdx, rcx    ; add col value to row
        sar rdx, 3      ; divide by 8
        mov rbx, [rdi+rdx]  ; get byte at address, put in rbx
        or rbx, r12         ; or byte with mask
        mov [rdi+rdx], rbx  ; move or output into the byte spot
        mov rax, rbx

        mov rsp, rbp        ; restore stack pointer to before we pushed parameters onto the stack
        pop rbp             ; remove rbp from the stack to restore rsp to initial value
        ret                 ; return value in rax




get_bit_elem:
        push rbp            ; save the base pointer on the stack (at rsp+8)
        mov rbp, rsp        ; set up the rbp for the bottom of this frame

        ; rdi contains array pointer
        ; rsi contains row width
        ; rdx contains row
        ; rcx contains col

        ; add your code here - for now returning 0
        mov rbx, rdx    ; index = row
        imul rbx, rsi   ; index = row * width
        add rbx, rcx   ; index = (row * width) + column

        mov r13, rbx    ; byte_offset = index
        sar r13, 3      ; byte_offset = index / 8

        mov r8, rbx     ; bit_offset = index
        mov r9, r13     ; temp = byte_offset
        imul r9, 8      ; temp = byte_offset * 8
        sub r8, r9      ; bit_offset = index - temp

        mov r10, 8      ; mask = 8
        sub r10, r8     ; mask = 8 - bit_offset
        dec r10         ; mask = 8 - bit_offset - 1
        mov r11, 0      ; cntr start at 0
        mov r12, 1      ; r12 holds masked byte
        jmp l2

l2:
        cmp r11, r10     ; r11 (cntr) r10 (mask value)
        jge end2           ; jump out if r11 >= r10
        inc r11          ; enter loop body, increment counter
        sal r12, 1       ; pad right with 0
        jmp l2

end2:
        imul rdx, 10    ; multiply row by 10
        add rdx, rcx    ; add col value to row
        sar rdx, 3      ; divide by 8
        mov rbx, [rdi+rdx]  ; get byte at address, put in rbx
        and rbx, r12         ; and byte with mask
        cmp rbx, 0
        je noship
        jg ship


finish:
        mov rsp, rbp        ; restore stack pointer to before we pushed parameters onto the stack
        pop rbp             ; remove rbp from the stack to restore rsp to initial value
        ret                ; return value in rax

ship:
    mov rax, 1
    jmp finish
noship:
    mov rax, 0
    jmp finish