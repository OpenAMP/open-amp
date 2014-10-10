/*************************************************************************
*
* FILE NAME
*
*      nucleus.ld
*
* DESCRIPTION
*
*      Linker description (RAM) file for the following:
*
*      BOARD:      Xilinx ZC702 EVK Board (Cortex A9)
*
*************************************************************************/

MEMORY
{
    ram :   ORIGIN = 0x10000000, LENGTH = 0x10000000
}

SECTIONS 

{    
    .text ALIGN(ORIGIN(ram), 0x100000):
    {
        _ld_text_start = . ;
        KEEP (*(esal_vectors)) ;
        KEEP (*(esal_code)) ;
        *(.text .text*) ;
        *(.glue_7t) ;
        *(.glue_7) ;
        *(.gnu.linkonce.t*)

        /* Place process mode-switching support. */
        KEEP (*(nutramp)) ;

        . = ALIGN(4) ;
        PROVIDE (_rtl_init_start = .) ;

        /* Place only C RTL support initializers. */
        KEEP (*libc.a:(.init_array*)) ;

        PROVIDE (_rtl_init_end = .) ;

        . = ALIGN(4) ;
        PROVIDE (_cxx_ctor_start = .);

        /* Place all C++ constructors (initializers). */
        *(.ctors);
        KEEP (*(.init_array*)) ;

        PROVIDE (_cxx_ctor_end = .);
        *(.dtors);
        LONG (0);

        _ld_text_end = . ;

    } > ram

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)

    } > ram

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;

    } > ram

    nusymstr :
    {
        KEEP(*(nusymstr)) ;

    } > ram

    nusymtab :
    {
        PROVIDE (_nu_symbol_table_start = .) ;
        KEEP(*(nusymtab)) ;
        LONG(0x00000000) ;

    } > ram

    nuksymstr :
    {
        KEEP(*(nuksymstr)) ;

    } > ram
    
    nuksymtab :
    {
        PROVIDE (_nu_ksymbol_table_start = .) ;
        KEEP(*(nuksymtab)) ;
        LONG(0x00000000) ;

    } > ram
    
    .rodata ALIGN(0x100000):
    {
        _ld_rodata_start = . ;
        *(.rodata*) ;
        _ld_rodata_end = . ;
        
    } > ram

    .rtl ALIGN(0x100000):
    {
        _ld_rtl_start = . ;
        *libc.a:lib_a-impure.o (.data)
        *libc.a:lib_a-mallocr.o (.data)
        *libc.a:lib_a-mallocr.o (.bss)
        *libc.a:lib_a-sglueg.o (.bss)
        *libc.a:lib_a-stdout.o (.bss)
        *libc.a:lib_a-gdtoa-gethex.o (COMMON)
        *libc.a:lib_a-reent.o (COMMON)
        _ld_rtl_end = . ;

    } > ram

    .data ALIGN(0x100000):
    {
        _ld_rom_data_start = . ;
        _ld_ram_data_start = . ;
        *(.data) ;
        *(.data*) ;
        *(.gnu.linkonce.d*)
        _ld_ram_data_end = . ;

    } > ram

    .bss (NOLOAD) :
    {
        _ld_bss_start = . ;
        *(.bss) ;
        *(.bss*) ;
        *(COMMON) ;
        _ld_bss_end = . ;

    } > ram
}
