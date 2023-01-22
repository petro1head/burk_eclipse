#ifndef _ARCH_KOMDIV_KERNEL_H_
#define _ARCH_KOMDIV_KERNEL_H_

#ifdef __LANGUAGE_ASSEMBLY__

/* Stack base for kernel */
#define KERNEL_STACK	0xa0002000
#define EXCEPTION_STACK	0xa0001000
#define DBG_EXCEPTION_STACK	0xa0001100

#endif /* __LANGUAGE_ASSEMBLY__ */


/* Variables & save area */
#define SREGS		0xa0000200
#define DbgREGS		0xa0000300

#define SR0 0
#define SR1 0x4
#define SR2 0x8
#define SR3 0xc
#define SR4 0x10
#define SR5 0x14
#define SR6 0x18
#define SR7 0x1c
#define SR8 0x20
#define SR9 0x24
#define SR10 0x28
#define SR11 0x2c
#define SR12 0x30
#define SR13 0x34
#define SR14 0x38
#define SR15 0x3c
#define SR16 0x40
#define SR17 0x44
#define SR18 0x48
#define SR19 0x4c
#define SR20 0x50
#define SR21 0x54
#define SR22 0x58
#define SR23 0x5c
#define SR24 0x60
#define SR25 0x64
#define SR26 0x68
#define SR27 0x6c
#define SR28 0x70
#define SR29 0x74
#define SR30 0x78
#define SR31 0x7c

#define S_ZERO    0
#define S_AT   0x04
#define S_V0   0x08
#define S_V1   0x0c
#define S_A0   0x10
#define S_A1   0x14
#define S_A2   0x18
#define S_A3   0x1c
#define S_T0   0x20
#define S_T1   0x24
#define S_T2   0x28
#define S_T3   0x2c
#define S_T4   0x30
#define S_T5   0x34
#define S_T6   0x38
#define S_T7   0x3c
#define S_S0   0x40
#define S_S1   0x44
#define S_S2   0x48
#define S_S3   0x4c
#define S_S4   0x50
#define S_S5   0x54
#define S_S6   0x58
#define S_S7   0x5c
#define S_T8   0x60
#define S_T9   0x64
#define S_K0   0x68
#define S_K1   0x6c
#define S_GP   0x70
#define S_SP   0x74
#define S_FP   0x78
#define S_RA   0x7c

#define SEPC	  0x80
#define SSR	  0x84
#define SBADVADDR 0x88
#define SCAUSE	  0x8c
#define SCONTEXT  0x90
#define SHI	  0x94
#define SLO	  0x98
#define SCONFIG   0x9c
#define SINDEX    0xa0
#define STLBLO    0xa4
#define SENTRYLO0 STLBLO
#define STLBHI    0xa8
#define SENTRYHI  STLBHI
#define EXCTYPE   0xac
#define SFC31     0xb0

#define FP00      0xb4
#define FP01      0xb8
#define FP02      0xbc
#define FP03      0xc0
#define FP04      0xc4
#define FP05      0xc8

#define SDEPC	  0xCC
#define SDEBUG	  0xD0
#define SREENTRY  0xD4

#define EXC_COUNT 0xD8
#define SENTRYLO1 0xdc

#endif /* _ARCH_KOMDIV_KERNEL.H_ */
