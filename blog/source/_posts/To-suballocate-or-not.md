---
title: To suballocate or not
date: 2020-02-24 10:15:04
tags:
---
I've been giving the topic of memory management and suballocators quite a bit of thought.  My initial thought was to write a chunk allocator at the OS level, and then a suballocator at the application level.  But I'm beginning to think that's the wrong approach.  There's the old programmer's saying, "Premature optimization is the root of all evil."  And I think that applies here.

My memory manager has a lot of areas for optimization.  It has a 9-byte header, for example.  Now this is fine if you're allocating a large chunk of ram, but what if you're just wanting a pointer to a character or boolean?  That's 900% overhead.  And then there's the issue that it's all in a big linked list, so every time you want to allocate memory, it's got to walk through the list until it finds the first segment that fits what you need.  And, of course, the First Fit algorithm is absolutely going to lead to memory fragmentation so that it will become less efficient as time goes.

But here's the thing.  The 900% overhead may sound bad, but how often do you want a pointer to a byte or a char?  Not very often.  Sure, you use chars, bytes, and booleans all the time, but usually it's a local variable.  You don't usually want them on the heap.  It's usually only arrays of these types that you'd allocate dynamically, and then it's a 9 byte overhead for the whole chunk, not each individual element.  Structs, of course, present a bit more complication, since one will often want to dynamically construct these individually, but it's really only the smallest of these that will present any issues.  

But let's be realistic here.  This is a toy project.  HOSS will never see production use, and it is highly unlikely it will be used by anyone other than myself.  And the size of the applications that will ever run on it will not likely ever be large enough that it will run out of ram even with the large overhead.

So for now I'm thinking I will just stick with my memory manager as is, and if the time comes that it's not working well enough, I can go back and optimize it then.