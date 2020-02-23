---
title: Back to HOSS
date: 2020-02-22 18:23:26
tags:
---
So it's been about a year since the last time I worked on HOSS, and I've decided to start playing around with it again.

The first issue I faced was that my toolchain no longer worked.  When I went to rebuild HOSS, I got an error from GCC that it could not find a library that it needed.  As you may remember, I was following a tutorial by [Carlos Fenollosa](https://github.com/cfenollosa/os-tutorial) that was specific to writing on OS on a Mac.  Among the various steps, he talked about building a cross-compiling version of GCC.  The version he used was 4.9, but I used a slightly later version, 6.x, for my initial HOSS work.  Even at the time I did this, GCC had advanced well beyond version 6, but I specifically chose to use an older version.  I don't quite remember why, other than a vague notion that for some reason later versions of GCC wouldn't work.  I don't recall why I thought this, but that's why I stuck with such an old version.

Well, when I got the missing library error, I was kind of screwed.  After doing some Googling, I didn't come up with anything definitive, but I got the impression that the library disappeared when I upgraded my version of mac OS.  So I decided to switch to the latest GCC.  I downloaded version 9.2, followed Fenollosa's instructions, and with only a few hiccups, everything worked.  The hiccups came from three libraries which I had installed with Brew, but their headers and libs weren't in the normal include/lib paths.  Fortunately the build process told me what to do, which was pass in some extra parameters, and once I did that everything worked.

So with my toolchain once again functioning, I rebuilt HOSS and started working on it again.

The next issue I ran into was that apparently my Bochs configuration was screwed up.  It was pointing to a file, `hoss-os`.img, but I had at some point switched to using the name, `os-image,` as my bootable image file.  But apparently I hadn't run a clean, so an older version of `hoss-os.img` was still hanging around.  This means that apparently I was running an outdated version of HOSS in Bochs.  This wasn't a huge deal, as I did most of my testing in QEMU anyway, but I was curious how I hadn't caught this before.  But I just changed the filename in my Bochs config, and everything was running again.

## Memory Managerment

So where I stopped before was in the middle of working on my memory manager.  I had written a simple memory manager using a simple First Fit algorithm, but I hadn't thoroughly tested it.  I have a command shell program.  It does very little, just presents a command prompt where you can type anything.  And it only supports a few commands, none of them actually useful for anything other than testing my memory manager.  Two commands, `alloc` and `free`, simply allocate 4k of ram and then free it.  You could call `alloc` multiple times, and it would allocate multiple 4k blocks, and it would display the allocated segment blocks.
![](images/alloc.png)
The problem was that `free` would only free the last allocated block of memory.  But that wouldn't test all the scenarios that I needed to test, of course.  So my first change was to make it so that it would free the allocated blocks in reverse order, i.e., you could call it multiple times and it would free the allocated blocks in the reverse order of how you allocated them.  Doing so and running some tests proved that my memory manager could allocate and free blocks without losing track.

But this didn't really cover a more real world scenario, of course.  I didn't know what would happen if I, say, allocated three blocks, and then freed the one in the middle.  The next time I allocated a 4k block, would it pick the one in the middle?  Would it add it to the end, skipping the empty block?  I needed to be able to arbitrarily free any memory block in whatever oder I chose.

This took more work than I expected.  The first issue was that my simple command interpreter really just did simple string matching to execute commands.  This meant that it couldn't support parameters, other than where the parameter was essentially part of the command.  For example, I had a command called `memmap` which would show me the memory map of usable memory that the system provided.  But I also had `memmap full`, which would also show unusable/reserved memory.  But this parameter was just done through simple string matching.  There was no real parsing going on.  But to get `free` to do what I needed, I would have to do some parsing.

Not a big deal, of course, but one of the aspects of writing your own operating system is that you have no library of functions to fall back on except whatever you write.  So deciding that `free` would accept an optional numeric parameter, which would be the index of the block you wanted to free, I ended up having to write my own versions of `strtok` and `atoi` and added these to my libc module.

Once I did that, updating my command shell was pretty simple, so that now I could do this:
![](images/alloc_and_free.png)