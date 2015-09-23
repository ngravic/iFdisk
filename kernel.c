__asm__("jmp $0x8, $main");

#include "kernel.h"

static DiskInfo info_disks[2];
static Disk current;
static ushort base_port = BASE_PORT_PRIMARY_ATA;

void main() 
{   	    
	clear_screen();         
	write_string(0x4F, 0, "                                     iFDisk                                     ");   	
    init_disk();		     
    draw_partition_table(4, 15);
	while (1==1);
} 

void write_string(int colour, int pos, const char *string)
{
	volatile char *video = (volatile char*)(0xB8000 + 2 * pos);
	while( *string != 0 )
	{
		*video++ = *string++;
		*video++ = colour;
	}
}

void clear_screen()
{
	volatile char *video = (volatile char*)0xB8000;
	int c = 2000;
	while(c)
	{
		*video++ = ' ';
		*video++ = 0;
		c--;
	}	
}

void save_string(char *dest, const unsigned int size, const ushort *buffer, const FieldIdty field)
{
    unsigned int i;
    char string[size];

    if (buffer[field] == 0)
        return;

    for (i = 0; i < (size - 1) / 2; i++) {
        string[2 * i]     = buffer[field + i] >> 8;
        string[2 * i + 1] = buffer[field + i] & 0xFF;
    }
    string[2 * i] = '\0';

    trim_string(dest, string);
}

void detect_disk(ushort *buffer, const ubyte disco)
{    
    const ubyte mode = CHS_MODE | (disco & 1) << 4;

    write_port(base_port + 6, mode);
    write_port(base_port + 2, 0);
    write_port(base_port + 3, 0);
    write_port(base_port + 4, 0);
    write_port(base_port + 5, 0);
    write_port(base_port + 7, IDENTIFY_COMMAND);

    if (read_port(base_port + 7) == 0)
        return;

    for (;;) {
        const ubyte estado = read_port(base_port + 7);
        if (estado & BUSY_STATUS)
            break;
        if (estado & ERROR_STATE)
            return;
    }

    for (;;) {
        const ubyte estado = read_port(base_port + 7);
        if (estado & DRQ_STATUS)
            break;
        if (estado & ERROR_STATE)
            return;
    }

    port_read_block(base_port + 0, buffer, 256);

    info_disks[disco].is_present = 1;
    info_disks[disco].sectors = buffer[I_SECTORS] | ((uint) buffer[I_SECTORS + 1]) << 16;
    save_string(info_disks[disco].serial, sizeof info_disks[disco].serial, buffer, I_SERIAL);
    save_string(info_disks[disco].firmware, sizeof info_disks[disco].firmware, buffer, I_FIRMWARE);
    save_string(info_disks[disco].model, sizeof info_disks[disco].model, buffer, I_MODEL);
    
    current = disco;    
    ushort disk_data[256];
    int i, j;
    if(read_disk(0, 1, disk_data)) {        
        ubyte disk_data_char[512];
        word_to_byte(disk_data, 256, disk_data_char);
        for(i = 0, j = 446; i < 4; i++, j += 16) {
            int plength = disk_data_char[j + 12];
            plength = plength | (disk_data_char[j + 13] << 8);
            plength = plength | (disk_data_char[j + 14] << 16);
            plength = plength | (disk_data_char[j + 15] << 24);
            int rsec = disk_data_char[j + 8];
            rsec = rsec | (disk_data_char[j + 9] << 8);
            rsec = rsec | (disk_data_char[j + 10] << 16);
            rsec = rsec | (disk_data_char[j + 11] << 24);
            info_disks[disco].partitions[i].sys_id = disk_data_char[j + 4];        
            info_disks[disco].partitions[i].boot_indicator = disk_data_char[j];            
            info_disks[disco].partitions[i].rel_sector = rsec;
            info_disks[disco].partitions[i].size = plength;
        }        
    }
}

void init_disk()
{
    ushort buffer[256];
    
    write_port(BASE_PORT_PRIMARY_ATA + 3, 0x12);    
    if (read_port(BASE_PORT_PRIMARY_ATA + 3) == 0x12) {
        detect_disk(buffer, 0);
        detect_disk(buffer, 1);
    }    
}

int read_disk(const uint lba, ushort sectors_qty, ushort *block)
{    
    const ubyte modo = LBA28_MODE | (current & 1) << 4;
    unsigned int i;

    if (!base_port)
        return 0;

    write_port(base_port + 6, modo | (lba >> 24 & 0x0f));
    write_port(base_port, 0);    
    write_port(base_port + 2, sectors_qty);
    write_port(base_port + 3, lba);
    write_port(base_port + 4, lba >>  8);
    write_port(base_port + 5, lba >> 16);
    write_port(base_port + 7, READ_COMMAND);
    
    while(!(read_port(0x1f7) & DRQ_STATUS))
    {}
    
    for(i = 0; i < sectors_qty; i++) {
        port_read_block(base_port, block + (256 * i), 256);
    }        
    return 1;
}

unsigned int trim_string(char *dest, const char *src)
{
    unsigned int i;
    
    for (; *src == ' '; ++src);

    for (i = 0; src[i]; ++i)
        dest[i] = src[i];
    dest[i] = '\0';

    while (i != 0 && dest[i - 1] == ' ')
        dest[--i] = '\0';

    return i;
}

char* itoa(int i, char b[], int base) 
{
    char const digit[] = "0123456789ABCDEF";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{
        ++p;
        shifter = shifter/base;
    }while(shifter);

    do{
        *--p = digit[i%base];
        i = i/base;
    }while(i);
    return b;
}

char* ctoa(unsigned char i, char b[], int base) 
{
    char const digit[] = "0123456789ABCDEF";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{
        ++p;
        shifter = shifter/base;
    }while(shifter);
    
    do{
        *--p = digit[i%base];
        i = i/base;
    }while(i);
    return b;
}

void port_read_block(const ushort port, ushort *block, const unsigned int size)
{
    __asm__ __volatile__("repne insw" : "=m" (block) : "c" (size), "d" (port), "D" (block));
}

void word_to_byte(ushort *src, int sizeofsrc, ubyte *dest)
{
    int i;
    for(i = 0; i < sizeofsrc; i++) {
        dest[2 * i] = (ubyte)(((src[i]) << 8) >> 8);
        dest[(2 * i) + 1] =(ubyte)((src[i]) >> 8);
    }
}

void strcpy(char *src, char *dest, int length) 
{
    int i;    
    for(i = 0; i < length; i++) {
        dest[i] = src[i];
    }
}

void draw_partition_table(int line, int offset)
{
    int i, j, k;
    ushort disk_data[256];
    ubyte disk_data_char[512];
    write_string(0x1f, (80 * line) + offset, " #  boot  start      end        system    ");     
    for(i = 0, k = 1; i < 2; i++) {
        if(info_disks[i].is_present) {
            for(j = 0; j < 4; j++) {
                PartitionInfo pi;
                char toPrint[81] = "                                                                                \0";
                itoa(i, toPrint, 10);
                itoa(j, toPrint + 1, 10);                
                pi = info_disks[i].partitions[j];                                
                if(pi.boot_indicator)
                    toPrint[5] = '*';               
                itoa(pi.rel_sector, toPrint + 10, 10);
                itoa(pi.rel_sector + pi.size - 1, toPrint + 21, 10);                
                                
                if(pi.sys_id == 0x01) {                    
                    strcpy("FAT12", toPrint + 32, 5);                    
                }
                else if(pi.sys_id == 0x04 || pi.sys_id == 0x06 || pi.sys_id == 0x0e) {
                    strcpy("FAT16", toPrint + 32, 5);                    
                }
                else if(pi.sys_id == 0x82) {
                    strcpy("Linux swap", toPrint + 32, 10);
                }
                else if(pi.sys_id == 0x83) {
                    strcpy("Linux", toPrint + 32, 5);
                } 
                else if(pi.sys_id == 0x07) {
                    strcpy("NTFS", toPrint + 32, 4);
                } 
                else {
                    strcpy("Unknown", toPrint + 32, 7);
                }
                write_string(0x0f, (80 * (k + line)) + offset, toPrint);
                k++;
            }   
        }        
    }    
}