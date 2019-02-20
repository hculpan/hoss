all: os-image

run: all
	qemu-system-i386 -drive file=os-image,index=0,if=floppy,media=disk,format=raw

os-image: boot_sect.bin kernel.bin
	cat $^ > os-image

kernel.bin: kernel_entry.o kernel.o
	i386-elf-ld -o kernel.bin -Ttext 0x1000 $^ --oformat=binary

kernel.o: kernel.c
	i386-elf-gcc -ffreestanding -c $< -o $@

kernel_entry.o: kernel_entry.asm
	nasm $< -f elf -o $@

boot_sect.bin: boot_sect.asm
	nasm $< -f bin -o $@

clean:
	rm -fr *.bin *.dis *.o os-image *.map

kernel.dis: kernel.bin
	ndisasm -b 32 $< > $@