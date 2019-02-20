#!/bin/bash

# build boot sector
nasm -f bin boot.asm -o boot.bin

# build kernel
gcc -ffreestanding -c kernel.c -o kernel.o -m32
/usr/local/opt/binutils/bin/gobjcopy --set-start 0x1000 -O binary kernel.o kernel.bin

# combine
cat boot.bin kernel.bin > os-image

echo "Wrote image to os-image"
