#ifndef _ASM_SCORE_ASMMACRO_H
#define _ASM_SCORE_ASMMACRO_H

#include <asm/asm-offsets.h>

#ifdef __ASSEMBLY__

.macro	SAVE_ALL
	mfcr	r30, cr0
	mv	r31, r0
	nop
	/* if UMs == 1, change stack. */
	slli.c	r30, r30, 28
	bpl	1f
	la	r31, kernelsp
	lw	r31, [r31]
1:
	mv	r30, r0
	addri	r0, r31, -PT_SIZE

	sw	r30, [r0, PT_R0]
	.set	r1
	sw	r1, [r0, PT_R1]
	.set	nor1
	sw	r2, [r0, PT_R2]
	sw	r3, [r0, PT_R3]
	sw	r4, [r0, PT_R4]
	sw	r5, [r0, PT_R5]
	sw	r6, [r0, PT_R6]
	sw	r7, [r0, PT_R7]

	sw	r8, [r0, PT_R8]
	sw	r9, [r0, PT_R9]
	sw	r10, [r0, PT_R10]
	sw	r11, [r0, PT_R11]
	sw	r12, [r0, PT_R12]
	sw	r13, [r0, PT_R13]
	sw	r14, [r0, PT_R14]
	sw	r15, [r0, PT_R15]

	sw	r16, [r0, PT_R16]
	sw	r17, [r0, PT_R17]
	sw	r18, [r0, PT_R18]
	sw	r19, [r0, PT_R19]
	sw	r20, [r0, PT_R20]
	sw	r21, [r0, PT_R21]
	sw	r22, [r0, PT_R22]
	sw	r23, [r0, PT_R23]

	sw	r24, [r0, PT_R24]
	sw	r25, [r0, PT_R25]
	sw	r25, [r0, PT_R25]
	sw	r26, [r0, PT_R26]
	sw	r27, [r0, PT_R27]

	sw	r28, [r0, PT_R28]
	sw	r29, [r0, PT_R29]
	orri	r28, r0, 0x1fff
	li	r31, 0x00001fff
	xor	r28, r28, r31

	mfcehl	r30, r31
	sw	r30, [r0, PT_CEH]
	sw	r31, [r0, PT_CEL]

	mfcr	r31, cr0
	sw	r31, [r0, PT_PSR]

	mfcr	r31, cr1
	sw	r31, [r0, PT_CONDITION]

	mfcr	r31, cr2
	sw	r31, [r0, PT_ECR]
	
	mfcr	r31, cr5
	srli	r31, r31, 1
	slli	r31, r31, 1
	sw	r31, [r0, PT_EPC]
.endm

#endif /* __ASSEMBLY__ */
#endif /* _ASM_SCORE_ASMMACRO_H */
