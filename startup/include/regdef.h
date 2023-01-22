#ifndef _ARCH_KOMDIV_REGDEF_H_
#define _ARCH_KOMDIV_REGDEF_H_

#define CPU_1B812_ID        0x01000230
#define CPU_1890BM1_ID      0x01000230
#define CPU_5890BE1_v1_ID   0x01000231
#define CPU_5890BE1_v2_ID   0x01000232
#define CPU_1890BM2_ID      0x01000233
#define CPU_KOMDIV32_P_ID   0x01000234
#define CPU_OKVS_N_OLD_ID   0x01000235
#define CPU_OKVS_N_ID	    0x01001201
#define CPU_RESERVE32_ID    0x01000240
#define CPU_LIGATURE32_ID   0x01001101	// was 0x01000260
#define CPU_PERGA_ID        0x01001400
#define CPU_SPICA128_ID     0x01001501
#define CPU_FORTRESS_ID     0x01001300
#define CPU_FORTRESS_v2_ID  0x01001301
#define CPU_ORBITA_ID       0x01001601
#define CPU_TRIPLE_ID       0x01001700
#define CPU_VM3_ID          0x12348105

#ifdef __LANGUAGE_ASSEMBLY__

#define r0	$0
#define r1	$1
#define r2	$2
#define r3	$3
#define r4	$4
#define r5	$5
#define r6	$6
#define r7	$7
#define r8	$8
#define r9	$9
#define r10	$10
#define r11	$11
#define r12	$12
#define r13	$13
#define r14	$14
#define r15	$15
#define r16	$16
#define r17	$17
#define r18	$18
#define r19	$19
#define r20	$20
#define r21	$21
#define r22	$22
#define r23	$23
#define r24	$24
#define r25	$25
#define r26	$26
#define r27	$27
#define r28	$28
#define r29	$29
#define r30	$30
#define r31	$31

/* aliases for general registers */
#define zero		$0
#define	at		$1		/* assembler temporaries */
#define	v0		$2		/* value holders */
#define	v1		$3
#define	a0		$4		/* arguments */
#define	a1		$5
#define	a2		$6
#define	a3		$7
#define	t0		$8		/* temporaries */
#define	t1		$9
#define	t2		$10
#define	t3		$11
#define	t4		$12
#define	t5		$13
#define	t6		$14
#define	t7		$15
#define	s0		$16		/* saved registers */
#define	s1		$17
#define	s2		$18
#define	s3		$19
#define	s4		$20
#define	s5		$21
#define	s6		$22
#define	s7		$23
#define	t8		$24		/* temporaries */
#define	t9		$25
#define	k0		$26		/* kernel registers */
#define	k1		$27
#define	gp		$28		/* global pointer */
#define	sp		$29		/* stack pointer */
#define	s8		$30		/* saved register */
#define	fp		$30		/* frame pointer (old usage) */
#define	ra		$31		/* return address */

#define fp0	$f0
#define fp1	$f1
#define fp2	$f2
#define fp3	$f3
#define fp4	$f4
#define fp5	$f5
#define fp6	$f6
#define fp7	$f7
#define fp8	$f8
#define fp9	$f9
#define fp10	$f10
#define fp11	$f11
#define fp12	$f12
#define fp13	$f13
#define fp14	$f14
#define fp15	$f15
#define fp16	$f16
#define fp17	$f17
#define fp18	$f18
#define fp19	$f19
#define fp20	$f20
#define fp21	$f21
#define fp22	$f22
#define fp23	$f23
#define fp24	$f24
#define fp25	$f25
#define fp26	$f26
#define fp27	$f27
#define fp28	$f28
#define fp29	$f29
#define fp30	$f30
#define fp31	$f31

/* Floating-Point Control registers */
#define C1_FC0	$0		/* Implementation/Rev register */
#define C1_IRR	$0		/* Implementation/Rev register */
#define fc0	$0
#define C1_IR	$0
#define C1_FCONFIG	$24
#define C1_FCCR	$25
#define C1_FEXR	$26
#define C1_FENR	$28
#define C1_CSR	$31		/* Control/Status register */
#define fc31	$31
#define C1_SR	$31
#define C1_FC31	$31
#define C1_FCSR $31


/* System Control Coprocessor (CP0) registers */
#define C0_INDEX	$0
#define C0_RANDOM	$1
#define C0_TLBLO	$2
#define C0_ENTRYLO0	$2
#define C0_ENTRYLO1	$3
#define C0_CONTEXT	$4
#define C0_PAGEMASK	$5
#define C0_WIRED	$6
#define C0_BADVADDR	$8		/* Bad Virtual Address */
#define C0_COUNT	$9
#define C0_TLBHI	$10
#define C0_ENTRYHI	$10
#define C0_COMPARE	$11
#define C0_SR		$12		/* Processor Status */
#define C0_STATUS	$12
#define C0_CAUSE	$13		/* Exception Cause */
#define C0_EPC		$14		/* Exception PC */
#define C0_PRID		$15		/* Processor Rev Indentifier */

/* Config Register problem */
#ifdef _MIPS_ARCH_MIPS1
#define C0_CONFIG	$3
#else
#define C0_CONFIG	$16
#endif

/* modern architecture registers */
#define C0_CONFIG0	$16,0
#define C0_CONFIG1	$16,1
#define C0_CONFIG2	$16,2
#define C0_CONFIG3	$16,3
#define C0_CONFIG4	$16,4

#define C0_IWATCH       $18
#define C0_DWATCH       $19

#define C0_CTRL_PARCOUNT $20		/* for 5890VM1T Kvartal */
#define C0_PARCOUNTER    $21		/* for 5890VM1T Kvartal */
#define C0_REGLIST       $22		/* for 5890VM1T Kvartal */

#define C0_CNTERRCTRL	 $20		/* for 1900VM2T Reserve32 */
#define C0_CNTERR	 $21		/* for 1900VM2T Reserve32 */
#define C0_CNTERRMASK	 $22		/* for 1900VM2T Reserve32 */
#define C0_MBIST         $22		/* for Reserve32 redefined to 23 */
#define	C0_CNTERRINDEX	 $24		/* for 1900VM2T Reserve32 */

#define C0_CTRL_ENCOUNT	  $20
#define C0_COUNTERR	  $21
#define C0_MASKCOUNTERR   $22
#define C0_INDEXCOUNTERR  $24	

/* IFlowTrace registers */
#define UserTraceData1	$23,3
#define UserTraceData2	$24,3

/* debug registers */
#define	C0_DEBUG	$23 //,0		// EJTAG
#define	C0_DEPC		$24 //,0		// EJTAG
#define	C0_DESAVE	$31 //,0		// EJTAG

/* k64 registers */
#define C0_XCONTEXT	$20
#define C0_PERFCNT0	$25,0
#define C0_PERFCNT1	$25,1
#define C0_PERFCNT2	$25,2
#define C0_PERFCNT3	$25,3
#define C0_PERFCNT4	$25,4
#define C0_PERFCNT5	$25,5
#define C0_PERFCNT6	$25,6
#define C0_PERFCNT7	$25,7
#define C0_ERRCTL	$26
#define C0_TAGLO	$28,0
#define C0_DATALO	$28,1
#define C0_TAGHI	$29,0
#define C0_DATAHI	$29,1
#define C0_ERROREPC	$30

#endif	/* __LANGUAGE_ASSEMBLY__ */


#ifndef __LANGUAGE_ASSEMBLY__
#define	CACHED_TO_PHYS(x)	((unsigned)(x) & 0x1fffffff)
#define	PHYS_TO_CACHED(x)	((unsigned)(x) | CACHED_MEMORY_ADDR)
#define	UNCACHED_TO_PHYS(x) 	((unsigned)(x) & 0x1fffffff)
#define	PHYS_TO_UNCACHED(x) 	((unsigned)(x) | UNCACHED_MEMORY_ADDR)
#define VA_TO_CINDEX(x) 	((unsigned)(x) & 0xffffff | CACHED_MEMORY_ADDR)
#define	CACHED_TO_UNCACHED(x)	(PHYS_TO_UNCACHED(CACHED_TO_PHYS(x)))
#else
#define	CACHED_TO_PHYS(x)	((x) & 0x1fffffff)
#define	PHYS_TO_CACHED(x)	((x) | CACHED_MEMORY_ADDR)
#define	UNCACHED_TO_PHYS(x) 	((x) & 0x1fffffff)
#define	PHYS_TO_UNCACHED(x) 	((x) | UNCACHED_MEMORY_ADDR)
#define VA_TO_CINDEX(x) 	((x) & 0xffffff | CACHED_MEMORY_ADDR)
#define	CACHED_TO_UNCACHED(x)	(PHYS_TO_UNCACHED(CACHED_TO_PHYS(x)))

#endif /* __LANGUAGE_ASSEMBLY__ */

/*
 * Status register
 */
#define	SR_CUMASK	0xf0000000	/* coproc usable bits */

#define	SR_CU3		0x80000000	/* Coprocessor 3 usable */
#define	SR_CU2		0x40000000	/* Coprocessor 2 usable */
#define	SR_CU1		0x20000000	/* Coprocessor 1 usable */
#define	SR_CU0		0x10000000	/* Coprocessor 0 usable */

/* Cache control bits */
#define	SR_BEV		0x00400000	/* Exceptions in boot region */
#define	SR_TS		0x00200000	/* TLB shutdown */
#define	SR_PE		0x00100000	/* cache parity error */
#define	SR_CM		0x00080000	/* cache miss */
#define	SR_PZ		0x00040000	/* cache parity zero */
#define	SR_SWC		0x00020000	/* swap cache */
#define	SR_ISC		0x00010000	/* Isolate data cache */

/*
 * Bitfields in the Ligature 32 cp0 status register
 */
#define ST0_ICVR       (1 << 17) /* invalidate instruction cache */
#define ST0_DCVR       (1 << 16) /* invalidate data cache */
#define SR_ICVR		ST0_ICVR
#define SR_DCVR		ST0_DCVR

/*
 * Interrupt enable bits
 * (NOTE: bits set to 1 enable the corresponding level interrupt)
 */
#define	SR_IMASK	0xff00	/* Interrupt mask */
#define	SR_IMASK8	0x0000	/* mask level 8 */
#define	SR_IMASK7	0x8000	/* mask level 7 */
#define	SR_IMASK6	0xc000	/* mask level 6 */
#define	SR_IMASK5	0xe000	/* mask level 5 */
#define	SR_IMASK4	0xf000	/* mask level 4 */
#define	SR_IMASK3	0xf800	/* mask level 3 */
#define	SR_IMASK2	0xfc00	/* mask level 2 */
#define	SR_IMASK1	0xfe00	/* mask level 1 */
#define	SR_IMASK0	0xff00	/* mask level 0 */

#define	SR_IBIT8	0x8000	/* bit level 8 */
#define	SR_IBIT7	0x4000	/* bit level 7 */
#define	SR_IBIT6	0x2000	/* bit level 6 */
#define	SR_IBIT5	0x1000	/* bit level 5 */
#define	SR_IBIT4	0x0800	/* bit level 4 */
#define	SR_IBIT3	0x0400	/* bit level 3 */
#define	SR_IBIT2	0x0200	/* bit level 2 */
#define	SR_IBIT1	0x0100	/* bit level 1 */

#define	SR_KUO		0x0020	/* old kernel/user, 0 => k, 1 => u */
#define	SR_IEO		0x0010	/* old interrupt enable, 1 => enable */
#define	SR_KUP		0x0008	/* prev kernel/user, 0 => k, 1 => u */
#define	SR_IEP		0x0004	/* prev interrupt enable, 1 => enable */
#define	SR_KUC		0x0002	/* cur kernel/user, 0 => k, 1 => u */
#define	SR_IEC		0x0001	/* cur interrupt enable, 1 => enable */

#define	SR_IMASKSHIFT	8

#define	SR_FMT		"\20\40BD\26TS\25PE\24CM\23PZ\22SwC\21IsC\20IM7\17IM6\16IM5\15IM4\14IM3\13IM2\12IM1\11IM0\6KUo\5IEo\4KUp\3IEp\2KUc\1IEc"



#define FP_RN 0
#define FP_RZ 1
#define FP_RP 2
#define FP_RM 3

#define FP_RMODE 0x3
#define FP_STKY 0x7c
#define FP_ENABLE 0xf80
#define FP_EXC 0x3f000

#define FP_STKY_I 0x4
#define FP_STKY_U 0x8
#define FP_STKY_O 0x10
#define FP_STKY_Z 0x20
#define FP_STKY_V 0x40
#define FP_EN_I 0x80
#define FP_EN_U 0x100
#define FP_EN_O 0x200
#define FP_EN_Z 0x400
#define FP_EN_V 0x800
#define FP_EXC_I 0x1000
#define FP_EXC_U 0x2000
#define FP_EXC_O 0x4000
#define FP_EXC_Z 0x8000
#define FP_EXC_V 0x10000
#define FP_EXC_E 0x20000
#define FP_COND 0x800000

/*
 * Status register
 */
#define C0_STATUS_IE			(1 << 0)
#define C0_STATUS_EXL			(1 << 1)
#define C0_STATUS_ERL			(1 << 2)
#define C0_STATUS_KSU_KERNEL		((0 << 4) | (0 << 3))
#define C0_STATUS_KSU_SUPERVISOR	((0 << 4) | (1 << 3))
#define C0_STATUS_KSU_USER		((1 << 4) | (0 << 3))
#define C0_STATUS_UX			(1 << 5)
#define C0_STATUS_SX			(1 << 6)
#define C0_STATUS_KX			(1 << 7)
#define C0_STATUS_IM0			(1 << 8)
#define C0_STATUS_IM1			(1 << 9)
#define C0_STATUS_IM2			(1 << 10)
#define C0_STATUS_IM3			(1 << 11)
#define C0_STATUS_IM4			(1 << 12)
#define C0_STATUS_IM5			(1 << 13)
#define C0_STATUS_IM6			(1 << 14)
#define C0_STATUS_IM7			(1 << 15)
#define C0_STATUS_NMI			(1 << 19)
#define C0_STATUS_PE			(1 << 20)
#define C0_STATUS_TS			(1 << 21)
#define C0_STATUS_BEV			(1 << 22)
#define C0_STATUS_PX			(1 << 23)
#define C0_STATUS_ME			(1 << 25)
#define C0_STATUS_FR			(1 << 26)
#define C0_STATUS_RP			(1 << 27)
#define C0_STATUS_CU0			(1 << 28)
#define C0_STATUS_CU1			(1 << 29)
#define C0_STATUS_CU2			(1 << 30)
#define C0_STATUS_CU3			(1 << 31)




#endif /* _ARCH_KOMDIV_REGDEF_H_ */
