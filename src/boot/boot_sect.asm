[org 0x7c00]
KERNEL_OFFSET equ 0x1000

[bits 16]
    jmp boot_start

BOOT_DRIVE      db  0

boot_start:
    mov [BOOT_DRIVE], dl

    mov bp, 0x9000
    mov sp, bp

    call do_e820

    mov bx, MSG_REAL_MODE
    call print_string_16

    call load_kernel

    call switch_to_pm

    jmp $

%include "print_string_16.asm"
%include "disk_load.asm"
%include "mem_check.asm"
%include "gdt.asm"
%include "print_string.asm"
%include "switch_to_pm.asm"

[bits 16]

load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print_string_16


    mov bx, KERNEL_OFFSET
    mov dh, 50
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

[bits 32]

BEGIN_PM:
    call KERNEL_OFFSET

    jmp $


; Global variables
MSG_REAL_MODE   db  "Started in 16-bit Real Mode", 0
MSG_LOAD_KERNEL db  "Loading kernel into memory", 0

times 510-($-$$) db 0
dw 0xaa55