---
title: Introduction to HOSS
date: 2019-02-27 10:50:01
tags:
---
# HOSS
### Harry's Operating System Software
This is my attempt at writing an operating system.  My ambitions are low.  They do not
extend beyond learning, experimenting, and having fun.  Writing a full operating
system has been something I've wanted to do for a long time, and for whatever
reason I've finally decided to do so.

The original code was based on Nick Blundell's unfinished book, "Writing a Simple
Operating System -- from Scratch".  I found the pdf version online, and found
it to be a good, relatively simple (as simple as writing an OS boot loader can
be), and thorough introduction to getting an operating system booting on the 
i386 platform.  The boot code is largely based on this tutorial.

Blundell's work had two big problems, however.  First, and most
importantly, it wasn't completed.  In fact, he had entires in the
table of contents that had no corresponding text.  Clearly he had
meant to write more, but life intrudes for us all.

The second big issue for me is that Blundell assumes the reader is
using a Linux system.  I am using Mac OS, however.  Much of it
worked out of the box (e.g., nasm), but other things did not 
(e.g., linking multiple files).  For some of the issues, I was able
to translate to the Mac, but it became more and more difficult.

Fortunately, I found a github repository made by [Carlos Fenelossa]
(https://github.com/cfenollosa/os-tutorial).  This was a set of
tutorials for writing an operating system with the Mac as the 
development platform.  It was largely based on Blundell's tutorial,
so his code was very compatible with what I already had.  And
he had solved most of the problems I was already wrestline with.
The only downside was that his tutorials were a little out of date.
This only caused one issue, when building the cross-compiler.  The
tutorial specified version 4.9 of gcc, and it wasn't clear to me
if this was intentional or just the latest at the time.  But
the issue was sort of settled for me, because gcc 4.9 wouldn't 
build.  I ended up upgrading to 6.5, and everything worked just fine.

Like Blundell, Fenelossa's tutorial isn't complete.  By the time
you finish, you'll have a booting OS that does basic screen output
and can accept all-caps text input, but does nothing with it.  But
it's a very good start, and enough to get me working on a more
complete kernel.

With that done, I can discuss the basic layout of HOSS!