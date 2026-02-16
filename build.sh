i686-elf-as boot.s -o boot.o

i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -mgeneral-regs-only -mno-80387

i686-elf-gcc -T linker.ld -o myos -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

grub2-file --is-x86-multiboot myos

mkdir -p isodir/boot/grub
cp myos isodir/boot/myos
cp grub.cfg isodir/boot/grub/grub.cfg
grub2-mkrescue -o myos.iso isodir

qemu-system-i386 -cdrom myos.iso # -d int -no-reboot