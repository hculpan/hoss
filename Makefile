C_SOURCES = $(wildcard src/kernel/*.c src/drivers/*.c src/cpu/*.c src/libc/*.c)
HEADERS = $(wildcard src/kernel/*.h src/drivers/*.h src/cpu/*.h src/libc/*.h)

OBJ = ${C_SOURCES:.c=.o src/cpu/interrupt.o}

CFLAGS = -g -m32 -fno-exceptions -Wall -Wextra -ffreestanding

CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
LD = /usr/local/i386elfgcc/bin/i386-elf-ld

all: os-image

run: all
	qemu-system-i386 -drive file=os-image,index=0,if=floppy,media=disk,format=raw

os-image: src/boot/boot_sect.bin kernel.bin
	cat $^ > os-image

kernel.bin: src/kernel/kernel_entry.o ${OBJ}
	echo ${OBJ}
	${LD} -o $@ -Ttext 0x1000 $^ --oformat binary

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -I './16bit' -I './boot' -o $@

clean:
	rm -fr *.bin *.dis *.o os-image *.map
	rm -fr src/kernel/*.o src/boot/*.bin src/drivers/*.o src/cpu/*.o src/libc/*.o

kernel.dis: kernel.bin
	ndisasm -b 32 $< > $@
