[bits 16]

print_string_16:			; Routine: output string in SI to screen
	mov ah, 0Eh		        ; int 10h 'print char' function

.repeat:
	mov al, [bx]
	cmp al, 0
	je .done		        ; If char is zero, end of string
	int 10h			        ; Otherwise, print it
    add bx, 1
	jmp .repeat

.done:
	ret
