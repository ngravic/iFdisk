typedef unsigned short int ushort;
typedef unsigned char ubyte;
typedef unsigned int uint;

#define BASE_PORT_PRIMARY_ATA 0x1F0

#define READ_COMMAND 0x20
#define IDENTIFY_COMMAND 0xEC

#define ERROR_STATE 0x01
#define DRQ_STATUS 0x08
#define BUSY_STATUS 0x40

#define CHS_MODE 0xA0
#define LBA28_MODE 0xE0

typedef enum {
    I_SERIAL    = 10,
    I_FIRMWARE = 23,
    I_MODEL   = 27,
    I_SECTORS = 60
} FieldIdty;

typedef struct {
    char boot_indicator;    
    ubyte sys_id;    
    uint rel_sector;
    uint size;
} PartitionInfo;

typedef struct {    
    int is_present;    
    int sectors;    
    char serial[21];    
    char firmware[9];    
    char model[41];
    PartitionInfo partitions[4];
} DiskInfo;

typedef enum {
    PRIMARY_MASTER = 0,
    PRIMARY_SLAVE,
    SECONDARY_MASTER,
    SECODARY_SLAVE
} Disk;


/*
    http://wiki.osdev.org/ATA_PIO_Mode#x86_Directions
    Retorna 1 si todo OK, 0 de lo contrario
    Guarda en (block) la cantidad de sectores (sectors_qty) del disco desde la direccion LBA (lba)
*/
int read_disk(const uint lba, ushort sectors_qty, ushort *block);

/*
    Guarda en (block) la info obtenida del puerto (port) de tamanio (size)
*/
void port_read_block(const ushort port, ushort *block, const unsigned int size);

/*
    Retorna la longitud del nuevo string
    Recorta el string (src) y lo guarda en (dest)
*/
unsigned int trim_string(char *dest, const char *src);

/*
    Escribe en la direccion 0xB800 el string (string) de color (colour) en la posicion (pos)
*/
void write_string(int colour, int pos, const char *string);

/*
    Checkea los discos presentes y guarda su informacion en (info_disks)
*/
void init_disk();

/*
    Guarda en (dest) la informacion del disco en el campo (field) obtenida desde (buffer) con tamaño (size)
*/
void save_string(char *dest, const unsigned int size, const ushort *buffer, const FieldIdty field);

/*
    Limpia la pantalla
*/
void clear_screen();

/*
    Retorna el buffer b 
    Convierte el numero (i) de base (base) a string, y lo guarda en (b)
*/
char* itoa(int i, char b[], int base);

/*
    Similar a itoa, pero con bytes sin signo
*/
char* ctoa(unsigned char i, char b[], int base);

/*
    Convierte un arreglo de enteros cortos sin signo a un arreglo de bytes sin signo
*/
void word_to_byte(ushort *src, int sizeofsrc, ubyte *dest);

/*
    Copia los (length) primeros bytes de (src) a (dest)
*/
void strcpy(char *src, char *dest, int length);

/*
    Dibuja la informacion de las particiones de los discos primarios en la linea (line) con un corrimiento de izq a derecha (offset)
*/
void draw_partition_table(int line, int offset);

/*
    Escribe en el puerto (port) el byte sin signo (data)
*/
inline void write_port(const ushort port, const ubyte data)
{
    __asm__ __volatile__("out %%al, %%dx" : : "a" (data), "d" (port));
}

/*
    Retorna lo leido en el puerto
    Lee el puerto (port)
*/
inline ubyte read_port(const ushort port)
{
    ubyte result;
    __asm__ __volatile__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}