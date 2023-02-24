	.nolist
	.text
	.set	noat
	.set	noreorder	// DON'T TOUCH !!!
/*
 *
 *	Initialization and Interrupt handling for 1907VM014
 *
 *----------------------------------------------------------------------------
 *
 */
#include "regdef.h"
#include "kernel.h"
#include "chip_asm.h"
#include "exc_vectors.h"


#define UART_DIVISOR  (24000000/16/57600)
//#define UART_DIVISOR  (24000000/16/115200)
// #define UART_DIVISOR  1

/*
# RAM using
#  vectors     return stack       Variables		Exc_vectors
# !---------!--------------------!--------------------!--------------------!
#0xa0000000 0xa00000b0    0xa0000200                0xa0000600
#
*/
/* ------------------------------------ *
 * Stack  (~0xb0-0x200)			* 
 *------------------------------------- *
 Variables and Save area - see kernel.h */
 
/*-----------------------------------------------------*/

/* Registers:							*/
/*               s3 - RegBaseAdr				*/
/*               k0 - register file base region - 0xa0000200	*/
/*               sp - stack pointer 				*/

	.org	0
Entry_point:
	B	EXCRESET
	//B	Entry_point
	nop

	.org    0x100
ROM_UTLB_refill:
	B	EXC100
	nop

	.org    0x180  	
ROM_exc:
	B	EXC180
	nop

/*--------------------------------------------------------------*/
EXCRESET:
	lui    t0,0x3053 // Cp1 on, Cp0 on, BEV->bootstrap, reset caches
	mtc0   t0,C0_SR
	lui    t0,0x3050 // Cp1 on, Cp0 on, BEV->bootstrap, no reset caches
	mtc0   t0,C0_SR
//	B	Entry_point
//	nop

	// Goto cached space
	/*
	la	a0,0f
	lui	v0,0x2000
	or	a0,a0,v0	// set bit 29
	xor	a0,a0,v0	// clear bit 29 (0xBFC...-->0x9FC...)
	jr	a0
	nop
	*/
0:
	// init memory controller
	bal	MemCtrl_Init
	nop

	li	t2,0x2c800007 
        ori	t2,t2,0x100	# IgnorPar, bit 8
	mtc0	t2,C0_CONFIG
	

	ctc1   zero,C1_SR
	mthi   zero
	mtlo   zero

Common_config:	
	mtc0	zero,C0_TLBHI

	ctc1	zero,$31			

	li     sp,KERNEL_STACK		# set sp

	
REG_INIT:
 
/* ------------------------------------------------- */   
Lclear_reentry_flag:
	li	k0,DbgREGS		// k0=regfile base address
	sw	zero,SREENTRY(k0)	// reentry flag

/* ------------------------------------------------- */
/*       Interrupt controller initialization         */
/* ------------------------------------------------- */   
init_interrupt_controller:
	bal	intc_init
	nop
/* ------------------------------------------------- */
/*                  RS-232 initialization            */
/* ------------------------------------------------- */   
UARTs_init:
/* port 0 */
        li	a0,UART0_BASE
	bal	init_rs
        addiu   a1,zero,UART_DIVISOR
			     
/* port 1 */
        li	a0,UART1_BASE
	bal	init_rs
        addiu   a1,zero,UART_DIVISOR


/* Set some Cp0 registers*/
	mtc0    zero,C0_CONTEXT  # clear PTEbase
	mtc0    zero,C0_CAUSE    # clear Sw Intrpts

/* Set some flags */
	li	k0,SREGS
	sw	zero, EXC_COUNT(k0)

	bal	Init_EVT	// Init Exception vector table
	nop

/* ------------------------------------------------- */
/*                  GPIO initialization              */
/* ------------------------------------------------- */   
/*GPIO_init:
        li	a0,GPIO_INTFLAG
        sw	zero,(a0)
        li	a1,0xffffffff	/* Enable Alt Func */
/*        li	a0,GPIO_AFIO
        sw	a1,(a0)
        li	a0,GPIO_AFMODE
        sw	zero,(a0)	/* Alt Func 0 */

	b	start_PROGRAM
	nop

//##############################################################
	.org    0x480
Debug_exc:
	mtc0	k0,C0_DESAVE
	
	// Check Reentry to Debug Exception
	li	k0,DbgREGS	# k0=regfile base address
	lw	k0,SREENTRY(k0)	// reentry flag
	beqz	k0,1f
	nop
	  // Re-entry to exception handler - save only DEBUG
	  li	k0,DbgREGS	# k0=regfile base address
	  mfc0	v1,C0_DEBUG
	  b	100f
	  sw	v1,SDEBUG(k0)	# DEBUG

1:
	// Save some registers
	li	k0,DbgREGS	# k0=regfile base address
	sw	k0,SREENTRY(k0)	// reentry flag = not NULL!!!

	sw	ra,SR31(k0)	// because "jalr" will be used
	sw	sp,SR29(k0)	// because "stack" will be used

	sw	r1,SR1(k0)	# at
	sw	r2,SR2(k0)	# v0
	mfc0	v0,C0_DEPC
	sw	r3,SR3(k0)	# v1
	mfc0	v1,C0_DEBUG
	sw	r4,SR4(k0)	# a0
	sw	r5,SR5(k0)	# a1
	sw	r6,SR6(k0)	# a2
	sw	r7,SR7(k0)	# a3
	sw	r8,SR8(k0)	# t0
	sw	r9,SR9(k0)	# t1
	sw	v0,SDEPC(k0)	# DEPC
	sw	v1,SDEBUG(k0)	# DEBUG

	li	sp,DBG_EXCEPTION_STACK # set sp

100:
	// Check user handler
	li	t0,VECT_BASE
	lw	t0,DBG_Exc_v(t0)
	nop

	beqz	t0,999f
	nop

	jalr	t0	/* call user's handler */
	nop


999:
	// Save some registers
	li	k0,DbgREGS	# k0=regfile base address
	lw	ra,SR31(k0)    
	lw	sp,SR29(k0)    
	// set DEPC
	lw	v0,SDEPC(k0)	# read DEPC (may be changed)
	mtc0	v0,C0_DEPC

	lw	r1,SR1(k0)	# at
	lw	r2,SR2(k0)	# v0
	lw	r3,SR3(k0)	# v1
	lw	r4,SR4(k0)	# a0
	lw	r5,SR5(k0)	# a1
	lw	r6,SR6(k0)	# a2
	lw	r7,SR7(k0)	# a3
	lw	r8,SR8(k0)	# t0
	lw	r9,SR9(k0)	# t1
	sw	zero,SREENTRY(k0)	// reentry_flah = 0

	.set	arch=mips32
	deret
	.set	arch=default

//..............................................................




//..............................................................
//..............................................................
start_PROGRAM:
    b       WRESTART
	nop
	li	sp,PROGRAM_STACK
	//li	t0,PROGRAM_ADDR
	//jalr	t0
	jal	main
	nop


//la a0,T_SYS
//jal tos_asm
//nop
//la	v0, _edata
//jalr	v0
//nop


Return_from_program:
// v0 = 0 - OK, v0 != 0 - ERROR
	
Sleep:  b	Sleep
	nop


/* -------------------------------------------------
# Warm restart
*/
WRESTART:
	b	WRESTART
	nop

/* ----------------------------------------------------------- */
/* handle exception UTLB miss */
EXC100:
	li      k0,SREGS	# k0=regfile base address
	sw      ra,SR31(k0)    
	sw      k1,SR27(k0)
	sw      zero,EXCTYPE(k0)  # save exception type
	b	EXCEP
	nop

EXC180:
	li      k0,SREGS	# k0=regfile base address
	sw      ra,SR31(k0)
	addiu   ra,zero,1       
	sw      k1,SR27(k0)
	sw      ra,EXCTYPE(k0)  # save exception type.
//	b	EXCEP
//	nop
	// Goto "EXCEP" in cached space
	la	k1,EXCEP
	lui	ra,0x2000
	or	k1,k1,ra	// set bit 29
	xor	k1,k1,ra	// clear bit 29 (0xBFC...-->0x9FC...)
	jr	k1
	nop
EXCEP:
	bal     SAVE_1_30       # save r1-r30
	nop		        # and set sp

	bal 	save_cp0
	nop

	lw	t0, EXC_COUNT(k0)
	nop
	addiu	t0, t0, 1
	sw	t0, EXC_COUNT(k0)
	
	lw	t0,SCAUSE(k0)
	la	t2,table	# table address
        andi    t0,t0,0x3c	# offset in table
	addu	t0,t0,t2	# address of handler address
	lw	t0,(t0)		# handler address
	li	t2, ~0x20000000
	and	t0, t0, t2	# cached address	
	jr	t0
	nop
table:
	.word	Int      # 0
	.word	TLBmod   # 1
	.word	TLBL     # 2
	.word	TLBS     # 3
	.word	ADEL     # 4
	.word	ADES     # 5
	.word	IBE      # 6
	.word	DBE      # 7
	.word	Sys      # 8
	.word	Bp       # 9
	.word	RI       # 10
	.word	CpU      # 11
	.word	Ov       # 12
	.word	IWatch   # 13
	.word	DWatch   # 14
	.word	FPE      # 15

/*----------------------------------------------
* a0 - handler offset in exception vector table 
*/
user_handler:
	li	t0,VECT_BASE
	addu	t0,t0,a0
	lw	t0,(t0)
	nop

	beq     t0,zero,ret_to_main_handler
	nop

	jalr	t0	/* call user's handler */
	nop

/* point of returning from user's handler */
FROM_USER_HANLER:

	bal     LOAD_1_30
	nop

	// Restore SR (may be KU bits are changed)
	lw	ra,SSR(k0)
	nop
	mtc0	ra,C0_SR

	lw      ra,SR31(k0)
	lw      k0,SEPC(k0)
	nop

 	jr      k0
	rfe

ret_to_main_handler:
	jr	ra
	nop


/*------------------------------------------------------------
# At entry of handler all registers are saved
# EXCTYPE contains exception type
# --------------------------------------------------
#                Exception handlers
# --------------------------------------------------
#   Mod   TLB Modified
# Source:  try to write to not dirty page
*/

TLBmod:
	bal	user_handler
	addiu	a0,zero,TLBmod_v
        la      a0,T_MOD	# v0=text "TLB Modified"
	bal     ExcMess
	nop
        b       WRESTART
	nop
/* --------------------------------------------------
#   TLBL   TLB Load: load or instruction fetch
#   TLBS   TLB Store: store
# Source:  TLB miss (EXCTYPE=0)
#          TLB invalid (bit V=0) (EXCTYPE=1)
*/

TLBL:
	bal	user_handler
	addiu	a0,zero,TLBL_v
	b	TLB_U
	nop
TLBS:
	bal	user_handler
	addiu	a0,zero,TLBS_v

TLB_U:
        lw      t0,EXCTYPE(k0)
        beq     t0,zero,e_tlbs2		#  UTLB Refill
	nop
        lw      t1,SBADVADDR(k0)	# v1=VPN
        li      t0,0xfffff000
        and     t1,t1,t0
        mtc0    t1,C0_TLBHI
	nop
        tlbp
        mfc0    t1,C0_INDEX	# v1=index
	nop
        bltz    t1,e_tlbs2	# TLB Refill detected
	nop

        la      a0,T_TLBI	# v0=addr. of text "TLB Invalid"
        bal     ExcMess      
	nop
        b       WRESTART	# 'Teplyj' restart
	nop
e_tlbs2:
        la      a0,T_TLBR    # v0=addr. of text "TLB Refill"
        bal     ExcMess      
	nop
        b       WRESTART     # 'Teplyj' restart
	nop
/* -------------------------------------------------------- 
#  AdEL   Address error: load or instruction fetch
#  AdES   Address error: store
# Source: load or instruction fetch on unaligned address
#            store to unaligned address
#            Access to kernel space in user mode
*/

ADEL:
	bal	user_handler
	addiu	a0,zero,ADEL_v
	B	ADE_U
	nop
ADES:
	bal	user_handler
	addiu	a0,zero,ADES_v
ADE_U:
        lw      t0,SSR(k0)
        lw      t1,SBADVADDR(k0)
        srl     t0,t0,3
        srl     t1,t1,31
        and     t0,t1,t0
        andi    t0,t0,1  /* t0=0 Kern */
                         /* t0=1 User&(vaddr>0xa0..) */
        beq     t0,zero,UnUlLF 	# in kernel mode unaligned load or instruction fetch
	nop
/* Access to kernel space in user mode */
	mfc0	t0,C0_SR
	li	t1,0xfffffff7
	and	t1,t1,t0
	mtc0	t1,C0_SR	# set KUp to kernel mode
        la      a0,T_U_K  	# a0=addr. of text "User to Kern. ref"
        bal     ExcMess      
	nop
        b       WRESTART 
	nop
/* in kernel mode unaligned load or instruction fetch */
UnUlLF:
        la      a0,T_ULF  	# a0=addr. of text "Unaligned address"
        bal     ExcMess      
	nop
        b       WRESTART
	nop
/* --------------------------------------------------
#   IBE    Bus Error: instruction fetch
# Source: hardware error on instruction fetch
*/
IBE:
	bal	user_handler
	addiu	a0,zero,IBE_v
        la	a0,T_IBE  	# a0=addr. of text "Bus Err.Ins.fetch"
        bal     ExcMess      
	nop
        b       WRESTART
	nop
/* --------------------------------------------------
#   DBE    Bus Error: data access
# Source: hardware error on load (/store data - not in bt83)
*/
DBE:
	bal	user_handler
	addiu	a0,zero,DBE_v

        la      a0,T_DBE     	# a0=addr. of text "Bus Err.Data ref."
        bal     ExcMess      
	nop
        b       WRESTART 
	nop
/* --------------------------------------------------
#   Ov    Integer Overflow
# Sources: ADD, ADDI, SUB
*/
Ov:
	bal	user_handler
	addiu	a0,zero,Ov_v
        la      a0,T_OV      	# a0=addr. of text "Overflow"
        bal     ExcMess
	nop
        b       WRESTART 
	nop
/* --------------------------------------------------
#   IWatch    Instruction watch
*/
IWatch:
	bal	user_handler
	addiu	a0,zero,IWATCH_v
        la      a0,T_IWATCH    	# a0=addr. of text "IWatch"
        bal     ExcMess
	nop
        b       WRESTART
	nop
/* --------------------------------------------------
#   DWatch    Data watch
*/
DWatch:
	bal	user_handler
	addiu	a0,zero,DWATCH_v
        la      a0,T_DWATCH    	# a0=addr. of text "DWatch"
        bal     ExcMess
	nop
        b       WRESTART
	nop
/* --------------------------------------------------
#   Sys   System Call
# Source: SYSCALL
*/
Sys:
	bal	user_handler
	addiu	a0,zero,Sys_v
        la      a0,T_SYS  	# a0=addr. of text "System Call"
        bal     ExcMess      
	nop
        b       WRESTART 
	nop
/* --------------------------------------------------
#   Bp    Break Point
# Source: BREAK
*/
Bp:
	bal	user_handler
	addiu	a0,zero,Bp_v

        la      a0,T_BP 	# a0=addr. of text "Breakpoint"
        bal     ExcMess      
	nop
        b       WRESTART
	nop
/* --------------------------------------------------
#   RI    Reserved Instruction
# Source: reserved instruction
*/
RI:
	bal	user_handler
	addiu	a0,zero,RI_v
        la      a0,T_RI   	# a0=addr. of text "Res. Instr"
        bal     ExcMess      
	nop
        b       WRESTART 
	nop
/* --------------------------------------------------
#   CpU   Coprocessor Unusable
# Source: CP1-CP3 unusable
#         CP0 access in user mode and CP0 marked as unusable
*/
CpU:
	bal	user_handler
	addiu	a0,zero,CpU_v
        la      a0,T_CPU  	# a0=addr. of text "Coproc. Unusable"
        bal     ExcMess      
	nop
        b       WRESTART 
	nop
/* -------------------------------------------------- */
/* Unknown exception */
Reserved_exc:
	bal	user_handler
	addiu	a0,zero,Reserved_exc_v

        la      a0,T_UNKN	 # a0=addr. of text "UnKnown Exception"
        bal     ExcMess      
	nop
        b       WRESTART 
	nop

/* --------------------------------------------------
#   FPU exception
# Source: FPU
*/
FPE:
	bal	user_handler
	addiu	a0,zero,FPE_v
	// Enable FPU (to read FPU control register without Coprocessor Unusable Exc.) ?
	// But how can be FPE if FPU is unusable?
        mfc0    t0,C0_SR
        lui	t1,0x6000    	# Cp1 on
        or	t1,t1,t0
        mtc0	t1,C0_SR	// temporarily 
        cfc1	t2,C1_SR
        sw	t2,SFC31(k0)
        li	t1,0xfffc0003
        and	t2,t2,t1      	# clear bits 17:2 C1_SR
        ctc1	t2,C1_SR
        mtc0	t0,C0_SR	// restore SR
        la      a0,T_FPE	# a0=addr. of text
        bal     ExcMess      
	nop
        b       WRESTART
	nop
/* --------------------------------------------------
#   Int   Interrupt: external interrupt
# Source: external devices
*/
Int:
	bal	user_handler
	addiu	a0,zero,Int_v

        lw      t0,SCAUSE(k0)
        lw      t1,SSR(k0)
	nop

/* Here on Unknown external interrupt */
          la      a0,T_UINT	# a0=addr. of text
          bal     ExcMess      
	  nop
          b       WRESTART 
	  nop


/* --------------------------------------------------
                    Subroutines
   -------------------------------------------------- */

/*--------------------------------------------------------
init RS port
input:
	a0 - RS port base address
	a1 - divisor
	uses t0
*/
init_rs:
        addiu   t0,zero,0x80
        sb      t0,LCR(a0)	// set bit "drab" of LCR
        sb      a1,DivLSB(a0)
	srl	a1,a1,8
        sb      a1,DivMSB(a0)
        ori     t0,zero,0x7	// 8bit + 2stop + no_parity
        sb      t0,LCR(a0)
        sb      zero,IER(a0)	// IER - disable ints
        sb      zero,MCR(a0)	// MCR
        lbu     zero,RxD(a0)	// RxD
        lbu     zero,LSR(a0)	// LSR
	jr	ra
        lbu     zero,MSR(a0)	// MSR




/*--------------------------------------------------------------
	void intc_init( void ) - disable all interrupts
--------------------------------------------------------------*/

#define	oINTENABLE	0x90
#define	oINTREQ		0x80
#define	oINTMAP		0x00

intc_init:
	addiu	sp,sp,-16
	sw	ra,0x0(sp)
	sw	a0,0x4(sp)
	sw	v0,0x8(sp)
	sw	v1,0xC(sp)

	li	a0,INTC_BASE
	sw	zero,oINTENABLE(a0) // disable all
	
	// write IntMap[0..31] with 0xF (ignore interrupt)
	li	v0,32/4		// cnt = ...
	li	v1,0xF		// fill_val = 0xF
	addiu	a0,a0,oINTMAP	// a0 = &IntMap[0]
intc_init1:
        sw	v1,0x0(a0)	// ptr[0] = fill_val
        sw	v1,0x4(a0)	// ptr[1] = fill_val
        sw	v1,0x8(a0)	// ptr[2] = fill_val
        sw	v1,0xC(a0)	// ptr[3] = fill_val
	addiu	v0,v0,-1	// cnt--
	bgtz	v0,intc_init1
	addiu	a0,a0,16	// ptr+=4
	
	lw	ra,0x0(sp)
	lw	a0,0x4(sp)
	lw	v0,0x8(sp)
	lw	v1,0xC(sp)
	jr	ra
        addiu  sp,sp,16

/*--------------------------------------------------------------
	Init Exception vector table
As cache is write through, we will use cached destination
--------------------------------------------------------------*/
	.align	4
Jump_2_EXC100:
	la	k0,EXC100
	jr	k0
	nop

Jump_2_EXC180:
	la	k0,EXC180
	jr	k0
	nop



Init_EVT:
	li	t0,VECT_BASE
	lui	s0,0x2000
	or	t0,t0,s0	// make cached 
	xor	t0,t0,s0	// from uncached
	ori	v1,zero,NVECT
0:
	sw	zero,0x0(t0)
	sw	zero,0x4(t0)
	sw	zero,0x8(t0)
	sw	zero,0xC(t0)
	addiu	v1,v1,-16
	bgtz	v1,0b
	addiu	t0,t0,16

/* Init RAM vectors 0x80000000 and 0x80000080 */
	la	a0,Jump_2_EXC100 // from1 = ...
	or	a0,a0,s0	// make cached 
	xor	a0,a0,s0	// from uncached
	la	a1,Jump_2_EXC100 // from2 = ...
	or	a1,a1,s0	// make cached 
	xor	a1,a1,s0	// from uncached
	lui	a2,0x8000	// to = ...

	lw	t0,0x0(a0)
	lw	t1,0x4(a0)
	lw	t2,0x8(a0)
	lw	t3,0xC(a0)
	sw	t0,0x0(a2)
	sw	t1,0x4(a2)
	sw	t2,0x8(a2)
	sw	t3,0xC(a2)

	lw	t0,0x0(a1)
	lw	t1,0x4(a1)
	lw	t2,0x8(a1)
	lw	t3,0xC(a1)
	sw	t0,0x80(a2)
	sw	t1,0x84(a2)
	sw	t2,0x88(a2)
	sw	t3,0x8C(a2)

	jr	ra
	nop


/*
# --------------------------------------------------
# Save r1-r30
# And set stack pointer sp (r29)
*/
	.set noat
SAVE_1_30:
        sw     r0,SR0(k0)	# zero
        sw     r1,SR1(k0)	# at
        sw     r2,SR2(k0)	# v0
        sw     r3,SR3(k0)	# v1
        sw     r4,SR4(k0)	# a0
        sw     r5,SR5(k0)	# a1
        sw     r6,SR6(k0)	# a2
        sw     r7,SR7(k0)	# a3
        sw     r8,SR8(k0)	# t0
        sw     r9,SR9(k0)	# t1
        sw     r10,SR10(k0)	# t2
        sw     r11,SR11(k0)	# t3
        sw     r12,SR12(k0)	# t4
        sw     r13,SR13(k0)	# t5
        sw     r14,SR14(k0)	# t6
        sw     r15,SR15(k0)	# t7
        sw     r16,SR16(k0)	# s0
        sw     r17,SR17(k0)	# s1
        sw     r18,SR18(k0)	# s2
        sw     r19,SR19(k0)	# s3
        sw     r20,SR20(k0)	# s4
        sw     r21,SR21(k0)	# s5
        sw     r22,SR22(k0)	# s6
        sw     r23,SR23(k0)	# s7
        sw     r24,SR24(k0)	# t8
        sw     r25,SR25(k0)	# t9
        sw     r26,SR26(k0)	# k0
//        sw     r27,SR27(k0)	# k1	/* k1 saved in EXCEP or GOA */
        sw     r28,SR28(k0)	# gp
        sw     r29,SR29(k0)	# sp
        sw     r30,SR30(k0)	# fp

        mfhi    t0
        mflo    t1
	sw	t0,SHI(k0)
	sw	t1,SLO(k0)
	
        li     sp,EXCEPTION_STACK # set sp
        jr     ra
	nop

/* --------------------------------------------------
* Restore r1-r30
*/
LOAD_1_30:
	lw	t0,SHI(k0)
	lw	t1,SLO(k0)
	mthi    t0
	mtlo    t1
        lw     r1,SR1(k0)
        lw     r2,SR2(k0)
        lw     r3,SR3(k0)
        lw     r4,SR4(k0)
        lw     r5,SR5(k0)
        lw     r6,SR6(k0)
        lw     r7,SR7(k0)
        lw     r8,SR8(k0)
        lw     r9,SR9(k0)
        lw     r10,SR10(k0)
        lw     r11,SR11(k0)
        lw     r12,SR12(k0)
        lw     r13,SR13(k0)
        lw     r14,SR14(k0)
        lw     r15,SR15(k0)
        lw     r16,SR16(k0)
        lw     r17,SR17(k0)
        lw     r18,SR18(k0)
        lw     r19,SR19(k0)
        lw     r20,SR20(k0)
        lw     r21,SR21(k0)
        lw     r22,SR22(k0)
        lw     r23,SR23(k0)
        lw     r24,SR24(k0)
        lw     r25,SR25(k0)
/*       lw     r26,SR26(k0) */
        lw     r27,SR27(k0)	/* k1 saved in EXCEP or GOA */
        lw     r28,SR28(k0)
        lw     r29,SR29(k0)
        lw     r30,SR30(k0)
        jr     ra
	nop
/* -------------------------------------------------- */
/* Save CP0 registers */

save_cp0:	/* k0 must be set */

/* save Cp0 registers */
	mfc0    t0,C0_SR
	mfc0    t1,C0_EPC
	mfc0    t2,C0_CAUSE
	sw      t0,SSR(k0)
	sw      t1,SEPC(k0)
	sw      t2,SCAUSE(k0)
	mfc0    t1,C0_BADVADDR
	mfc0    t2,C0_CONFIG
        nop
	sw      t1,SBADVADDR(k0)
	sw      t2,SCONFIG(k0)

	jr	ra
	nop
	

/* --------------------------------------------------
# Print standard message about exception
#       a0=text address
*/
ExcMess:
        addiu  sp,sp,-8
        sw     ra,(sp)
	sw     a0,4(sp)
        la     a0,T_EXCEP    # a0=text 'EXCEPTION: '
        jal    tos_asm
	nop
        jal    tos_asm
        lw     a0,4(sp)

        la     a0,T_EPC
        jal    tos_asm
	nop
	jal	ton_asm
	lw	a0,SEPC(k0)

        la     a0,T_SR
        jal    tos_asm
	nop
	jal	ton_asm
	lw	a0,SSR(k0)

        la     a0,T_CAUSE
        jal    tos_asm
	nop
	jal	ton_asm
	lw	a0,SCAUSE(k0)

        la     a0,T_BADVADDR
        jal    tos_asm
	nop
	jal	ton_asm
	lw	a0,SBADVADDR(k0)

        la     a0,T_CR
        jal    tos_asm
	nop

        lw     ra,(sp)
	lw     a0,4(sp)
        jr     ra
        addiu  sp,sp,8

T_EXCEP:  .asciiz  "\n\rExc "
T_EPC:    .asciiz  "EPC "
T_SR:     .asciiz  " SR "
T_CAUSE:  .asciiz  " CAUSE "
T_BADVADDR:  .asciiz  " BADVADDR "
	  .align 2

/*--------------------------------------------------------------
--------------------------------------------------------------*/
MemCtrl_Init:
	la	a0,MEM_0_BASE	// ptr = BaseAddress of first address range control unit
	la	a1,MemParTable	// from = &MemParTable
	la	a2,MemParOffsTable

	li	t0,8	// loop_cnt (number of ranges)
0:
	  lw	v0,0x0(a1)	// Lo address
	  lw	v1,0x4(a1)	// Hi address
	  sw	v0,oLO_BORDER(a0)
	  sw	v1,oHI_BORDER(a0)

	  lw	v0,0x8(a1)	// Delay
	  lw	v1,0xC(a1)	// Wait states
	  sw	v0,oDELAY(a0)
	  sw	v1,oWAIT_STATES(a0)

	  lw	v0,0x10(a1)	// Offset Delay
	  lw	v1,0x14(a1)	// Config
	  sw	v0,oOFF_DELAY(a0)
	  sw	v1,oCONFIG(a0)


	  lw	v0,0x18(a1)	// Offset Delay
	  addiu	t0,t0,-1	// loop_cnt-- (in load delay slot)
	  sw	v0,oBURST_CONFIG(a0)

	  addiu a2,a2,4  
	  lw	t2, 0x0(a2)
	  la	a0,MEM_0_BASE	// ptr = BaseAddress of first address range control unit
	  add	a0, a0, t2
	  bgtz	t0,0b
	  addiu	a1,0x20		// from += 8 (in int32)
	  


//.....	Small delay
	li	t0,0x10
1:
	bgtz	t0,1b
	addiu	t0,t0,-1

	jr	ra
	nop

	.align	4
MemParTable:
/*					LO_BORDER	HI_BORDER	DELAY		WAIT_STATES	OFF_DELAY	CONFIG		BURST_CONFIG*/
/*CS0(ROM)*/  .word	0x1fc00000, 0x1fe00000, 0xff070702, 0x00000F0F, 0x14121214, 0x00000400,	0x0000060F, 0		// 0
/*CS1     */  .word	0x00000000, 0x00100000, 0xFF000000, 0x00000303,	0x03030303, 0x00001900,	0x80000003, 0		// 1
/*Flash   */  .word	0x04500000, 0x04500000, 0x20020202, 0x00000808, 0x080A0A0A, 0x000041f1,	0x80000206, 0		// 2 5
/*CS2 	  */  .word	0x00200000, 0x00300000, 0xFF000000, 0x00000303, 0x03030303, 0x00001900,	0x80000003, 0		// 3 2
/*CS3	  */  .word	0x00400000, 0x00500000, 0xFF000000, 0x00000303, 0x03030303, 0x00001900,	0x80000003, 0 		// 4 3
/*Bank3   */  .word	0x04400000, 0x00100000, 0x20020202, 0x00000808, 0x08080808, 0x00000982,	0x80000206, 0		// 5 4
/*Rsrv    */  .word	0x04600000, 0x04600000, 0x02020202, 0x00000808, 0x08080808, 0x00004a04,	0x80000206, 0		// 6 6
/*Rsrv    */  .word	0x04700000, 0x04700000, 0x05020202, 0x00000808, 0x08080808, 0x00000986,	0x80000206, 0		// 7 7
		
MemParOffsTable:
	    .word	0,0x00000100, 0x00000500, 0x00000200, 0x00000300, 0x00000400, 0x00000600, 0x00000700 				


/* --------------------------------------------------*/


ENDMON:

T_U_K:   .asciiz  "Reference to Kernel Space in User mode\r\n"
T_ULF:   .asciiz  "Unaligned Address\r\n"
T_UNKN:  .asciiz  "UnKnown Exception\r\n"
T_IBE:   .asciiz  "Bus Error - Instruction fetch\r\n"
T_DBE:   .asciiz  "Bus Error - Data reference\r\n"
T_OV:    .asciiz  "Integer Overflow\r\n"
T_IWATCH:
	 .asciiz  "IWatch\r\n"
T_DWATCH:
	 .asciiz  "DWatch\r\n"
T_SYS:   .asciiz  "System Call\r\n"
T_BP:    .asciiz  "Breakpoint\r\n"
T_RI:    .asciiz  "Reserved Instruction\r\n"
T_FPE:	 .asciiz  "FPU Exception\r\n"
T_CPU:   .asciiz  "Coprocessor Unusable\r\n"
T_UINT:  .asciiz  "Unknown Hardware Interrupt\r\n"
T_HIWE:  .asciiz  "Write Error Interrupt\r\n"
T_SINT1: .asciiz  "Soft Interrupt 1\r\n"
T_SINT2: .asciiz  "Soft Interrupt 2\r\n"
T_MOD:   .asciiz  "TLB Modified\r\n"
T_TLBI:  .asciiz  "TLB Invalid\r\n"
T_TLBR:  .asciiz  "TLB Refill\r\n"
T_CR:	 .asciiz  "\n\r"
	 .align 2
/* -------------------------------------------------- */
