OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

ENTRY(Reset_Handler)

/* Location and size of core 1 program */
__CORE1_RAM_BASE    = 0x0;
__CORE1_RAM_SIZE    = 64M;

/* For debugging support */
__CORE1_WAITER_SIZE = 512;

/* Ensure these stack sizes are aligned to 8 bytes */
__FIQ_STACK_SIZE = 4096;
__IRQ_STACK_SIZE = 4096;
__SVC_STACK_SIZE = 4096;
__ABT_STACK_SIZE = 4096;
__UND_STACK_SIZE = 4096;
__SYS_STACK_SIZE = 8192;  /* This is also for the user mode, because they use the same stack pointer */

MEMORY {
    __CORE1_RAM (rwx) : ORIGIN = __CORE1_RAM_BASE, LENGTH = __CORE1_RAM_SIZE
}

/* A solution to the linker warning of first load segment having rwx is to manually create the program headers with the correct segment flags */
/* Without this, the linker will create default LOAD segment having flags specified by the MEMORY, i.e. rwx flags above */
/* FLAGS bits: bit2 = read    (r)           */
/*             bit1 = write   (w)           */
/*             bit0 = execute (x)           */
/* Examples: 4 = r, 5 = rx, 6 = rw, 7 = rwx */
PHDRS {
    __LOAD_RX PT_LOAD FLAGS(5);
    __LOAD_RW PT_LOAD FLAGS(6);
}

SECTIONS {
    .vectors : {
        Image$$VECTORS$$Base = .;   /* Used by CMSIS */
        
        *(RESET)                    /* Used by CMSIS and my startup for vector table */
        *(.vectors)                 /* Used by HWLib */
        
        Image$$VECTORS$$Limit = .;  /* Used by CMSIS */
    } > __CORE1_RAM : __LOAD_RX

    .core1_waiter : {
        . = ALIGN(4);
    
        . = . + __CORE1_WAITER_SIZE;
        
        . = ALIGN(4);
    } > __CORE1_RAM : __LOAD_RX

    .text : {
        . = ALIGN(4);
        __text = .;
        
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
        __etext = .;
    } > __CORE1_RAM : __LOAD_RX

    .rodata : {
        . = ALIGN(4);
        *(.rodata)     /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)    /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(4);
    } > __CORE1_RAM : __LOAD_RX

		/* MMU L1 translation table block */
    .mmu_ttb_l1 : {
				. = ALIGN(16384);
        __mmu_ttb_l1_entries_start = .;
				*(mmu_ttb_l1_entries)
        __mmu_ttb_l1_entries_end = .;
    } > __CORE1_RAM : __LOAD_RX

		/* MMU L2 translation table block */
    .mmu_ttb_l2 : {
				. = ALIGN(16384);
				__mmu_ttb_l2_entries_start = .;
        *(mmu_ttb_l2_entries)
        __mmu_ttb_l2_entries_end = .;
    } > __CORE1_RAM : __LOAD_RX

    .ARM.extab : {
        . = ALIGN(4);
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        . = ALIGN(4);
    } > __CORE1_RAM : __LOAD_RX

    .ARM.exidx : {
        . = ALIGN(4);
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
        . = ALIGN(4);
    } > __CORE1_RAM : __LOAD_RX

    /* C++ runtime: static constructors */
    .ctors : {
        . = ALIGN(4);
        KEEP(*crtbegin.o(.ctors))
        KEEP(*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP(*(SORT(.ctors.*)))
        KEEP(*crtend.o(.ctors))
        . = ALIGN(4);
    } > __CORE1_RAM : __LOAD_RX

    /* C++ runtime: static destructors and atexit() */
    .dtors : {
         . = ALIGN(4);
        KEEP(*crtbegin.o(.dtors))
        KEEP(*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP(*(SORT(.dtors.*)))
        KEEP(*crtend.o(.dtors))
        . = ALIGN(4);
    } > __CORE1_RAM : __LOAD_RX

    .preinit_array : {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(4);
    } > __CORE1_RAM : __LOAD_RX

    .init_array : {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(4);
    } > __CORE1_RAM : __LOAD_RX

    .fini_array : {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(4);
    } > __CORE1_RAM : __LOAD_RX

    .eh_frame_hdr : {
        . = ALIGN(4);
        KEEP(*(.eh_frame_hdr))
        *(.eh_frame_entry)
        *(.eh_frame_entry.*)
        . = ALIGN(4);
    } > __CORE1_RAM : __LOAD_RX

    .eh_frame : {
        . = ALIGN(4);
        KEEP(*(.eh_frame))
        *(.eh_frame.*)
        . = ALIGN(4);
    } > __CORE1_RAM : __LOAD_RX

    .data : {
        . = ALIGN(4);
        __data_start = .;  /* User defined symbol */
        
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d.*)
        
        . = ALIGN(4);
        __data_end = .;  /* User defined symbol */
    } > __CORE1_RAM : __LOAD_RW

		.dma_buffer (NOLOAD) : {
			. = ALIGN(1048576);
			__dma_buffer_start = .;
			
			*(.dma_buffer)
			
			. = ALIGN(1048576);
			__dma_buffer_end = .;
		} > __CORE1_RAM : __LOAD_RW

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
    } > __CORE1_RAM : __LOAD_RW

    .heap (NOLOAD) : {
        . = ALIGN(4);
        __heap_start = .;  /* User defined symbol */
        
        *(.heap*)
        . = ORIGIN(__CORE1_RAM) + LENGTH(__CORE1_RAM) - . - __FIQ_STACK_SIZE - __IRQ_STACK_SIZE - __SVC_STACK_SIZE - __ABT_STACK_SIZE - __UND_STACK_SIZE - __SYS_STACK_SIZE;  /* Calculate maximum heap size to move stack all the way to the end of RAM */
        
        . = ALIGN(4);
        __heap_end = .;    /* User defined symbol */
        __heap_limit = .;  /* Used by newlib */
    } > __CORE1_RAM : __LOAD_RW

    .stack (NOLOAD) : {
        . = ALIGN(8);
        
        __FIQ_STACK_BASE = .;
        . += __FIQ_STACK_SIZE;
        __FIQ_STACK_LIMIT = .;
		
		__IRQ_STACK_BASE = .;
        . += __IRQ_STACK_SIZE;
        __IRQ_STACK_LIMIT = .;
		
		__SVC_STACK_BASE = .;
        . += __SVC_STACK_SIZE;
        __SVC_STACK_LIMIT = .;
		
		__ABT_STACK_BASE = .;
        . += __ABT_STACK_SIZE;
        __ABT_STACK_LIMIT = .;
		
		__UND_STACK_BASE = .;
        . += __UND_STACK_SIZE;
        __UND_STACK_LIMIT = .;
        
		__SYS_STACK_BASE = .;
        . += __SYS_STACK_SIZE;
        __SYS_STACK_LIMIT = .;
        
        __stack = .;  /* Used by newlib */
        
        . = ALIGN(8);
    } > __CORE1_RAM : __LOAD_RW

    .ARM.attributes 0 : { KEEP(*(.ARM.attributes)) }
    /DISCARD/ : { *(.note.GNU-stack) }
}
