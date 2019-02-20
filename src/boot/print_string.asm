[bits 32]
; Define some constants 
VIDEO_MEMORY equ 0xb8000 
WHITE_ON_BLACK equ 0x0f
; prints a null-terminated string pointed to by EDX
print_string:
    pusha
    mov edx, VIDEO_MEMORY   ; Set edx to the start of vid mem.

.loop:
    mov al, [ebx]           ; Store the char at EBX in AL 
    mov ah, WHITE_ON_BLACK  ; Store the attributes in AH
    
    cmp al, 0               ; if (al == 0), at end of string, so 
    je .done                ; jump to done

    mov [edx], ax           ; Store char and attributes at current
                            ; character cell.

    add ebx, 1              ; Increment EBX to the next char in string. 
    add edx, 2              ; Move to next character cell in vid mem.

    jmp .loop               ; loop around to print the next char.

.done:
    popa
    ret                     ; Return from the function


clear_screen:
    pusha
    mov edx, VIDEO_MEMORY
    mov bx, 2000

.loop:
    mov al, 32
    mov ah, WHITE_ON_BLACK

    cmp bx, 0
    je .done

    mov [edx], ax

    sub bx, 1
    add edx, 2

    jmp .loop

.done:
    popa
    ret