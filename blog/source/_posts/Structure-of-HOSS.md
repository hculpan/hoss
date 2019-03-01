---
title: Structure of HOSS
date: 2019-02-23 12:04:02
tags:
---
![](images/dir_structure_1.png)
As I mentioned in my first post, HOSS is so far very heavily based on two different tutorials.  As a result, my structure largely matches that given in both of these, particularly Fenollosa's.  

My boot loader can be found in the /boot subdirectory.  I largely followed Blundell's tutorial here, with the main code being in boot_sect.asm.  As the extensions suggests, it is written in assembly and is assembled using NASM.  The other files here are just included in the main file, so they will not produce their own object files and thus don't have to be linked.  One change I made from Blundell's code is that I named the 16-bit print string procedure print_string_16, and the 32-bit version is just print_string.  At the time I was doing it, I thought I would be using the 32-bit version more, but of course once I boot, I start using the version developed in C (see screen.c in /drivers).

The boot process works as follows:
1. Basic 16-bit setup stuff (e.g., setup stack, print initial message).
2. Load next 15 sectors from disk into memory at 0x1000.  This loads the rest of our kernel.  This is just a very simple kernel loader.  It doesn't know anything about a file system or anything like that.  It just takes all the bytes from the 15 sectors after the first and puts them in memory.  If there aren't actually 15 sectors (which there aren't when building just the basic os-image), it just loads as much as it finds.
3. Create simple GDT structurs.  This is a flat structure that creates one Data and one Code segment, and both map to the same memory.
4. Switch to protected mode.
5. Call the kernel entry point, which was loaded in step 2 to memory address 0x1000.

Once the boot process has completed, the boot loader will hand off control to the kernel, starting with the kernel_main function in kernel/kernel.c.  When the kernel is built, it is compiled to start at an offset of 0x1000 because that's where the kernel is loaded by the boot process.  And it is compiled without linking any libraries from the development platform.  Thus everything has to be written from scratch without reusing any libraries that one would normally have available with C.  The kernel has it's process:
1. Print out a message so I know it's started (the kernel equivalent of "Hello, world!")
2. Setup the IDT and the ISR's.  To be honest, this is probably the one area of the boot/kernel that I am fuzziest on, at so far.  I understand the basic idea of an interrupt and that you need a routine to handle each interrupt.  As I understand the process, the Interrupt Descriptor Table (IDT) is a structure that basically informs the cpu what Interrupt Service Routine (ISR) to call for each interrupt.  Most call the same ISR, a generic routine that just prints out a message.  This is primarily for error interrupts (e.g., invalid opcode or general protection fault) that will never happen because my code never has bugs! :)  Obviously at some point, I'll need ISR's to better handle these situations, but for the moment this will do.  The only other important thing that happens here is that I setup an ISR for the timer and the keyboard.  I'll discuss more about the keyboard in a subsequent post.  As you might imagine, the code for the IDT and ISR configuration comes almost completely unchanged from Blundell.
3. A simple test loop that takes keyboard input and echoes it out to the screen.  This will be the beginning of my command shell, but for now it's in kernel.c.  This routine allows me to test out the screen and keyboard device drivers, which is what I'll discuss next.