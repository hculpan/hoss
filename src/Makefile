C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h)

OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o}

CFLAGS = -g -m32 -fno-exceptions -Wall -Wextra -ffreestanding

all: os-image

run: all
	qemu-system-i386 -drive file=os-image,index=0,if=floppy,media=disk,format=raw

os-image: boot/boot_sect.bin kernel.bin
	cat $^ > os-image

kernel.bin: kernel/kernel_entry.o ${OBJ}
	echo ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

%.o: %.c ${HEADERS}
	i386-elf-gcc ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -I './16bit' -I './boot' -o $@

clean:
	rm -fr *.bin *.dis *.o os-image *.map
	rm -fr kernel/*.o boot/*.bin drivers/*.o cpu/*.o libc/*.o

kernel.dis: kernel.bin
	ndisasm -b 32 $< > $@