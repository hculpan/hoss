---
title: The Screen Device
date: 2019-02-24 13:38:53
tags:
---
![](images/screen_output_1.png)
The screen device driver was the first external device I had to interface with, and much of the code that I used came from either Blundell or Fenelossa, although I certainly added my own changes and refinements.

The screen device driver was instructive because, while it is simple, it is the first time that you really notice that you're in 32-bit Protected Mode.  The reason for this is because you lose access to BIOS.  In 16-bit mode, printing out some text on the display is as simple as populating a few registers and calling the appropriate interrupt.  

Here, for example, is my print_string_16.asm code:
```
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
```
If you're not used to Assembly code, this may be a little difficult to read, but essentially you call the routine by placing the memory address of the string you want to print into the BX register.  The routine then goes character by character, printing each one until it finds a \0 (the standard terminator for a C-style string).  At that point, it returns.  The hard work is done by the BIOS when it calls `int 10h`, which tells it to display the character in register AL to the screen.

But the BIOS is 16-bit code, so as soon as you switch to Protected Mode, it's not accessible.  There are two basic solutions to this.  First, you can switch back to 16-bit mode to call the BIOS, then switch back when you're done.  Or, second, you can interface with the device using data ports.  The latter option is the one I went with, as the former seemed more difficult.

When accessing the video in this manner, you can use DMA to write data to the screen.  For a VGA monitor, video memory starts at 0xb8000, so if you want to write an 'A' to the upper left corner, you just put an A in that memory address.  Simple.  But each screen location is actually represented by two bytes, not one.  The second byte is the attributes byte, which determines background color, foreground color, and whether it's blinking.  So to write to the next location on the screen, you'd have to write to location 0xb8002, not 0xb8001.  This method makes a lot of things simpler than the BIOS approach, especially if you want to be able to write text at specific locations on the screen with specific colors and such.  And text wrapping is automatically handled as well.  If you're text wraps around to the next line, this just happens because the memory is linear.  So 0xb09e is the end of the first line and 0xb0a0 is the first character of the next line.  

The base routine for this is `print_char` in drivers/screen.c.  This is a private routine to the device driver, so even the kernel can't call it.  It's just the primitive that all the other routines call.  And as with most of the rest of my code at this point, most of it comes from Blundell/Fenollosa.  It basically does of the work of taking a col/row location and converting that to a memory address, and it assigns the character and the attribute byte to the appropriate locations.  

The other big thing that `print_char` handles is the location of the cursor.  When a character is printed, `print_char` will automatically set the cursor to the next location on the screen.  It does this by using the data ports for the VGA display device.  In `cpu/ports.c` are a set of generic routines to read/write bytes and shorts to device ports.  To change the location of the cursor, it sets the offset (the calculated offset from 0xb8000 based on the column and row) to the device, with the high byte being written to port 0x3d4 and the low byte to 0x3d5.  To find the location of the cursor, it just reads the bytes from thexe same ports.  This feature comes in handy, because then we can provide routines that don't require a location.  You can print a message to the screen, and the cursor will move to the end of that message.  Then when you print another message, assuming you don't provide a specific location, it just uses the cursor location to figure out where to put the new message.

One weakness of writing to video memory, however, is scrolling.  When text hits the bottom of the screen, we expect that the text just scrolls upward.  Using DMA, there's no easy way to do this.  I wrote a function, handle_scrolling, that simply moves everything in video memory as if it were one line higher, then outputs the new bottom line.

The only other issue was the backspace.  Fenollosa had a routine, `kprint_backspace()`, that I largely used as is, but it didn't seem to quite work for me.  When I tested it, hitting the backspace created an inverse dot character; it didn't actually backspace.  I fixed that so it blanked out whatever character was then and moved the cursor back one full space.