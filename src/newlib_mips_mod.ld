ENTRY(_start)
/*STARTUP(startup.o)*/
/* Use default output format */
/* OUTPUT_FORMAT("elf32-bigmips") */
__DYNAMIC  =  0;

PROVIDE (__stack = 0);

/*
 * Initalize some symbols to be zero so we can reference them in
 * startup, whether they are available or not.
 */
PROVIDE (softfloat_fpe_handler = 0);
PROVIDE (raise = 0);
PROVIDE (exit = 0);

SECTIONS
{	
  /*
   * We have no ELF loader. Use one section for everything that we need at
   * runtime to simplify binary image creation.
   */
  .text : {

    /* .text "section" */
    _ftext = . ;
    PROVIDE (eprol = .);
    *(.text)
    *(.text.*)
    *(.gnu.linkonce.t.*)
    *(.mips16.fn.*)
    *(.mips16.call.*)
    *(.reginfo*)
    /* .init "section" */
    KEEP (*(.init))

    /* .fini "section" */
    KEEP (*(.fini))

    /* .rel.sdata "section" */
    PROVIDE (__runtime_reloc_start = .);
    *(.rel.sdata)
    PROVIDE (__runtime_reloc_stop = .);

    PROVIDE (etext = .);
    _etext  =  .;
  
    /* .eh_frame_hdr "section" */
    *(.eh_frame_hdr)

    /* .eh_frame "section" */
    KEEP (*(.eh_frame))

    /* .gcc_except_table "section" */
    *(.gcc_except_table)

    . = .;
    /* .rodata "section" */
    *(.rdata)
    *(.rodata)    *(.rodata.*)

   _fdata = ALIGN(16);
    /* .data section */
    *(.data)
    *(.data.*)
    *(.k128.text)
    *(.gnu.linkonce.d.*)

    . = ALIGN(8);
    _gp = . + 0x8000;
    __global = _gp;

    . = ALIGN(4);
    PROVIDE (edata  =  .);
    _edata  =  .;

  }

  . = ALIGN(32);
  PROVIDE (end = .);
  _end = .;
}
