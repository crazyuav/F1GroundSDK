OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(vectors)

/* Highest address of the user mode stack */
_estack = 0x2000A000;     /* end of RAM */
_physical_end_itcm = 0xFFFC;


MEMORY
{
    ITCM2                : ORIGIN = 0x00000000, LENGTH = 0x10000
    ITCM2_EXT_LMA        : ORIGIN = 0x00010000, LENGTH = 0x6000
    DTCM2                : ORIGIN = 0x20000000, LENGTH = 0xA000
    ITCM2_EXT_VMA        : ORIGIN = 0x2000A000, LENGTH = 0x6000
}

SECTIONS
{
    .vectors :
    {
        *(.isr_vectors)
        . = ALIGN(4);
    } > ITCM2

    .start :
    {
        *(.start*)
        . = ALIGN(4);
    } > ITCM2

    .text :
    {
        _text_start = . ;
        *(.text*)
        . = ALIGN(4);
    } > ITCM2

    __exidx_start = . ;
    .ARM.exidx :
    {
        *(.ARM.exidx*)
        . = ALIGN(4);
    } > ITCM2
    __exidx_end = . ;

    __extab_start = . ;
    .ARM.extab :
    {
        *(.ARM.extab*)
        . = ALIGN(4);
    } > ITCM2
    __extab_end = . ;

    .itcm_ext :
    {
        *(.h264*)
        . = ALIGN(4);
    } > ITCM2_EXT_VMA AT> ITCM2_EXT_LMA

    .rodata  :
    {
        *(.rodata*)
        . = ALIGN(4);
    } > ITCM2

    .eh_frame  :
    {
        *(*eh_frame*)
        . = ALIGN(4);
    }

    .jcr  :
    {
        *(*jcr*)
        . = ALIGN(4);
    }

    .preinit_array     :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(4);
    }  > ITCM2

    .init_array     :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(4);
    }  > ITCM2

    .fini_array     :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(4);
    }  > ITCM2

    _data_start = . ;
    .data :
    {
        *(.data*)
        . = ALIGN(4);
        *(.ramcode)
        . = ALIGN(4);
    } > DTCM2 AT> ITCM2

    .data_e :
    {
        _data_end = . ;
    } AT> ITCM2

    .itcm_e :
    {
        . = ALIGN(4);
        end_itcm = .;
    } > ITCM2

    .bss :
    {
        _bss_start = . ;
        *(.bss*)
        . = ALIGN(4);
        *(COMMON)
        . = ALIGN(4);
        _bss_end = . ;
    } > DTCM2

    end = . ;
}


