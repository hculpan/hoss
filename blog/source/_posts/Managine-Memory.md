---
title: Managing Memory and Bochs
date: 2019-03-10 10:34:21
tags:
---
Over the last week or so, I've been working on a memory manager.  It was the next fundamental step for my kernel, something I really needed to do before anything else, so I was anxious to get it done.

My first effort didn't work out that well and, in retrospect, was pretty stupid.  If you recall from my prior post, I had gotten the system memory map.  This gave me a view on what memory was available and what wasn't.  The way it appeared, all memory above 1mb was available in one continguous chunk, while below 1mb it was more fragmented.  So I made the easy choice that for heap memory, I would ignore everything below 1mb.

With that decided, I started in to writing my memory manager.  I created a structure called `Memory_Segment` that had basic information: The starting address of a segment, length, type (for the moment, just free, system, or user), and pointers to the left and right segment descriptor.  Deciding that I would limit allocation to 4k segments, I created a solid block of memory to store an array of these, with enough memory allocated to manage the entire available memory.  This block started at 1mb, with the actual free ram located above this system block.

This was a very poor approach though.  I had an array of structures, but because I wanted to do a buddy allocation scheme, I was also treating them as a tree.  Mixing a dynamic tree structure with an array was a bad idea, and predictably it failed.  I could allocate memory pretty easily, but freeing became onerous.

After a good night's sleep, I decided to scrap what I had and start over.  I came up with a far more rational - and, I suspect, more standard - approach.  Rather than storing the segment discriptor in a block set aside for that purpose, I decided to store the descriptor with the allocated memory as a header.  So I altered my descriptor structure to a simpler and shorter format, containing just the length, type, and pointer to previous.  My pointer to next could be calculated by simpling added the address of the descriptor with the size of the structure and length of that memory block.  This gave me a linked list of memory blocks, and made writing a first fit allocator pretty straightforward.  The only other thing I had to do was add a descriptor with type TYPE_EOM and length of 0 at the end of memory, which gave me a clean marker to end the list.

### Bochs
So far, I've been using QEMU almost exclusively for development, with a little testing in VirtualBox and an attempt in VMWare Fusion.  In the last, I get a CPU fault, which I've not bothered to try to track down yet.  With VirtualBox, I get a very different error.  Everything works, but it seems to lose some keyboard strokes.  I can type a letter and sometimes it works and sometimes nothing happens.

I didn't think too much about it, as everything worked just great in QEMU.  But for some reason I decided to get Bochs to work.  The issue with Bochs, for those who haven't used it, is there doesn't seem to be a simple way to start.  Obviously there's the documentation, but for someone who's trying to get something done, this is pretty onerous.  I finally found a video that discussed about editing and saving properties, and then another that discussed basic commands.  So with that, I pretty quickly had Bochs up and running with my os image.

But the thing was, it had the same keyboard behavior as I saw in VirtualBox.  When I ignored it earlier with VirtualBox, I kind of wrote it off to, "It works in QEMU, but not in VB, but I don't know which one is more correct behavior.  So worry about it later."  But with Bochs acting like VB, that kind of suggested that QEMU was probably not accurate.

I spent a while trying to track it down, and of course went straight to my interrupt handler routine for the keyboard.  At the moment, my keyboard device driver does two very basic things.  It manages the state of the Shift, Ctrl, and Alt keys, and it maps scancodes to ASCII characters.  It has two basic external methods, `waitForAscii` and `waitForScancode`.  Both are blocking, and they just loop until a key is pressed.  Obviously this won't be sufficient in the long term, but it meets my needs for now.
```
char waitForAscii() {
    char result = 0;
    while (!result) {
        result = last_ascii;
        last_ascii = 0;
        last_scancode = 0;
    }
    return result;
}
```
This is pretty simple.  The only thing that might need some explanation is the `last_ascii` and the `last_scancode` variables.  These are just driver-level variables that are set by the interrupt handler to the scancode and the mapped ascii code, if it maps to ascii.  Appropriate use of kprint statements allows me to eliminate the interrupt handler, so I can to this function next.  It took me a bit of thought, but I finally realized what was going on.  The interrupt handler could fire at any point in this function, including in between, say, the setting of `result` and resetting `last_ascii`.  This is, of course, very similar to problems with multithreading code, so I really should have seen this right away.  In any case, the answer was reasonably simple.  Just disable and enable interrupts at the appropriate times, as so:
```
    while (!result) {
        asm volatile("cli");
        result = last_ascii;
        last_ascii = 0;
        last_scancode = 0;
        asm volatile("sti");
    }
```
So now keyboard input works consistently in QEMU, VirtualBox, and Bochs.

But all is still not golden.  There's still the CPU fault in VMWare Fusion, of course, though for the moment I'm willing to ignore this.  More concerning is that I have a memory allocation problem with VB.  Right now, I have a little test in my shell that allows me to type `alloc` and it will just allocate 4k on the heap and display the resulting memory segment chain.  On QEMU and Bochs, I can seemingly do this all day long with no problem.  On VB, it locks up and often prints strange things on the screen.  A new problem to look at, but that's for another post.