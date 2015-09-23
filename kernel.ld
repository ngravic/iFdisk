SECTIONS {   
    . = 0x9000;     
    .text : {
        * (.text)
    }        
    .rodata : {
        * (.rodata)        
    } 
    .data :  {
        * (.data)        
    } 
    .bss : {
        * (COMMON)
        * (.bss)
    }
}
