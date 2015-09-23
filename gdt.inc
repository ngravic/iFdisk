;;; Empezar la GDT con el espacio reservado para el descriptor nulo. En el
;;; mismo, guardar el tamaño y la dirección de la GDT, para pasarle a la
;;; instrucción `lgdt'.
;;; Parámetros: %1 <- dirección de inicio de la GDT
;;;             %2 <- dirección de fin de la GDT
%macro gdt_empezar 2
        dw      %2 - %1 - 1
        dd      %1
        dw      0
%endmacro

;;; Construir un descriptor, es decir, una entrada de una GDT, LDT o IDT.
;;; Parámetros: %1 <- límite  (20 bits)
;;;             %2 <- base    (32 bits)
;;;             %3 <- acceso   (8 bits)
;;;             %4 <- banderas (4 bits)
%macro gdt_descriptor 4
        dw      %1 & 0xFFFF
        dw      %2 & 0xFFFF
        db      %2 >> 16 & 0xFF
        db      %3
        db      %4 << 4 | %1 >> 16 & 0xF
        db      %2 >> 24 & 0xFF
%endmacro
