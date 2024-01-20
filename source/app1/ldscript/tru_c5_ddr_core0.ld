OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(reset_handler)

/* These must match the linker file of the corresponding core 1 program */
_CORE1_RAM_BASE = 0x0;
_CORE1_RAM_SIZE = 64M;

/* Location and size of core 0 program */
_CORE0_RAM_BASE = _CORE1_RAM_BASE + _CORE1_RAM_SIZE;
_CORE0_RAM_SIZE = 64M;

/* Ensure these stack sizes are aligned to 8 bytes */
_SYS_STACK_SIZE = 8192;
_UND_STACK_SIZE = 4096;
_ABT_STACK_SIZE = 4096;
_SVC_STACK_SIZE = 4096;
_IRQ_STACK_SIZE = 4096;
_FIQ_STACK_SIZE = 4096;

MEMORY {
    _CORE0_RAM (rwx) : ORIGIN = _CORE0_RAM_BASE, LENGTH = _CORE0_RAM_SIZE
}

/* A solution to the linker warning of first load segment having rwx is to manually create the program headers with the correct segment flags */
/* Without this, the linker will create default LOAD segment having flags specified by the MEMORY, i.e. rwx flags above */
/* FLAGS bits: bit2 = read    (r)           */
/*             bit1 = write   (w)           */
/*             bit0 = execute (x)           */
/* Examples: 4 = r, 5 = rx, 6 = rw, 7 = rwx */
PHDRS {
    _LOAD_RX PT_LOAD FLAGS(5);
    _LOAD_RW PT_LOAD FLAGS(6);
}

SECTIONS {
    .vectors : {
        _vectors = .;
        
        KEEP(*(.vectors))
        
        _evectors = .;
    } > _CORE0_RAM : _LOAD_RX

    .text : {
        . = ALIGN(4);
        _text = .;
        
        *(.text)
        *(.text.*)
        *(.gnu.linkonce.t.*)
        *(.gnu.linkonce.r.*)
        *(.gnu.warning)
        *(.glue_7t)
        *(.glue_7)
        *(.gcc_except_table)
        
        KEEP(*(.init))
        KEEP(*(.fini))
        
        . = ALIGN(4);
        _etext = .;
    } > _CORE0_RAM : _LOAD_RX

    .rodata : {
        . = ALIGN(4);
        *(.rodata)     /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)    /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(4);
    } > _CORE0_RAM : _LOAD_RX
    
    /* MMU L1 and L2 translation table */
    .mmu_ttb (ALIGN(16384)) : {
        _TTB_BASE = .;
        *(MMU_TTB)
        _TTB_LIMIT = .;
    } > _CORE0_RAM : _LOAD_RX

    .ARM.extab : {
        . = ALIGN(4);
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        . = ALIGN(4);
    } > _CORE0_RAM : _LOAD_RX

    .ARM.exidx : {
        . = ALIGN(4);
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
        . = ALIGN(4);
    } > _CORE0_RAM : _LOAD_RX

    /* C++ runtime: static constructors */
    .ctors : {
        . = ALIGN(4);
        KEEP(*crtbegin.o(.ctors))
        KEEP(*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP(*(SORT(.ctors.*)))
        KEEP(*crtend.o(.ctors))
        . = ALIGN(4);
    } > _CORE0_RAM : _LOAD_RX

    /* C++ runtime: static destructors and atexit() */
    .dtors : {
         . = ALIGN(4);
        KEEP(*crtbegin.o(.dtors))
        KEEP(*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP(*(SORT(.dtors.*)))
        KEEP(*crtend.o(.dtors))
        . = ALIGN(4);
    } > _CORE0_RAM : _LOAD_RX

    .preinit_array : {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(4);
    } > _CORE0_RAM : _LOAD_RX

    .init_array : {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(4);
    } > _CORE0_RAM : _LOAD_RX

    .fini_array : {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(4);
    } > _CORE0_RAM : _LOAD_RX

    .eh_frame_hdr : {
        . = ALIGN(4);
        KEEP(*(.eh_frame_hdr))
        *(.eh_frame_entry)
        *(.eh_frame_entry.*)
        . = ALIGN(4);
    } > _CORE0_RAM : _LOAD_RX

    .eh_frame : {
        . = ALIGN(4);
        KEEP(*(.eh_frame))
        *(.eh_frame.*)
        . = ALIGN(4);
    } > _CORE0_RAM : _LOAD_RX

    .data : {
        . = ALIGN(4);
        _data_start = .;  /* User defined symbol */
        
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d.*)
        
        . = ALIGN(4);
        _data_end = .;  /* User defined symbol */
    } > _CORE0_RAM : _LOAD_RW

    .bss (NOLOAD) : {
        . = ALIGN(4);
        __bss_start = .;
        __bss_start__ = .;
        
        *(.bss)
        *(.bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        
        . = ALIGN(4);
        _bss_end__ = .;
        __bss_end__ = .;
        
        /* End of all global variables */
        PROVIDE(end = .);  /* Used by newlib's syscalls */
        __end__ = .;       /* Used by newlib's semihosting */
        _end = .;
    } > _CORE0_RAM : _LOAD_RW

    .heap (NOLOAD) : {
        . = ALIGN(4);
        _heap_start = .;  /* User defined symbol */
        
        *(.heap*)
        . = ORIGIN(_CORE0_RAM) + LENGTH(_CORE0_RAM) - . - _SYS_STACK_SIZE - _UND_STACK_SIZE - _ABT_STACK_SIZE - _SVC_STACK_SIZE - _IRQ_STACK_SIZE - _FIQ_STACK_SIZE;  /* Calculate maximum heap size to move stack all the way to the end of RAM */
        
        . = ALIGN(4);
        _heap_end = .;     /* User defined symbol */
        __heap_limit = .;  /* Used by newlib */
    } > _CORE0_RAM : _LOAD_RW

    .stack (NOLOAD) : {
        . = ALIGN(8);
        
        _SYS_STACK_BASE = .;
        . += _SYS_STACK_SIZE;
        _SYS_STACK_LIMIT = .;
        
        _UND_STACK_BASE = .;
        . += _UND_STACK_SIZE;
        _UND_STACK_LIMIT = .;
        
        _ABT_STACK_BASE = .;
        . += _ABT_STACK_SIZE;
        _ABT_STACK_LIMIT = .;
        
        _SVC_STACK_BASE = .;
        . += _SVC_STACK_SIZE;
        _SVC_STACK_LIMIT = .;
        
        _IRQ_STACK_BASE = .;
        . += _IRQ_STACK_SIZE;
        _IRQ_STACK_LIMIT = .;
        
        _FIQ_STACK_BASE = .;
        . += _FIQ_STACK_SIZE;
        _FIQ_STACK_LIMIT = .;
        
        __stack = .;  /* Used by newlib */
        
        . = ALIGN(8);
    } > _CORE0_RAM : _LOAD_RW

    .ARM.attributes 0 : { KEEP(*(.ARM.attributes)) }
    /DISCARD/ : { *(.note.GNU-stack) }
}
