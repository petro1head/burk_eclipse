
OUTPUT_FORMAT("elf32-bigmips", "elf32-bigmips", "elf32-littlemips")

OUTPUT_ARCH(mips)

/*STARTUP(startup.o) */

PROVIDE (softfloat_fpe_handler = 0);
PROVIDE (raise = 0);
PROVIDE (exit = 0);

SECTIONS
{
	.text ALIGN(0x8):
	{
		_ftext = . ;
		*(.text)
	} =0

	.rodata ALIGN(0x8):
	{
		*(.rodata)
		_etext = .;
	}

	.data ALIGN(0x8):
	{
		_fdata = . ;
		*(.data)
	}

	/*  _gp = ALIGN(16) + 0x7ff0; */

	/* We want the small data sections together, so single-instruction offsets
	can access them all, and initialized data all before uninitialized, so
	we can shorten the on-disk segment size.  */

	.sdata ALIGN(0x8):
	{
	_gp = . ;
	*(.sdata)
	}
	_edata  =  .;


	__bss_start = .;
	_fbss = .;
	/*
	.sbss ALIGN(0x8):
	{
	*(.sbss)
	*(.scommon)
	}
	*/
	.bss ALIGN(0x8):
	{
	*(.bss)
	*(COMMON)
	}

	_end = . ;
	_ebss = .;

	.reginfo ALIGN(0x8):
	{
	*(.reginfo)
	}

	.eh_frame ALIGN(0x8):
	{
	*(.eh_frame)
	}

	_LengthOfText = (_etext - _ftext + 7) & 0xfffffff8;
	_LengthOfData = (_edata - _fdata + 7) & 0xfffffff8;
	_LengthOfBSS = (_ebss - _fbss + 7) & 0xfffffff8;

}
