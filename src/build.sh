#!/bin/bash

# build boot sector
nasm -f bin boot.asm -o boot.bin
nasm -f elf kernel_entry.asm -o kernel_entry.o

# build kernel
i386-elf-gcc -ffreestanding -c kernel.c -o kernel.o -m32
# /usr/local/opt/binutils/bin/gobjcopy --set-start 0x1000 -O binary kernel.o kernel.bin
i386-elf-ld -o kernel.bin -Ttext 0x1000 kernel_entry.o kernel.o --oformat binary

# combine
cat boot.bin kernel.bin > os-image

echo "Wrote image to os-image"
