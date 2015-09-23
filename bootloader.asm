[ORG 0x7C00]
[BITS 16]

%include "gdt.inc"

jmp loadKernel

loadKernel:        
        MOV AH, 0x02    ;Funcion leer sectores
        MOV AL, 0x0C    ;Numero de sectores a leer (AL = 3)
        XOR DL, DL      ;Ubicacion Floppy A (DL = 0)
        XOR CH, CH      ;Numero de cilindro (CH = 0)
        XOR DH, DH      ;Numero de cabeza (DH = 0)
        MOV CL, 0x02    ;Numero de sector (CL = 2)
        XOR BX, BX
        MOV ES, BX      
        MOV BX, 0x9000  ;Buffer de datos        
        INT 0x13        ;Funcion 0x13 de BIOS (Lectura de disco)
        JZ activar_linea_a20                      

activar_linea_a20:        
        IN      al, 0x92
        TEST    al, 2
        JNZ     cargar_modo_protegido
        OR      al, 2
        AND     al, 0xFE
        OUT     0x92, al

cargar_modo_protegido:
        ;; Pasamos a modo protegido de 32 bits, con segmentación plana pero
        ;; sin paginación.
        CLI
        LGDT    [gdt]
        MOV     eax, cr0
        OR      al, 1
        MOV     cr0, eax        
        ;; Hacemos un salto largo a la siguiente instrucción para alterar CS y
	;; limpiar la tubería de instrucciones de la CPU. CS todavía contiene
	;; una dirección de segmento de modo real, así que debemos cargarle un
        ;; selector de modo protegido con los fines de no tener restricciones
        ;; de direccionamiento y de establecer el tamaño por defecto de
        ;; operadores en 32 bits. Si bien CS tranquilamente se podría modificar
        ;; en el salto que hay más adelante, la tubería debe ser limpiada en
        ;; este preciso momento para asegurarse de que los registros de
	;; segmento que se cargan a continuación lo hagan en modo protegido.
        jmp     dword 0x8:protegido

        [BITS 32]
protegido:
        MOV     ax, 0x10
        MOV     ds, ax
        MOV     ss, ax
        MOV     esp, 0xFFFF
        MOVZX   edx, dl
        PUSH    edx
        MOV     ax, 0
        ;MOV     dword [0xb8000], 0x07690748        
        JMP     0x8:0x9000
        

gdt:    gdt_empezar gdt, gdt_fin
        gdt_descriptor 0xffffffff, 0, 0b10011010, 0b1100
        gdt_descriptor 0xffffffff, 0, 0b10010010, 0b1100        
gdt_fin:
        times   510 - ($ - $$) db 0
        dw      0xAA55
