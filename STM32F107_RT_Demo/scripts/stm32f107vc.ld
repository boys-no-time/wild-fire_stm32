
/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x2000FFFF;    /* end of RAM */

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x500;      /* required amount of heap  */
_Min_Stack_Size = 0x500; /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 256K
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 64K
}

/* Define output sections */
SECTIONS
{
    /* The startup code goes first into FLASH */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector)) /* Startup code */
        . = ALIGN(4);
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
      . = ALIGN(4);
      *(.text)           /* .text sections (code) */
          *(.text*)          /* .text* sections (code) */
          *(.glue_7)         /* glue arm to thumb code */
          *(.glue_7t)        /* glue thumb to arm code */
          *(.eh_frame)
          *(.gnu.linkonce.t*)

          KEEP (*(.init))
          KEEP (*(.fini))

          /* section information for finsh shell */
          . = ALIGN(4);
          __fsymtab_start = .;
          KEEP(*(FSymTab))
          __fsymtab_end = .;
          . = ALIGN(4);
          __vsymtab_start = .;
          KEEP(*(VSymTab))
          __vsymtab_end = .;
          . = ALIGN(4);

          /* section information for initial. */
          . = ALIGN(4);
          __rt_init_start = .;
          KEEP(*(SORT(.rti_fn*)))
          __rt_init_end = .;
          . = ALIGN(4);

          _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
      . = ALIGN(4);
      *(.rodata)         /* .rodata sections (constants, strings, etc.) */
          *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
          . = ALIGN(4);
  } >FLASH

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
  .ARM : {
      __exidx_start = .;
      *(.ARM.exidx*)
      __exidx_end = .;
  } >FLASH

  .preinit_array     :
  {
      PROVIDE_HIDDEN (__preinit_array_start = .);
      KEEP (*(.preinit_array*))
      PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH
  .init_array :
  {
      PROVIDE_HIDDEN (__init_array_start = .);
      KEEP (*(SORT(.init_array.*)))
      KEEP (*(.init_array*))
      PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
  .fini_array :
  {
      PROVIDE_HIDDEN (__fini_array_start = .);
      KEEP (*(SORT(.fini_array.*)))
      KEEP (*(.fini_array*))
      PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
      . = ALIGN(4);
      _sdata = .;        /* create a global symbol at data start */
      *(.data)           /* .data sections */
          *(.data*)          /* .data* sections */

          . = ALIGN(4);
          _edata = .;        /* define a global symbol at data end */
  } >RAM AT> FLASH


  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
      /* This is used by the startup in order to initialize the .bss secion */
      _sbss = .;         /* define a global symbol at bss start */
      __bss_start__ = _sbss;
      *(.bss)
          *(.bss*)
          *(COMMON)

          . = ALIGN(4);
          _ebss = .;         /* define a global symbol at bss end */
          __bss_end__ = _ebss;
          __bss_end = _ebss;
  } >RAM

  ._user_heap_stack :
  {
      . = ALIGN(4);
      PROVIDE ( end = . );
      PROVIDE ( _end = . );
      . = . + _Min_Stack_Size ;
      . = . + _Min_Heap_Size;
      . = ALIGN(4);
      __user_end = .;
  } >RAM


  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
      libc.a ( * )
      libm.a ( * )
      libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }

    /* Stabs debugging sections.  */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    /* DWARF debug sections.
     * Symbols in the DWARF debugging sections are relative to the beginning
     * of the section so we begin them at 0.  */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
}
