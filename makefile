all: main
	
main: kernel.bin boot.bin	
	dd if=boot.bin of=floppy.img
	dd if=kernel.bin seek=1 of=floppy.img	

kernel.bin: kernel.c kernel.ld 	
	gcc -S -m32 -ffreestanding kernel.c 
	gcc -c -g -m32 -ffreestanding kernel.c -o kernel.o 	
	ld -m elf_i386 -static -T kernel.ld -nostdlib -o kernel.elf kernel.o
	objcopy -O binary kernel.elf kernel.bin	

boot.bin: 
	nasm -f bin bootloader.asm -o boot.bin

clean:
	rm -f boot.bin
	rm -f kernel.bin
	rm -f kernel.elf
	rm -f kernel.o	
	rm -f disk.o
	rm -f utils.o
	rm -f ports.o
run:
	#Debug data
	objcopy --only-keep-debug kernel.elf kernel.sym
	objcopy --strip-debug kernel.elf
	objcopy -O binary kernel.elf kernel.bin 	
	#Run the vm
	qemu-system-i386 -s -fda floppy.img -hda disk.img -boot order=a -monitor stdio -m 16 -enable-kvm 