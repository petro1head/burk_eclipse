#include "regdef.h"
#include "exc_vectors.h"
#include "chip_asm.h"

	.set noreorder

/* --------------------------------------------------
# Terminal output string
# Not destroys registers except a0
# Input: a0=addrerss of text string with '\0' at the end
*/
	.globl	tos_asm
	.ent	tos_asm
	
tos_asm:
	addiu  sp,sp,-8
        sw     t0,4(sp)
        sw     ra,(sp)
        addu   t0,zero,a0
TOS1:   lbu    a0,(t0)      # a0=byte
        addiu  t0,t0,1
        beq    a0,zero,TOSEnd
	nop
        bal    tob_asm
	nop
        b      TOS1
	nop
TOSEnd: lw     ra,(sp)
        lw     t0,4(sp)
        jr     ra
        addiu  sp,sp,8
	.end	tos_asm

/* ----------------------------------------------------
# Terminal output byte
# Input: a0=byte
*/
	.globl	tob_asm
	.ent	tob_asm
	
tob_asm:
	addiu	sp,sp,-8
	sw	t0,(sp)
	sw	t1,4(sp)

#ifdef FOR_TESTER
	// For Tester pass through only '\05' '\08' bytes
	addiu	t0,zero,5
	beq	a0,t0,0f	// if 5 - goto real output
	addiu	t0,zero,8
	beq	a0,t0,0f	// if 8 - goto real output
	nop
	b	999f		// if not 5 && not 8 -- goto return
	nop
0:
#endif
	li	t0,UART0_BASE
1:
	lb	t1,LSR(t0)
	nop
	andi	t1,t1,0x20
	beq	t1,zero,1b
	nop
	sb	a0,TxD(t0)
999:
	lw	t0,(sp)
	lw	t1,4(sp)
	jr	ra
	addiu	sp,sp,8
	.end	tob_asm

/* ----------------------------------------------------
# Terminal read byte
# Output: v0=byte
*/
	.globl	trb
	.ent	trb
	
trb:
	addiu	sp,sp,-8
	sw	t0,(sp)
	sw	t1,4(sp)

	li	t0,UART0_BASE
1:
	lb	t1,LSR(t0)
	nop
	andi	t1,t1,0x01
	beq	t1,zero,1b
	nop
	lb	v0,TxD(t0)
999:
	lw	t0,(sp)
	lw	t1,4(sp)
	jr	ra
	addiu	sp,sp,8
	.end	trb

/* ----------------------------------------------------
# Terminal output byte empty (transmitter is empty)
# Output: v0=1 if empty, 0 - transmitting
*/
	.globl	tob_empty
	.ent	tob_empty
	
tob_empty:
	addiu	sp,sp,-8
	sw	t0,(sp)
	sw	t1,4(sp)

#ifdef FOR_TESTER
	b	999f		// if not 5 && not 8 -- goto return
	addiu	v0,zero,1
#endif
	li	t0,UART0_BASE
	addiu	v0,zero,0x60
	lb	t1,LSR(t0)	// status
	nop
	and	t1,t1,v0
	beq	t1,v0,999f	// if (status&0x60) == 0x60 --> empty, return(0x60)
	nop

	// if here - not empty, return(0)
	addiu	v0,zero,0


999:
	lw	t0,(sp)
	lw	t1,4(sp)
	jr	ra
	addiu	sp,sp,8
	.end	tob_empty
/* --------------------------------------------------
* input:  a0=number
*/
prdig:
        addiu   sp,sp,-12
        sw      a0,(sp)
        sw      a2,4(sp)
        sw      ra,8(sp)
        andi    a0,a0,0xf
        addiu   a2,a0,-10
	bltz	a2,digit1
	nop
	b	p11
        addiu   a0,a0,0x37      # 'A'-10
digit1:
	addiu   a0,a0,'0'
p11:
	bal	tob_asm
	nop
        lw      ra,8(sp)
        lw      a2,4(sp)
        lw      a0,(sp)
	jr	ra
        addiu   sp,sp,12
/* --------------------------------------------------
* input:  a0=number
*/
	.globl	ton_asm
	.ent	ton_asm
	
ton_asm:
        addiu   sp,sp,-12
        sw      a0,(sp)
        sw      a2,4(sp)
        sw      ra,8(sp)
	addu	a2,zero,a0
	bal	prdig
	srl	a0,a2,28
	bal	prdig
	srl	a0,a2,24
	bal	prdig
	srl	a0,a2,20
	bal	prdig
	srl	a0,a2,16
	bal	prdig
	srl	a0,a2,12
	bal	prdig
	srl	a0,a2,8
	bal	prdig
	srl	a0,a2,4
	bal	prdig
	addu	a0,zero,a2
	
        lw      ra,8(sp)
        lw      a2,4(sp)
        lw      a0,(sp)
	jr	ra
        addiu   sp,sp,12
	.end	ton_asm

/*--------------------------------------------------------------
	void tonf( a0=val, a1=number_of_hex_digits_to_output )
--------------------------------------------------------------*/
	.globl	tonf
	.ent	tonf
tonf:
	addiu   sp,sp,-20
	sw      ra,0(sp)
	sw      v0,4(sp)
	sw      a0,8(sp)
	sw      a1,12(sp)
	sw      a2,16(sp)

	addu	a2,zero,a0	// a2 = copy of val

	// if ( num > 8 ) num = 8
	addiu	a0,zero,8
	sltu	a0,a0,a1	// a0 = ( 8 < num ) ? 1 : 0
	beqz	a0,0f
	nop
	  addiu	a1,zero,8	
0:
	// if ( num == 0 ) num = 1
	bnez	a1,1f
	nop
	  addiu	a1,zero,1
1:
	
//.....	Loop
2:
	  // shift = (num-1)*4
	  addiu	a1,a1,-1	// num--
	  sll	v0,a1,2		// v0 = (num-1)*4
	
	  bal	prdig
	  srlv	a0,a2,v0
	
	bgtz	a1,2b		// if ( num > 0 ) goto loop
	nop
	
	
	lw      ra,0(sp)
	lw      v0,4(sp)
	lw      a0,8(sp)
	lw      a1,12(sp)
	lw      a2,16(sp)
	jr	ra
        addiu   sp,sp,20

	.end	tonf



/*---------------------------------------------*/
#if 0
	.globl ExcConn
	.ent   ExcConn
ExcConn:
        addiu	sp,sp,-4
	sw	ra,(sp)
	li	ra,VECT_BASE
	addu	ra,ra,a0
	lw	v0,(ra)
	sw	a1,(ra)
	lw	ra,(sp)
	addiu	sp,sp,4
	jr	ra
	nop
	.end   ExcConn
#endif
// ------------------------------------------------------------ //
//      void error_end(void);            	       		//
//           error_end                  			//
// ------------------------------------------------------------ //

	.globl error_end
	.ent error_end
	
error_end:
#ifndef AUTO_BIN
#ifndef FOR_PRIME
	addiu	sp, sp, -8
	sw	a0,4(sp)
	sw	ra,(sp)
	
	li	a0,0x5		// ^E
	bal	tob_asm
	nop
	
	lw	ra,(sp)
	lw	a0,4(sp)
	addiu	sp,sp,8
#endif
#endif
	jr	ra
	nop
	.end error_end

// ------------------------------------------------------------ //
//      void happy_end(void);            	       		//
//           happy_end                  			//
// ------------------------------------------------------------ //

	.globl happy_end
	.ent happy_end
	
happy_end:
#ifndef AUTO_BIN
#ifndef FOR_PRIME
	addiu	sp, sp, -8
	sw	a0,4(sp)
	sw	ra,(sp)
	
	li	a0,0x8		// ^H
	bal	tob_asm
	nop
	
	lw	ra,(sp)
	lw	a0,4(sp)
	addiu	sp,sp,8
#endif
#endif
	jr	ra
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		
	.end happy_end
