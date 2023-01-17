/*
* startup.s - the startup code
*/

#include <regdef.h>

	.text	
	.set noreorder
ENTRY:
	li	sp,PROGRAM_STACK
	addiu	sp,sp,-4
	sw	ra,(sp)

	bal	BEGIN
	nop
FTEXT:
	.word	_ftext		/* .text section target start address */
ETEXT:                         	
	.word	_etext		/* .text section target end address */
LENGTH_OF_TEXT:
	.word	_LengthOfText	/* length of .text section */
FDATA:                          	
	.word	_fdata		/* .data section target start address */
EDATA:
	.word	_edata		/* .data section target end address */
LENGTH_OF_DATA:
	.word	_LengthOfData	/* length of .data section */
FBSS:
	.word	_fbss		/* .bss & .sbss sections target start address */
EBSS:
	.word	_ebss		/* .bss & .sbss sections target end address */
LENGTH_OF_BSS:
	.word	_LengthOfBSS	/* length of .bss section */
SDATA:
	.word	_gp		/* .sdata section target begin address */

BEGIN:
	la	t0,ENTRY
	la	t1,FTEXT
	subu	t1,t1,t0
	subu	ra,ra,t1	/* ra - current base address */

	mfc0	t0,C0_SR
	lui	t1,0x6000
	or	t0,t0,t1
	mtc0	t0,C0_SR
	nop

	li	t4,PROGRAM_ADDR
	beq	ra,t4,EndMoveMemory	/* it's true then memories don't copy */
	nop

	/* move .text section */
	/* get a destination address */
	la	t3,FTEXT
	subu	t3,t3,t4
	addu	t3,t3,ra
	lw	t0,(t3)
	nop
	/* get a source address */
	subu	t3,t0,t4
	addu	t1,t3,ra
	/* get a length */
	la	t3,LENGTH_OF_TEXT
	subu	t3,t3,t4
	addu	t3,t3,ra
	lw	t2,(t3)
	nop
	beq	zero,t2,EndMoveText
	nop
MoveText:
	addiu	t2,t2,-4
	lw	t3,(t1)
	addiu	t1,t1,4
	sw	t3,(t0)
	bgtz	t2,MoveText
	addiu	t0,t0,4
EndMoveText:

	/* move .data section */
	/* get a destination address */
	la	t3,FDATA
	subu	t3,t3,t4
	addu	t3,t3,ra
	lw	t0,(t3)
	nop
	/* get a source address */
	subu	t3,t0,t4
	addu	t1,t3,ra
	/* get a length */
	la	t3,LENGTH_OF_DATA
	subu	t3,t3,t4
	addu	t3,t3,ra
	lw	t2,(t3)
	nop
	beq	zero,t2,EndMoveData
	nop
MoveData:
	addiu	t2,t2,-4
	lw	t3,(t1)
	addiu	t1,t1,4
	sw	t3,(t0)
	bgtz	t2,MoveData
	addiu	t0,t0,4
EndMoveData:

EndMoveMemory:
	/* clear .bss section */
	/* get a destination address */
	la	t3,FBSS
	subu	t3,t3,t4
	addu	t3,t3,ra
	lw	t0,(t3)
	/* get a length */
	la	t3,LENGTH_OF_BSS
	subu	t3,t3,t4
	addu	t3,t3,ra
	lw	t1,(t3)
	nop
	beq	zero,t1,EndClearBSS
	nop
ClearBSS:
	addiu	t1,t1,-4
	sw	zero,(t0)
	bgtz	t1,ClearBSS
	addiu	t0,t0,4
EndClearBSS:

	la	t3,SDATA
	subu	t3,t3,t4
	addu	t3,t3,ra
	lw	gp,(t3)

	ctc1	zero,$31
	nop

	addiu	sp,sp,-256

	la	t0,main  /* jump to moved copy in RAM */
	jalr	t0
	nop

	addiu	sp,sp,256
	lw	ra,(sp)
	nop

	jr	ra		/* jump exit */
	addiu	sp,sp,4
