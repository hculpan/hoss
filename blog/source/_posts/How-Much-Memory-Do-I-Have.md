---
title: How Much Memory Do I Have?
date: 2019-03-01 10:20:20
tags:
---
Over the last two days, I've been contemplating my next steps with the OS, with the two most obvious choices being memory manager or floppy drive interface.  But pretty quickly I settled on the memory manager as the more fundamental component.  As the OS is now, I have full control over memory even in my "user" shell program (I haven't implemented any sort of protection yet), and with a 32-bit memory model, I clearly have enough space to load a whole floppy into memory many times over.  But I feel like I wouldn't get too far before I'd want to have some way to allocate and track memory.

With that in mind, I've been looking at various sources for an approach to memory management, but the best I've found is Tannenbaum's book, "Modern Operating Systemns".  Based on that, I've decided to go with a page allocator using the buddy algorithm.  This is just for physical memory, of course; I have no plans to implement virtual memory, as I can't imagine this project getting big enough that I'd ever need it.  When I launch QEMU to test HOSS, I'm already allocating 1gb.  This is way less than my computer actually has, of course, so I've got plenty of room to grow.  But at the moment, 1gb seems like more memory than I could possibly use.  So just physical ram it is!

The first step in constructing a memory manager, though, is to figure out how much ram I actually have.  My first attempt at googling brought me to a page discussing BIOS interrupt 15h, function E801h.  The nice thing is, this is quite simple and only took me a minute or two to add to my `boot/boot_sect.asm` file.  The only wrinkle I found here was that, since it's a BIOS call, I had to call it from my 16-bit boot loader, but how to get it to my 32-bit kernel?  I solved that just by adding a variable to my boot loader.  Since I wanted to be sure I knew what address it would be at, since I would have to hard code the address in my 32-bit kernel, I put it at the top of the boot loader, not the bottom.  Thus the top of `boot/boot_sect.asm` became:
```
[org 0x7c00]
KERNEL_OFFSET equ 0x1000

[bits 16]
    jmp boot_start

MEMORY_SIZE     dd 0x00000000
BOOT_DRIVE      db  0

boot_start:
```
By doing this, I knew that MEMORY_SIZE would be at 7C02h, because `jmp boot_start` when assembled would take up only 2 bytes.  This added the tiny overhead of a small jump, but since I will likely use FAT12/16 as my file system, I will need to put a structure at the top of the boot loader in any case.

With that done, I simply created an `int *` in my kernel and hard coded the address to 7C02h, and viola I had the amount of available ram.

But there's a wrinkle to this simple solution.  While it tells me how much ram is free, it doesn't tell me where it is.  At first I was going to hard code this, but then I found the [OS Dev Wiki](https://wiki.osdev.org/Main_Page) and it's explanation for [detecting memory](https://wiki.osdev.org/Detecting_Memory_(x86)).  It discussed function E801h, of course, but indicated that E820h is really the standard function for finding available information.  Rather than giving you a since amount of available memory, it gaves you a table of memory ranges, indicating which are usable and which are not.

E820h definitely took some time to understand, and even though they had two sample implementations (one in assembly, the other C), it took me a while to figure out the particulars.  But though the E820h call has a number of peculariaties, overall it's pretty straightforward.  You call the function repeatedly with each call giving you the next entry in the table.  Each entry gives you the starting address of the memory block, the length, and the type (1 = usable, anything else meaning not).  Using the example assembler code, I had an integer value (4 bytes) at 8000h, which was the number of entries.  The table was put right after this, with each entry being a total of 24 bytes in length.  Thus the first entry was at 8004h, the second at 801Ch, and so on.  So it was a simple matter in the kernel to hard code another `int *` to 8000h.  I then created a struct to match the layout of the table entries, created a pointer to the struct hard coded to 8004h, and then just treated the table as an array.  Very simple.

But it didn't quite work out.  I kept getting very odd values.  For example, my first memory block was a usable segment at address 0, but it was only 2 bytes long.  And the vast bulk of memory was in an unusable block, also starting at 0.  The documentation warned that there could be overlap, but this was ridiculous.  I did some further searching and found that others had had similar problems, and the resolution had been to pack the struct.  Basically the concern was that the optimizer was trying to align the 64-bit entries along 8-byte boundaries.  I tried adding `__attribute((packed))__`, but this didn't change anything.  I also tried breaking the 64-bit numbers into two 32-bit numbers, especially since the numbers I was dealing with would all fit easily in the lower portion.  But while that changed the numbers, they still were pretty screwed up.

My final solution was a bit of a kludge, but it worked.  Rather than using a struct, I just used pointer arithmetic.  So I set my base pointer address to 8004h.  As I looked through the entries, I just changed the base pointer to `8004h + (index * 24)`.  And then I'd add the offset for the specific value location.  For example, the length is the second 64-bit value in the entry, so it's address was `8004h + 8 + (index * 24)`.  Once I did that, everything worked.

![](images/mem_map.png)

My final bit of work on this was to add support to the shell for the commands `memmap` and `memmap full`, which showed the map of usable memory and of all memory, respectively.

At the end of all this, as I was doing final testing, I came across a bit of a vexing problem.  I had added a block in the kernel to display available memory as it started up.  I outputted this in decimal rather than hex, and I wanted it separated with commas, as it's easier to read.  This necessitated adding several new functions and code blocks.  None of it a big deal, except suddenly my shell stopped functioning correctly.  It stopped outputting the prompt `>` and the intro text, but you could still type and it would output what you typed.  I backed out all of my code, of course, and the problem went away.  But after scanning the code and seeing no issue, I put it back but removed the code that called it.  So it would be a part of my OS image, but not actually called.  My problem came back.  I concocted theories of memory being overwritten and that sort of thing, all of which turned out to lead nowhere.  What it turned out to be is that my boot loader would load up 15 sectors off the boot disk, but my os-image had grown to 25 sectors.  So it wasn't loaded a big chunk of it into memory.  Obviously the fix was simple, just load more sectors into memory.  Since it doesn't give an error if those sectors don't exist (i.e., if the os-image file is only 10k in size, ~20 sectors, but you say load 30, it will just load what it can find and give no error), I set it to load 50 sectors.  Honestly, I probably should have gone higher, but c'est la vie.  

If my kernel grows much larger, I'll probably start looking into doing a multi-stage loader (i.e., load boot sector first, which loads pre-kernel, which then loads final runtime kernel).  By the time I get to this point, though, I presumably will have a file system that I can read for my final kernel.