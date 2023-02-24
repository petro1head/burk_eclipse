#include <regdef.h>
        
/* ---------------------------------------------------- */        
	.set noreorder
	.globl  flushcache
	.globl  flush_data_cache
	.ent flushcache
flush_data_cache:
flushcache:
	addiu   sp,sp,-4*3
	sw      ra,(sp)
	sw      t0,4(sp)
	sw      t1,8(sp)

fc_begin:
	bal     to_kseg1
	lui     t0,0xe000
to_kseg1:
	and     ra,ra,t0		/* ra = space bits */
	la      t1,flushcache_itself
	li      t0,0x1fffffff
	and     t1,t1,t0
	lui     t0,0xa000
	or      t1,t1,t0		/* t1 = kseg1 address */
	jr      t1
	nop

flushcache_itself:
	mfc0	t0, C0_SR
	lui	t1, 0x3
	or	t1,t0,t1		
/* set clear-bits of Instruction and Data Cache in STATUS reg */
	mtc0	t1, C0_SR
	nop
	mtc0	t0, C0_SR		# reset clear-bits
	nop

	lw      ra,(sp)
	lw      t0,4(sp)
	lw      t1,8(sp)
	addiu   sp,sp,4*3
	jr      ra
	nop

	.set reorder
	.end flushcache

