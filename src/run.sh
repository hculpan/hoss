#!/bin/bash

# nasm -f bin boot.asm -o boot.bin
qemu-system-i386 -drive file=os-image,index=0,if=floppy,media=disk,format=raw
