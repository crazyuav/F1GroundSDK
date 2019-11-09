OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(vectors)

/* Highest address of the user mode stack */
_estack = 0x20080000;     /* end of RAM */
_physical_end_itcm = 0x0007FFFC;

MEMORY
{
    ITCM0      : ORIGIN = 0x00000000, LENGTH = 0x80000
    DTCM0      : ORIGIN = 0x20000000, LENGTH = 0x80000
}

SECTIONS
{
    .vectors :
    {
        *(.isr_vectors)
        . = ALIGN(4);
    } > ITCM0

    .start :
    {
        *(.start*)
        . = ALIGN(4);
    } > ITCM0

    .text :
    {
        *(.text*)
        . = ALIGN(4);
    } > ITCM0

    __exidx_start = . ;
    .ARM.exidx :
    {
        *(.ARM.exidx*)
        . = ALIGN(4);
    } > ITCM0
    __exidx_end = . ;

    __extab_start = . ;
    .ARM.extab :
    {
        *(.ARM.extab*)
        . = ALIGN(4);
    } > ITCM0
    __extab_end = . ;

    .rodata :
    {
        *(.rodata*)
        . = ALIGN(4);
    } > ITCM0

    .eh_frame          :
    {
        *(*eh_frame*)
        . = ALIGN(4);
    } > ITCM0

    .jcr              :
    {
        *(*jcr*)
        . = ALIGN(4);
    } > ITCM0
    
    .preinit_array     :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(4);
    }  > ITCM0

    .init_array     :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(4);
    }  > ITCM0

    .fini_array     :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(4);
    }  > ITCM0

    _data_start = . ;
    .data :
    {
        *(.data*)
        . = ALIGN(4);
        *(.ramcode)
        . = ALIGN(4);
    } > DTCM0 AT> ITCM0

    .data_e :
    {
        _data_end = . ;
    } AT> ITCM0

    .itcm_e :
    {
        . = ALIGN(4);
        end_itcm = .;
    } > ITCM0

    .bss :
    {
        _bss_start = .;
        *(.bss*)
        . = ALIGN(4);
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;
    } > DTCM0

    end = . ;
}


