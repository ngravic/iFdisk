# iFdisk
iFdisk es una pequeña utilidad booteable el cual muestra la informacion del disco y la muestra en pantalla, diseñado para i386

# Requerimientos
Para compilar y ejecutar el sistema se necesita tener instalado NASM para el bootloader, GCC para el kernel y QEMU como VM (Virtual Machine).


# Compilación y uso
Para la compilación se utilizará el archivo Makefile del trabajo. Para ello se deberá abrir una consola, dirigirse al directorio del TP y ejecutar la sentencia make. Se generarán 6 archivos (kernel.o, kernel.bin, kernel.sym, kernel.elf, boot.bin y floppy.img), donde floppy.img es la imagen de disco que correrá el sistema y se podrá ejecutar con la sentencia make run en la terminal. 
Una vez ejecutado, el sistema se verá de la siguiente manera:

Hasta el momento, sólo se soportan 5 tipos de particiones (FAT12, FAT16, Linux swap, Linux y NTFS) a modo de ejemplo. Las particiones no soportadas se mostrarán como Unknown.

# Funcionamiento
El funcionamiento se basa en los siguientes archivos:
bootloader.asm:
	Este archivo es el primero en ejecutarse. El mismo contiene instrucciones para cargar el modo protegido y el kernel. 
kernel.c:
	En él están los cuerpos de las funciones necesarias para leer el disco, escribir en pantalla y otras funciones de anexo.
kernel.h:
	En este archivo están los headers de todas las funciones que se presentan en kernel.c y las estructuras de datos de los discos.
kernel.ld:
	Este archivo es necesario para la alineación de las direcciones de memoria, requerido por el linker.
gdt.inc:
	Aquí se encuentra las macros para la construcción de la GDT

# Bibliografía
http://wiki.osdev.org/

http://www.osdever.net/

http://en.wikipedia.org/wiki/Master_boot_record

http://www.codeproject.com/Articles/45788/The-Real-Protected-Long-mode-assembly-tutorial-for

http://ftp.gnu.org/old-gnu/Manuals/gas-2.9.1/html_node/as_203.html

http://geezer.osdevbrasil.net/os/

http://viralpatel.net/taj/tutorial/booting.php

http://mikeos.sourceforge.net/

https://thiscouldbebetter.wordpress.com/2011/03/17/entering-protected-mode-from-assembly/

http://averstak.tripod.com/fatdox/partition.htm

http://davidjwalling.com/pro_os_008.html

http://www.monstersoft.com/tutorial1/PM_intro.html

http://thestarman.pcministry.com/asm/mbr/GRUB.htm

http://www.brokenthorn.com/Resources/OSDev5.html

http://members.tripod.com/protected_mode/alexfru/pmtuts.html

http://www.shikadi.net/moddingwiki/B800_Text

http://faydoc.tripod.com/cpu/div.htm

http://es.wikipedia.org/wiki/Int_13h#INT_13h_AH.3D02h_-_Leer_sectores

http://inglorion.net/documents/tutorials/x86ostut/simple_kernel/

http://julu.sg1005.myweb.hinet.net/Share/20110906-Writing%20Boot%20Loader%20with%20GAS%20Assembly.pdf


