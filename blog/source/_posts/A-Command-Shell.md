---
title: A Command Shell
date: 2019-02-28 16:16:19
tags:
---
As I mentioned in my last post, the keyboard driver that Fenollosa gave in his tutorial had basic test code that would track a buffer, output the keys as pressed to the display, and process the simple command `END`.  I refactored that code to move it out of the keyboard driver, paring it down just to what was needed to support the keyboard.  While at this point I obviously don't have applications or a way for them to call an OS-level API, I still wanted to encapsulate the keyboard so that it would be simple for calling processes to access.

The result of this is that I refactored all the extraneous code out of the keyboard driver, and originally gave it a new home in the `kernel_main` function.  But obviously this really isn't part of the kernel.

I created a new directory, `shell`, with a new main file, `shell.c`.  Thus I started the beginning of my OS's main command shell.  

Right now, it's not much.  It still has the same little typewriter-like functionality that the original keyboard driver had.  But I've added a command, `scancode`, that will put the shell into a mode where it will output the scancode of whatever character is pressed or released.

While doing this, I also went ahead and addressed some of the shortcomings of my keyboard driver.  I added a `waitForScancode` function, so a user program can get any key.  I also added flags for the Ctrl, Alt, and CapsLock keys.  The Caps Lock key turned out to be a bit more difficult than I had anticipated.  Unlike the other keys, the Caps Lock really only effects a subset of keys, `a` through `z`.  But the numeric keys, for example, aren't effected.  Even further, if you have Caps Lock enabled but then hold Shift, that reverts a letter to lowercase.  None of this is inherent to the keyboard, of course, and I could have chosen to do it differently in my OS, but I'm a strong believer in standard keyboards.  One of my biggest annoyances when playing with old computers like the Apple II and Commodore 64 is their non-standard keyboards.  So to fully support all this functionality turned out to be a bit trickier than I thought.  My scancode-to-ASCII conversion logic probably doubled in size.  But it wasn't that big to begin with, so it's not a big deal.  Just took a number of code-compile-test cycles to finally get it all right.

The only things left on the keyboard driver are to turn off the Caps Lock indicator on startup (right now, the driver assumes Caps Lock is off), and add application functions for polling.