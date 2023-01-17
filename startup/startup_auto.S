/*
 * Unified startup file for all platforms.
 * (for autonomous binaries)
 *
 * Features:
 *   + interrupt vectors in memory (BEV = 0)
 *   + full exception handling support
 *
 * Based on startup-file for 32-bit platforms.
 */

#define pADDR .word

/*
 * Include some files
 */
#include "regdef.h"
#include "kernel.h"
#include "exc_vectors.h"

	.set noreorder
	.set noat
	.text

	.globl	_start
_start:
	.align	3

	.org	0x200

/*-------------------------------------------------------*/
/*------------------- Main routine ----------------------*/
/*-------------------------------------------------------*/
startup:
	/* Initialize sp & gp */
	li	sp, PROGRAM_STACK	
	la	gp, _gp      // set the global data pointer, _gp got from linker script
	addiu	sp, sp, -48
	sw	s0, 0(sp)    // used for exit code
	la	t0, RetAddress
	sw	ra, 0(t0)

	/* Update Status & FCSR */
	la	t0, OS_Status
	mfc0	t1, C0_SR
        nop
	sw	t1, (t0)
	li	t0, 0x70000000	/* turn on all coprocessors */
	or	t1, t1, t0
	mtc0	t1, C0_SR

	la	t0, OS_FCSR
	cfc1	t1, C1_FCSR
        nop
	sw	t1, (t0)
	li	t0, 0xFFFC0000
	and	t1, t1, t0
	ctc1	t1, C1_FCSR

	/* Set exception handlers */
	bal	SetExcHandlers
	nop

	/* Set arguments for main */
	addiu	a1, sp, 32	/* argv = sp + 32 */
	addiu	a2, sp, 40	/* envp = sp + 40 */
	sw	zero, (a1)	/* argv[argc] = 0 */
	sw	zero, (a2)	/* envp[0] = 0 */

	/* Execute user program */
	jal	main
	move	a0, zero	/* argc = 0 */

	la	t9, exit
	beq	t9, zero, _exit
	move	a0, v0  /* Pass exit code to _exit() or exit() */
	jalr	t9	/* Call exit() if using libc (if found) */
	nop


	.globl	_exit
	.ent _exit
_exit:
	/* Initialize sp */
	li	sp, PROGRAM_STACK	
	addiu	sp, sp, -48

	move	s0, a0  /* Preserve exit code */

	/* Restore exception handlers */
	bal	RestoreExcHandlers
	nop

	/* Restore exit code */
	move	v0, s0

	/* Return to boot/OS */
	la	t0, RetAddress
	lw	ra, 0(t0)
	lw	s0, 0(sp)
	jr	ra
	nop

	.end _exit


	.data

	.align	3
OS_Status:
	.ds	0x4
OS_FCSR:
	.ds	0x4

	.align	3
	.text

/*-------------------------------------------------------*/
/*---------- Save/Restore handlers functions ------------*/
/*-------------------------------------------------------*/

/*
 * Helper copy function
 * a0 = source, a1 = dest
 */
CopyHandler:
	lw	t0, 0x00(a0)
	lw	t1, 0x04(a0)
	lw	t2, 0x08(a0)
	lw	t3, 0x0C(a0)
	sw	t0, 0x00(a1)
	sw	t1, 0x04(a1)
	sw	t2, 0x08(a1)
	sw	t3, 0x0C(a1)
	jr	ra
	nop


SetExcHandlers:
	addiu	sp, sp, -8
	sw	ra, 0(sp)

	/* Save old handlers from offset 0x00 */
        li      a0, 0xA0000000
	la	a1, OldHandlers
	bal	CopyHandler
	nop

	/* Save old handlers from offset 0x80 */
	addiu	a0, a0, 0x80
	addiu	a1, a1, 0x10
	bal	CopyHandler
	nop

#ifndef _MIPS_ARCH_MIPS1
	/* Save old handlers from offset 0x100 */
	addiu	a0, a0, 0x80
	addiu	a1, a1, 0x10
	bal	CopyHandler
	nop

	/* Save old handlers from offset 0x180 */
	addiu	a0, a0, 0x80
	addiu	a1, a1, 0x10
	bal	CopyHandler
	nop
#endif /* _MIPS_ARCH_MIPS1 */

	/* Set own handlers to offset 0x00 */
        la      a0, HandlerImage_General
        li      a1, 0xA0000000
	bal	CopyHandler
	nop

	/* Set own handlers to offset 0x80 */
	addiu	a1, a1, 0x80
	bal	CopyHandler
	nop

#ifndef _MIPS_ARCH_MIPS1
	/* Set own handlers to offset 0x100 */
	addiu	a1, a1, 0x80
	bal	CopyHandler
	nop

	/* Set own handlers to offset 0x180 */
	addiu	a1, a1, 0x80
	bal	CopyHandler
	nop
#endif /* _MIPS_ARCH_MIPS1 */

	/* Init Exception vector table */
	li	t0, VECT_BASE
	addiu	t1, t0, NVECT
0:	sw	zero, (t0)
	bne	t1, t0, 0b
	addiu	t0, t0, 4

	/* Store old Status and clear BEV in Status */
	la	t0, OldStatus
	mfc0	t1, C0_SR
        nop
	sw	t1, (t0)
	li	t0, 0xFFBFFFFF
	and	t0, t0, t1
	mtc0	t0, C0_SR

	/* Invalidate cache (for general exceptions) */
//	bal	icache_invalidate
	jal	flushcache
	nop

	lw	ra, 0(sp)
	nop	// load delay slot!!! (VM1/VM2)
        jr      ra
	addiu	sp, sp, 8


RestoreExcHandlers:
	addiu	sp, sp, -8
	sw	ra, 0(sp)

	/* Restore old handlers to offset 0x00 */
	la	a0, OldHandlers
        li      a1, 0xA0000000
	bal	CopyHandler
	nop

	/* Restore old handlers to offset 0x80 */
	addiu	a0, a0, 0x10
	addiu	a1, a1, 0x80
	bal	CopyHandler
	nop

#ifndef _MIPS_ARCH_MIPS1
	/* Restore old handlers to offset 0x100 */
	addiu	a0, a0, 0x10
	addiu	a1, a1, 0x80
	bal	CopyHandler
	nop

	/* Restore old handlers to offset 0x180 */
	addiu	a0, a0, 0x10
	addiu	a1, a1, 0x80
	bal	CopyHandler
	nop
#endif /* _MIPS_ARCH_MIPS1 */

	/* Restore old Status */
	la	t0, OldStatus
	lw	t0, (t0)
	nop	// load delay slot!!! (VM1/VM2)
	mtc0	t0, C0_SR

	/* Invalidate cache (for general exceptions) */
	jal	flushcache
	nop

	lw	ra, 0(sp)
	nop	// load delay slot!!! (VM1/VM2)
        jr      ra
	addiu	sp, sp, 8


/*
 * These functions' data section
 */
	.data

	.align	3
HandlerImage_General:    /* To be copied to 0xA0000000, 0xA0000080 etc. */
        la	k0, EXC_GENERAL
        jr      k0
        nop

	.align 3
OldStatus:
	.ds	0x8
OldHandlers:
	.ds	0x20

	.text


/*-------------------------------------------------------*/
/*----------------- Exception Handlers ------------------*/
/*-------------------------------------------------------*/

EXC_GENERAL:
	li	k0, SREGS       /* k0=regfile base address */
	sw	r31, SR31(k0)   # ra
	bal     SAVE_STATE      /* save r1-r30 and set sp */
	nop

	lw	t0, SCAUSE(k0)
	la	t1, ExcTable    /* table address */
        andi    t0, t0, 0x7c    /* offset in table */
	addu	t0, t0, t1      /* address of handler */
	lw	t0, (t0)        /* handler address */
	la	a0, VECT_BASE

	/* get handler address */
	jr	t0
	nop

common_exc:
	beq	zero, v0, 1f
	nop
	jalr	v0    /* user handler */
	nop
	b	ret_from_exc
	nop
1:	bal	ExcMessage
	nop

	/* Return to boot/OS */
	la	t0, RetAddress
	lw	ra, 0(t0)
	nop	// load delay slot!!! (VM1/VM2)
	jr	ra
	li	v0, 2

	/* successful return from user's handler */
ret_from_exc:
	bal	LOAD_STATE
	nop

	// Restore sw (may be KU bits are changed)
	lw	ra,SSR(k0)
	nop
	mtc0	ra,C0_SR

	lw	ra, SR31(k0)
	lw	k0, SEPC(k0)
	nop		// load delay slot (for VM1/VM2)!!!
	jr	k0
	rfe


Int:
	/* redirect FPU exceptions to FPE vector */
	/* check if cp1 is usable */
	mfc0	t0, C0_SR
	lui	t2, 0x2000
	and	t0, t0, t2
	beqz	t0, 0f
	nop

	/* check IP (Cause) with FPInt (Config) value */
	mfc0	t0, C0_CONFIG
	mfc0	t2, C0_CAUSE
	srl	t0, t0, 26
	srl	t2, t2, 10
	andi	t0, t0, 0x7
	srlv	t2, t2, t0
	andi	t2, t2, 1
	bnez	t2, FPE
	nop

0:	lw	v0, Int_v(a0)
	nop
	b	common_exc
	nop


TLBmod:	lw	v0, TLBmod_v(a0)
	nop
	b	common_exc
	nop


TLBL:	lw	v0, TLBL_v(a0)
	nop
	b	common_exc
	nop


TLBS:	lw	v0, TLBS_v(a0)
	nop
	b	common_exc
	nop


ADEL:	lw	v0, ADEL_v(a0)
	nop
	b	common_exc
	nop


ADES:	lw	v0, ADES_v(a0)
	nop
	b	common_exc
	nop


IBE:	lw	v0, IBE_v(a0)
	nop
	b	common_exc
	nop


DBE:	lw	v0, DBE_v(a0)

	nop
	b	common_exc
	nop


Sys:	lw	v0, Sys_v(a0)
	nop
	b	common_exc
	nop


Bp:	lw	v0, Bp_v(a0)
	nop
	b	common_exc
	nop


RI:	lw	v0, RI_v(a0)
	nop
	b	common_exc
	nop


CpU:	lw	v0, CpU_v(a0)
	nop
	b	common_exc
	nop


Ov:	lw	v0, Ov_v(a0)
	nop
	b	common_exc
	nop


Tr:	lw	v0, Tr_v(a0)
	nop
	b	common_exc
	nop


Reserved_exc:
	lw	v0, Reserved_exc_v(a0)
	nop
	b	common_exc
	nop

/*-------------------- FPE Exception --------------------*/

FPE:	lw	v0, FPE_v(a0)
	nop
	beq	zero, v0, FPE_softfloat
	nop
	jalr	v0   /* user handler */
	nop
	b	ret_from_exc
	nop

FPE_softfloat:
	/* check for handler existence (libsoftfloat linked) */
	.globl	softfloat_fpe_handler
        .text
	la	t9, softfloat_fpe_handler
	beqz	t9, FPE_raise_SIGFPE
	nop

	move	a0, k0
	jalr	t9
	addiu	a1, k0, SEPC  /* a1 now points to saved epc */

	beqz	v0, ret_from_exc_softfloat  /* emulation successful */
	nop

	b	0f
	move	s3, v0        /* in other case, use handler's result for exc message */


	/* bad result */
FPE_raise_SIGFPE:
	cfc1	s3, C1_FCSR
        nop
	srl	s3, s3, 12    /* use FCSR cause for exc message */
	andi	s3, s3, 0x7F

	/* check for raise() existence (libc linked) */
0:	.globl	raise
	.text
	la	a0, raise
	beq	a0, zero, fpe_exc_message
	nop

	/* return to raise instead of back to program */
	la	t9, raise
	jalr	t9
	li	a0, 8 /* value of SIGFPE */

	/* return to program if raise signal is ignored */
	/* (fall through) */


	/* emulation successful */
ret_from_exc_softfloat:
	cfc1	t0, C1_FCSR
	li	t1, 0xFFFC0FFF
	and	t0, t0, t1
	ctc1	t0, C1_FCSR

	b	ret_from_exc
	nop


	/* default exc message, if no handlers */
fpe_exc_message:
	la	a0, ST_FPE
	bal	ExcMessage
	nop
	la	a0, ST_FPAEXC
	jal	tos_asm
	nop
	lui	a1, 2
	and	a1, s3, a1
	beqz	a1, 2f
	nop
	jal	tob_asm
	ori	a0, zero, 'E'
2:	lui	a1, 1
	and	a1, s3, a1
	beqz	a1, 3f
	nop
	jal	tob_asm
	ori	a0, zero, 'V'
3:	li	a1, 0x8000
	and	a1, s3, a1
	beqz	a1, 4f
	nop
	jal	tob_asm
	ori	a0, zero, 'Z'
4:	ori	a1, zero, 0x4000
	and	a1, s3, a1
	beqz	a1, 5f
	nop
	jal	tob_asm
	ori	a0, zero, 'O'
5:	ori	a1, zero, 0x2000
	and	a1, s3, a1
	beqz	a1, 6f
	nop
	jal	tob_asm
	ori	a0, zero, 'U'
6:	ori	a1, zero, 0x1000
	and	a1, s3, a1
	beqz	a1, 7f
	nop
	jal	tob_asm
	ori	a0, zero, 'I'

7:	/* Emergency exit */
	b	_exit
	li	a0, 1


/*------------------- Helper Functions ------------------*/
SAVE_STATE:
	sw	r0, SR0(k0)     # zero
	sw	r1, SR1(k0)     # at
	sw	r2, SR2(k0)     # v0
	sw	r3, SR3(k0)     # v1
	sw	r4, SR4(k0)     # a0
	sw	r5, SR5(k0)     # a1
	sw	r6, SR6(k0)     # a2
	sw	r7, SR7(k0)     # a3
	sw	r8, SR8(k0)     # t0
	sw	r9, SR9(k0)     # t1
	sw	r10, SR10(k0)   # t2
	sw	r11, SR11(k0)   # t3
	sw	r12, SR12(k0)   # t4
	sw	r13, SR13(k0)   # t5
	sw	r14, SR14(k0)   # t6
	sw	r15, SR15(k0)   # t7
	sw	r16, SR16(k0)   # s0
	sw	r17, SR17(k0)   # s1
	sw	r18, SR18(k0)   # s2
	sw	r19, SR19(k0)   # s3
	sw	r20, SR20(k0)   # s4
	sw	r21, SR21(k0)   # s5
	sw	r22, SR22(k0)   # s6
	sw	r23, SR23(k0)   # s7
	sw	r24, SR24(k0)   # t8
	sw	r25, SR25(k0)   # t9
/*	sw	r26, SR26(k0)   # k0 */
	sw	r27, SR27(k0)   # k1
	sw	r28, SR28(k0)   # gp
	sw	r29, SR29(k0)   # sp
	sw	r30, SR30(k0)   # fpd

	la	sp, ExcStack    # set sp

	mfhi	t0
	mflo	t1
	sw	t0, SHI(k0)
	sw	t1, SLO(k0)

	mfc0	t0, C0_SR
	mfc0	t1, C0_EPC
	mfc0	t2, C0_CAUSE
	mfc0	t3, C0_BADVADDR
	sw	t0, SSR(k0)
	sw	t1, SEPC(k0)
	sw	t2, SCAUSE(k0)
	sw	t3, SBADVADDR(k0)

        jr	ra
        nop


LOAD_STATE:
	lw	t0, SHI(k0)
	lw	t1, SLO(k0)
	mthi	t0
	mtlo	t1

	lw	r1, SR1(k0)
	lw	r2, SR2(k0)
	lw	r3, SR3(k0)
	lw	r4, SR4(k0)
	lw	r5, SR5(k0)
	lw	r6, SR6(k0)
	lw	r7, SR7(k0)
	lw	r8, SR8(k0)
	lw	r9, SR9(k0)
	lw	r10, SR10(k0)
	lw	r11, SR11(k0)
	lw	r12, SR12(k0)
	lw	r13, SR13(k0)
	lw	r14, SR14(k0)
	lw	r15, SR15(k0)
	lw	r16, SR16(k0)
	lw	r17, SR17(k0)
	lw	r18, SR18(k0)
	lw	r19, SR19(k0)
	lw	r20, SR20(k0)
	lw	r21, SR21(k0)
	lw	r22, SR22(k0)
	lw	r23, SR23(k0)
	lw	r24, SR24(k0)
	lw	r25, SR25(k0)
/*	lw	r26, SR26(k0) */
	lw	r27, SR27(k0)
	lw	r28, SR28(k0)
	lw	r29, SR29(k0)
	lw	r30, SR30(k0)
	jr	ra
	nop

/*
 * Exception message. Return to boot/OS after output.
 */
ExcMessage:
	addiu	sp, sp, -8
	sw	ra, 0(sp)

	la	a0, T_EXCEP     /* "Exception" message */
	jal	tos_asm
	nop

	lw	a0, SCAUSE(k0)
	la	t1, ExcMsgTable /* table address */
	andi	a0, a0, 0x7c    /* offset in table */
	addu	a0, a0, t1      /* address of message string in table */
	jal	tos_asm
	lw	a0, (a0)

	la	a0, T_EPC       /* "EPC" string */
	jal	tos_asm
	nop
	jal	ton_asm
	lw	a0, SEPC(k0)

	la	a0, T_SR        /* "sw" string */
	jal	tos_asm
	nop
	jal	ton_asm
	lw	a0, SSR(k0)

	la	a0, T_CAUSE     /* "Cause" string */
	jal	tos_asm
	nop
	jal	ton_asm
	lw	a0, SCAUSE(k0)

    la     a0,T_BADVADDR
    jal    tos_asm
    nop
    jal ton_asm
    lw  a0,SBADVADDR(k0)

	jal	tob_asm
	ori	a0, zero, '\n'

	lw	ra, 0(sp)
	nop			// load delay slot!!!	
	jr	ra
	addiu	sp, sp, 8


/* THIS FUNCTION SHOULD BE IN CORE LIBRARY!!! */
icache_invalidate:
	la	t0, ic_inv
	li	t1, 0x20000000
	or	t0, t0, t1	/* uncached address */
	jr	t0
	nop
ic_inv:
	mfc0	t0, C0_SR
	li	t1, 0x00020000	/* STATUS.ICVR bit */
	or	t0, t0, t1
	mtc0	t0, C0_SR
	xor	t0, t0, t1
	mtc0	t0, C0_SR

	/* return to cached (if any) */
	la	t0, ic_inv_end
	jr	t0
	nop
ic_inv_end:
	jr	ra
	nop
/*--------------------- Data Section --------------------*/
	.data
	.align 3

/* Stack for exception processing */
	.ds	0x200
ExcStack:
	.ds	16

RetAddress:
	.ds	4

	.align	3
ExcTable:
	pADDR Int     # 0
	pADDR TLBmod  # 1
	pADDR TLBL    # 2
	pADDR TLBS    # 3
	pADDR ADEL    # 4
	pADDR ADES    # 5
	pADDR IBE     # 6
	pADDR DBE     # 7
	pADDR Sys     # 8
	pADDR Bp      # 9
	pADDR RI      # 10
	pADDR CpU     # 11
	pADDR Ov      # 12
	pADDR Tr      # 13
	pADDR Reserved_exc  # 14
	pADDR FPE     # 15
	pADDR Reserved_exc  # 16
	pADDR Reserved_exc  # 17
	pADDR Reserved_exc  # 18
	pADDR Reserved_exc  # 19
	pADDR Reserved_exc  # 20
	pADDR Reserved_exc  # 21
	pADDR Reserved_exc  # 22
	pADDR Reserved_exc  # 23
	pADDR Reserved_exc  # 24
	pADDR Reserved_exc  # 25
	pADDR Reserved_exc  # 26
	pADDR Reserved_exc  # 27
	pADDR Reserved_exc  # 28
	pADDR Reserved_exc  # 29
	pADDR Reserved_exc  # 30
	pADDR Reserved_exc  # 31

ExcMsgTable:
	pADDR ST_Int     # 0
	pADDR ST_TLBmod  # 1
	pADDR ST_TLBL    # 2
	pADDR ST_TLBS    # 3
	pADDR ST_ADEL    # 4
	pADDR ST_ADES    # 5
	pADDR ST_IBE     # 6
	pADDR ST_DBE     # 7
	pADDR ST_Sys     # 8
	pADDR ST_Bp      # 9
	pADDR ST_RI      # 10
	pADDR ST_CpU     # 11
	pADDR ST_Ov      # 12
	pADDR ST_Tr      # 13
	pADDR ST_Reserved_exc  # 14
	pADDR ST_FPE     # 15
	pADDR ST_Reserved_exc  # 16
	pADDR ST_Reserved_exc  # 17
	pADDR ST_Reserved_exc  # 18
	pADDR ST_Reserved_exc  # 19
	pADDR ST_Reserved_exc  # 20
	pADDR ST_Reserved_exc  # 21
	pADDR ST_Reserved_exc  # 22
	pADDR ST_Reserved_exc  # 23
	pADDR ST_Reserved_exc  # 24
	pADDR ST_Reserved_exc  # 25
	pADDR ST_Reserved_exc  # 26
	pADDR ST_Reserved_exc  # 27
	pADDR ST_Reserved_exc  # 28
	pADDR ST_Reserved_exc  # 29
	pADDR ST_Reserved_exc  # 30
	pADDR ST_Reserved_exc  # 31

T_EXCEP:        .asciiz "\n\rException: "
T_EPC:          .asciiz "EPC="
T_SR:           .asciiz " sw="
T_CAUSE:        .asciiz " CAUSE="
ST_FPAEXC:      .asciiz "\nFPA exc flags:"
T_BADVADDR:     .asciiz  " BADVADDR "

ST_Int:         .asciiz "Interrupt\n\r"
ST_TLBmod:      .asciiz "TLB mod\n\r"
ST_TLBL:        .asciiz "TLBL\n\r"
ST_TLBS:        .asciiz "TLBS\n\r"
ST_ADEL:        .asciiz "ADEL\n\r"
ST_ADES:        .asciiz "ADES\n\r"
ST_IBE:         .asciiz "IBE\n\r"
ST_DBE:         .asciiz "DBE\n\r"
ST_Sys:         .asciiz "Syscall\n\r"
ST_Bp:          .asciiz "Breakpoint\n\r"
ST_RI:          .asciiz "Reserved instruction\n\r"
ST_CpU:         .asciiz "Coprocessor unusable\n\r"
ST_Ov:          .asciiz "Integer Overflow\n\r"
ST_Tr:          .asciiz "Trap\n\r"
ST_Reserved_exc: .asciiz "Reserved Exc Code\n\r"
ST_FPE:         .asciiz "Floating point exception\n\r"
