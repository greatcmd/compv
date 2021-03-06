#########################################################################
# Copyright (C) 2016-2018 Doubango Telecom <https://www.doubango.org>   #
# File author: Mamadou DIOP (Doubango Telecom, France).                 #
# License: GPLv3. For commercial license please contact us.             #
# Source code: https://github.com/DoubangoTelecom/compv                 #
# WebSite: http://compv.org                                             #
#########################################################################
#if defined(__arm__) && !defined(__aarch64__)
.include "compv_common_arm32.S"

.data

.extern
 
.text


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ arg(0) -> COMPV_ALIGNED(NEON) const compv_float64_t* x
@ arg(1) -> COMPV_ALIGNED(NEON) const compv_float64_t* y
@ arg(2) -> compv_uscalar_t numPoints
@ arg(3) -> compv_float64_t* tx1
@ arg(4) -> compv_float64_t* ty1
@ arg(5) -> compv_float64_t* sf1
COMPV_GAS_FUNCTION_DECLARE CompVMathStatsNormalize2DHartley_64f_Asm_NEON32
    COMPV_GAS_FUNCTION_PROLOG
    COMPV_GAS_SHADOW_ARGS_TO_STACK 6
	COMPV_GAS_SAVE_NEON_REGS

	@@ Load arguments @@
    ldm_args r0-r5
	x .req r0
	y .req r1
	numPoints .req r2
	tx1 .req r3
    ty1 .req r4
    sf1 .req r5

    i .req r6

    vecSqrt2 .req q13y
    vecTx .req q14x
    vecTy .req q14y
    vecMagnitude .req q15x
    vecOneOverNumPoints .req q15y

    veor vecTx, vecTx, vecTx
    veor vecTy, vecTy, vecTy
    veor vecMagnitude, vecMagnitude, vecMagnitude

    vmov s11, numPoints
    vcvt.f64.u32 q0x, s11
    vmov.f64 vecOneOverNumPoints, #1.000000e+00
    vmov.f64 vecSqrt2, #2.000000e+00
    vdiv.f64 vecOneOverNumPoints, vecOneOverNumPoints, q0x
    vsqrt.f64 vecSqrt2, vecSqrt2
    
    
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ for (i = 0; i < numPoints_ - 7; i += 8)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    lsrs i, numPoints, #3
    beq EndOf_LoopTxTy8_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32
    LoopTxTy8_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32:
        subs i, i, #1
        vld1.64 { q0, q1 }, [x :128]!
        vld1.64 { q2, q3 }, [x :128]!
        vadd.f64 q0x, q0x, q0y
        vadd.f64 q1x, q1x, q1y
        vadd.f64 q2x, q2x, q2y
        vadd.f64 q3x, q3x, q3y
        vld1.64 { q4, q5 }, [y :128]!
        vld1.64 { q6, q7 }, [y :128]!
        vadd.f64 q4x, q4x, q4y
        vadd.f64 q5x, q5x, q5y
        vadd.f64 q6x, q6x, q6y
        vadd.f64 q7x, q7x, q7y
        vadd.f64 q0x, q0x, q1x
        vadd.f64 q2x, q2x, q3x
        vadd.f64 q4x, q4x, q5x
        vadd.f64 q6x, q6x, q7x
        vadd.f64 q0x, q0x, q2x
        vadd.f64 q4x, q4x, q6x
        vadd.f64 vecTx, vecTx, q0x
        vadd.f64 vecTy, vecTy, q4x
        bne LoopTxTy8_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32
        EndOf_LoopTxTy8_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32:
        @@ EndOf_LoopTxTy8_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32 @@

    and r8, numPoints, #7 @ modulo 8
    and r9, numPoints, #3 @ modulo 4

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ .if (i < numPoints_ - 3)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    lsrs i, r8, #2
    beq EndOf_LoopTxTy4_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32
    LoopTxTy4_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32:
        vld1.64 { q0, q1 }, [x :128]!
        vadd.f64 q0x, q0x, q0y
        vadd.f64 q1x, q1x, q1y
        vld1.64 { q4, q5 }, [y :128]!
        vadd.f64 q4x, q4x, q4y
        vadd.f64 q5x, q5x, q5y
        vadd.f64 q0x, q0x, q1x
        vadd.f64 q4x, q4x, q5x
        vadd.f64 vecTx, vecTx, q0x
        vadd.f64 vecTy, vecTy, q4x
        EndOf_LoopTxTy4_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32:
        @@ EndOf_LoopTxTy4_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32 @@

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ .if (i < numPoints_ - 1)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    lsrs i, r9, #1
    beq EndOf_LoopTxTy2_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32
    LoopTxTy2_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32:
        vld1.64 { q0 }, [x :128]!
        vadd.f64 q0x, q0x, q0y
        vld1.64 { q4 }, [y :128]!
        vadd.f64 q4x, q4x, q4y
        vadd.f64 vecTx, vecTx, q0x
        vadd.f64 vecTy, vecTy, q4x
        EndOf_LoopTxTy2_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32:
        @@ EndOf_LoopTxTy2_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32 @@

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ .if (numPoints_ & 1)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    ands i, numPoints, #1
    beq EndOf_LoopTxTy1_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32
    LoopTxTy1_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32:
        vld1.64 { q0x }, [x :64]
        vld1.64 { q4x }, [y :64]
        vadd.f64 vecTx, vecTx, q0x
        vadd.f64 vecTy, vecTy, q4x
        EndOf_LoopTxTy1_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32:
        @@ EndOf_LoopTxTy1_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32 @@


    vmul.f64 vecTx, vecTx, vecOneOverNumPoints
    vmul.f64 vecTy, vecTy, vecOneOverNumPoints

    @ restore x and y @
    ldr_arg 0, x
    ldr_arg 1, y

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ for (i = 0; i < numPoints_ - 3; i += 4)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    lsrs i, numPoints, #2
    beq EndOf_LoopMagnitude4_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32
    LoopMagnitude4_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32:
        subs i, i, #1
        vld1.64 { q0, q1 }, [x :128]!
        vld1.64 { q4, q5 }, [y :128]!
        vsub.f64 q0x, q0x, vecTx
        vsub.f64 q4x, q4x, vecTy
        vsub.f64 q0y, q0y, vecTx
        vsub.f64 q4y, q4y, vecTy
        vsub.f64 q1x, q1x, vecTx
        vsub.f64 q5x, q5x, vecTy
        vsub.f64 q1y, q1y, vecTx
        vsub.f64 q5y, q5y, vecTy
        vmul.f64 q0x, q0x, q0x
        vmul.f64 q4x, q4x, q4x
        vmul.f64 q0y, q0y, q0y
        vmul.f64 q4y, q4y, q4y
        vmul.f64 q1x, q1x, q1x
        vmul.f64 q5x, q5x, q5x
        vmul.f64 q1y, q1y, q1y
        vmul.f64 q5y, q5y, q5y
        vadd.f64 q0x, q0x, q4x
        vadd.f64 q0y, q0y, q4y
        vadd.f64 q1x, q1x, q5x
        vadd.f64 q1y, q1y, q5y
        vsqrt.f64 q0x, q0x
        vsqrt.f64 q0y, q0y
        vsqrt.f64 q1x, q1x
        vsqrt.f64 q1y, q1y
        vadd.f64 q0x, q0x, q0y
        vadd.f64 q1x, q1x, q1y
        vadd.f64 vecMagnitude, vecMagnitude, q0x
        vadd.f64 vecMagnitude, vecMagnitude, q1x
        bne LoopMagnitude4_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32
        EndOf_LoopMagnitude4_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32:
        @@ EndOf_LoopMagnitude4_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32 @@


    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ .if (i < numPoints_ - 1)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    lsrs i, r9, #1
    beq EndOf_LoopMagnitude2_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32
    LoopMagnitude2_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32:
        vld1.64 { q0 }, [x :128]!
        vld1.64 { q4 }, [y :128]!
        vsub.f64 q0x, q0x, vecTx
        vsub.f64 q4x, q4x, vecTy
        vsub.f64 q0y, q0y, vecTx
        vsub.f64 q4y, q4y, vecTy
        vmul.f64 q0x, q0x, q0x
        vmul.f64 q4x, q4x, q4x
        vmul.f64 q0y, q0y, q0y
        vmul.f64 q4y, q4y, q4y
        vadd.f64 q0x, q0x, q4x
        vadd.f64 q0y, q0y, q4y
        vsqrt.f64 q0x, q0x
        vsqrt.f64 q0y, q0y
        vadd.f64 vecMagnitude, vecMagnitude, q0x
        vadd.f64 vecMagnitude, vecMagnitude, q0y
        EndOf_LoopMagnitude2_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32:
        @@ EndOf_LoopMagnitude2_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32 @@


    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ .if (numPoints_ & 1)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    ands i, numPoints, #1
    beq EndOf_LoopMagnitude1_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32
    LoopMagnitude1_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32:
        vld1.64 { q0x }, [x :64]
        vld1.64 { q4x }, [y :64]
        vsub.f64 q0x, q0x, vecTx
        vsub.f64 q4x, q4x, vecTy
        vmul.f64 q0x, q0x, q0x
        vmul.f64 q4x, q4x, q4x
        vadd.f64 q0x, q0x, q4x
        vsqrt.f64 q0x, q0x
        vadd.f64 vecMagnitude, vecMagnitude, q0x
        EndOf_LoopMagnitude1_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32:
        @@ EndOf_LoopMagnitude1_CompVMathStatsNormalize2DHartley_64f_Asm_NEON32 @@

    vmul.f64 vecMagnitude, vecMagnitude, vecOneOverNumPoints
    vdiv.f64 vecMagnitude, vecSqrt2, vecMagnitude
    
    vst1.64 { vecTx }, [tx1 :64]
    vst1.64 { vecTy }, [ty1 :64]
    vst1.64 { vecMagnitude }, [sf1 :64]
    
    .unreq x
	.unreq y
	.unreq numPoints
	.unreq tx1
    .unreq ty1
    .unreq sf1

    .unreq i

    .unreq vecSqrt2
    .unreq vecTx
    .unreq vecTy
    .unreq vecMagnitude
    .unreq vecOneOverNumPoints

    COMPV_GAS_RESTORE_NEON_REGS
    COMPV_GAS_UNSHADOW_ARGS 6
	COMPV_GAS_FUNCTION_EPILOG
	COMPV_GAS_FUNCTION_RETURN


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ arg(0) -> COMPV_ALIGNED(NEON) const compv_float64_t* x
@ arg(1) -> COMPV_ALIGNED(NEON) const compv_float64_t* y
@ arg(2) -> compv_uscalar_t numPoints
@ arg(3) -> compv_float64_t* tx1
@ arg(4) -> compv_float64_t* ty1
@ arg(5) -> compv_float64_t* sf1
COMPV_GAS_FUNCTION_DECLARE CompVMathStatsNormalize2DHartley_4_64f_Asm_NEON32
    COMPV_GAS_FUNCTION_PROLOG
    COMPV_GAS_SHADOW_ARGS_TO_STACK 6
	COMPV_GAS_SAVE_NEON_REGS

	@@ Load arguments @@
    ldm_args r0-r5
	x .req r0
	y .req r1
	numPoints .req r2
	tx1 .req r3
    ty1 .req r4
    sf1 .req r5

    i .req r6

    vecSqrt2 .req q13y
    vecTx .req q14x
    vecTy .req q14y
    vecMagnitude .req q15x
    vecOneOverNumPoints .req q15y

    vmov s11, numPoints
    vcvt.f64.u32 q0x, s11
    vmov.f64 vecOneOverNumPoints, #1.000000e+00
    vmov.f64 vecSqrt2, #2.000000e+00
    vdiv.f64 vecOneOverNumPoints, vecOneOverNumPoints, q0x
    vsqrt.f64 vecSqrt2, vecSqrt2
    vld1.64 { q0, q1 }, [x :128]!
    vadd.f64 q0x, q0x, q0y
    vadd.f64 q1x, q1x, q1y
    vld1.64 { q4, q5 }, [y :128]!
    vadd.f64 q4x, q4x, q4y
    vadd.f64 q5x, q5x, q5y
    vadd.f64 vecTx, q0x, q1x
    vadd.f64 vecTy, q4x, q5x
    vmul.f64 vecTx, vecTx, vecOneOverNumPoints
    vmul.f64 vecTy, vecTy, vecOneOverNumPoints
    ldr_arg 0, x
    ldr_arg 1, y    
    vld1.64 { q0, q1 }, [x :128]!
    vld1.64 { q4, q5 }, [y :128]!
    vsub.f64 q0x, q0x, vecTx
    vsub.f64 q4x, q4x, vecTy
    vsub.f64 q0y, q0y, vecTx
    vsub.f64 q4y, q4y, vecTy
    vsub.f64 q1x, q1x, vecTx
    vsub.f64 q5x, q5x, vecTy
    vsub.f64 q1y, q1y, vecTx
    vsub.f64 q5y, q5y, vecTy
    vmul.f64 q0x, q0x, q0x
    vmul.f64 q4x, q4x, q4x
    vmul.f64 q0y, q0y, q0y
    vmul.f64 q4y, q4y, q4y
    vmul.f64 q1x, q1x, q1x
    vmul.f64 q5x, q5x, q5x
    vmul.f64 q1y, q1y, q1y
    vmul.f64 q5y, q5y, q5y
    vadd.f64 q0x, q0x, q4x
    vadd.f64 q0y, q0y, q4y
    vadd.f64 q1x, q1x, q5x
    vadd.f64 q1y, q1y, q5y
    vsqrt.f64 q0x, q0x
    vsqrt.f64 q0y, q0y
    vsqrt.f64 q1x, q1x
    vsqrt.f64 q1y, q1y
    vadd.f64 q0x, q0x, q0y
    vadd.f64 q1x, q1x, q1y
    vadd.f64 vecMagnitude, q0x, q1x
    vmul.f64 vecMagnitude, vecMagnitude, vecOneOverNumPoints
    vst1.64 { vecTx }, [tx1 :64]
    vst1.64 { vecTy }, [ty1 :64]
    vdiv.f64 vecMagnitude, vecSqrt2, vecMagnitude    
    vst1.64 { vecMagnitude }, [sf1 :64]
    
    .unreq x
	.unreq y
	.unreq numPoints
	.unreq tx1
    .unreq ty1
    .unreq sf1

    .unreq i

    .unreq vecSqrt2
    .unreq vecTx
    .unreq vecTy
    .unreq vecMagnitude
    .unreq vecOneOverNumPoints

    COMPV_GAS_RESTORE_NEON_REGS
    COMPV_GAS_UNSHADOW_ARGS 6
	COMPV_GAS_FUNCTION_EPILOG
	COMPV_GAS_FUNCTION_RETURN


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ arg(0) -> COMPV_ALIGNED(NEON) const compv_float64_t* aX_h
@ arg(1) -> COMPV_ALIGNED(NEON) const compv_float64_t* aY_h
@ arg(2) -> COMPV_ALIGNED(NEON) const compv_float64_t* aZ_h
@ arg(3) -> COMPV_ALIGNED(NEON) const compv_float64_t* bX
@ arg(4) -> COMPV_ALIGNED(NEON) const compv_float64_t* bY
@ arg(5) -> COMPV_ALIGNED(NEON) compv_float64_t* mse
@ arg(6) -> compv_uscalar_t numPoints
COMPV_GAS_FUNCTION_DECLARE CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32
    COMPV_GAS_FUNCTION_PROLOG
    COMPV_GAS_SHADOW_ARGS_TO_STACK 7
	COMPV_GAS_SAVE_NEON_REGS

    @@ Load arguments @@
    ldm_args r0-r6
	aX_h .req r0
	aY_h .req r1
	aZ_h .req r2
	bX_ .req r3 @ bX is reserved name
    bY .req r4
    mse .req r5
    numPoints .req r6

    i .req r7

    vecOne .req q15y

    vmov.f64 vecOne, #1.000000e+00

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ for (i = 0; i < numPoints_ - 7; i += 8)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    lsrs i, numPoints, #3
    beq EndOf_LoopNumPoints8_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32
    LoopNumPoints8_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32:
        subs i, i, #1
        vld1.64 { q0, q1 }, [aZ_h :128]!
        vdiv.f64 q0x, vecOne, q0x
        vdiv.f64 q0y, vecOne, q0y
        vdiv.f64 q1x, vecOne, q1x
        vdiv.f64 q1y, vecOne, q1y
        vld1.64 { q2, q3 }, [aZ_h :128]!
        vdiv.f64 q2x, vecOne, q2x
        vdiv.f64 q2y, vecOne, q2y
        vdiv.f64 q3x, vecOne, q3x
        vdiv.f64 q3y, vecOne, q3y
        vld1.64 { q4, q5 }, [aX_h :128]!
        vld1.64 { q6, q7 }, [aY_h :128]!
        vmul.f64 q4x, q4x, q0x
        vmul.f64 q4y, q4y, q0y
        vmul.f64 q5x, q5x, q1x
        vmul.f64 q5y, q5y, q1y
        vmul.f64 q6x, q6x, q0x
        vmul.f64 q6y, q6y, q0y
        vmul.f64 q7x, q7x, q1x
        vmul.f64 q7y, q7y, q1y
        vld1.64 { q8, q9 }, [aX_h :128]!
        vld1.64 { q10, q11 }, [aY_h :128]!
        vmul.f64 q8x, q8x, q2x
        vmul.f64 q8y, q8y, q2y
        vmul.f64 q9x, q9x, q3x
        vmul.f64 q9y, q9y, q3y
        vmul.f64 q10x, q10x, q2x
        vmul.f64 q10y, q10y, q2y
        vmul.f64 q11x, q11x, q3x
        vmul.f64 q11y, q11y, q3y
        vld1.64 { q0, q1 }, [bX_ :128]!
        vld1.64 { q2, q3 }, [bY :128]!
        vsub.f64 q4x, q4x, q0x
        vsub.f64 q4y, q4y, q0y
        vsub.f64 q5x, q5x, q1x
        vsub.f64 q5y, q5y, q1y
        vsub.f64 q6x, q6x, q2x
        vsub.f64 q6y, q6y, q2y
        vsub.f64 q7x, q7x, q3x
        vsub.f64 q7y, q7y, q3y
        vld1.64 { q0, q1 }, [bX_ :128]!
        vld1.64 { q2, q3 }, [bY :128]!
        vsub.f64 q8x, q8x, q0x
        vsub.f64 q8y, q8y, q0y
        vsub.f64 q9x, q9x, q1x
        vsub.f64 q9y, q9y, q1y
        vsub.f64 q10x, q10x, q2x
        vsub.f64 q10y, q10y, q2y
        vsub.f64 q11x, q11x, q3x
        vsub.f64 q11y, q11y, q3y
        vmul.f64 q4x, q4x, q4x
        vmul.f64 q6x, q6x, q6x
        vmul.f64 q4y, q4y, q4y
        vmul.f64 q6y, q6y, q6y
        vmul.f64 q5x, q5x, q5x
        vmul.f64 q7x, q7x, q7x
        vmul.f64 q5y, q5y, q5y
        vmul.f64 q7y, q7y, q7y
        vmul.f64 q8x, q8x, q8x
        vmul.f64 q10x, q10x, q10x
        vmul.f64 q8y, q8y, q8y
        vmul.f64 q10y, q10y, q10y
        vmul.f64 q9x, q9x, q9x
        vmul.f64 q11x, q11x, q11x
        vmul.f64 q9y, q9y, q9y
        vmul.f64 q11y, q11y, q11y
        vadd.f64 q4x, q4x, q6x
        vadd.f64 q4y, q4y, q6y
        vadd.f64 q5x, q5x, q7x
        vadd.f64 q5y, q5y, q7y
        vadd.f64 q8x, q8x, q10x
        vadd.f64 q8y, q8y, q10y
        vadd.f64 q9x, q9x, q11x
        vadd.f64 q9y, q9y, q11y
        vst1.64 { q4, q5 }, [mse :128]!
        vst1.64 { q8, q9 }, [mse :128]!
        bne LoopNumPoints8_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32
        EndOf_LoopNumPoints8_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32:
        @@ EndOf_LoopNumPoints8_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32 @@

    and r8, numPoints, #7 @ modulo 8
    and r9, numPoints, #3 @ modulo 4

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ .if (i < numPoints_ - 3)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    lsrs i, r8, #2
    beq EndOf_LoopNumPoints4_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32
    LoopNumPoints4_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32:
        vld1.64 { q0, q1 }, [aZ_h :128]!
        vdiv.f64 q0x, vecOne, q0x
        vdiv.f64 q0y, vecOne, q0y
        vdiv.f64 q1x, vecOne, q1x
        vdiv.f64 q1y, vecOne, q1y
        vld1.64 { q4, q5 }, [aX_h :128]!
        vld1.64 { q6, q7 }, [aY_h :128]!
        vmul.f64 q4x, q4x, q0x
        vmul.f64 q4y, q4y, q0y
        vmul.f64 q5x, q5x, q1x
        vmul.f64 q5y, q5y, q1y
        vmul.f64 q6x, q6x, q0x
        vmul.f64 q6y, q6y, q0y
        vmul.f64 q7x, q7x, q1x
        vmul.f64 q7y, q7y, q1y
        vld1.64 { q0, q1 }, [bX_ :128]!
        vld1.64 { q2, q3 }, [bY :128]!
        vsub.f64 q4x, q4x, q0x
        vsub.f64 q4y, q4y, q0y
        vsub.f64 q5x, q5x, q1x
        vsub.f64 q5y, q5y, q1y
        vsub.f64 q6x, q6x, q2x
        vsub.f64 q6y, q6y, q2y
        vsub.f64 q7x, q7x, q3x
        vsub.f64 q7y, q7y, q3y
        vmul.f64 q4x, q4x, q4x
        vmul.f64 q6x, q6x, q6x
        vmul.f64 q4y, q4y, q4y
        vmul.f64 q6y, q6y, q6y
        vmul.f64 q5x, q5x, q5x
        vmul.f64 q7x, q7x, q7x
        vmul.f64 q5y, q5y, q5y
        vmul.f64 q7y, q7y, q7y
        vadd.f64 q4x, q4x, q6x
        vadd.f64 q4y, q4y, q6y
        vadd.f64 q5x, q5x, q7x
        vadd.f64 q5y, q5y, q7y
        vst1.64 { q4, q5 }, [mse :128]!
        EndOf_LoopNumPoints4_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32:
        @@ EndOf_LoopNumPoints4_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32 @@


    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ .if (i < numPoints_ - 1)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    lsrs i, r9, #1
    beq EndOf_LoopNumPoints2_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32
    LoopNumPoints2_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32:
        vld1.64 { q0 }, [aZ_h :128]!
        vdiv.f64 q0x, vecOne, q0x
        vdiv.f64 q0y, vecOne, q0y
        vld1.64 { q4 }, [aX_h :128]!
        vld1.64 { q6 }, [aY_h :128]!
        vmul.f64 q4x, q4x, q0x
        vmul.f64 q4y, q4y, q0y
        vmul.f64 q6x, q6x, q0x
        vmul.f64 q6y, q6y, q0y
        vld1.64 { q0 }, [bX_ :128]!
        vld1.64 { q2 }, [bY :128]!
        vsub.f64 q4x, q4x, q0x
        vsub.f64 q4y, q4y, q0y
        vsub.f64 q6x, q6x, q2x
        vsub.f64 q6y, q6y, q2y
        vmul.f64 q4x, q4x, q4x
        vmul.f64 q6x, q6x, q6x
        vmul.f64 q4y, q4y, q4y
        vmul.f64 q6y, q6y, q6y
        vadd.f64 q4x, q4x, q6x
        vadd.f64 q4y, q4y, q6y
        vst1.64 { q4 }, [mse :128]!
        EndOf_LoopNumPoints2_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32:
        @@ EndOf_LoopNumPoints2_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32 @@


    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ .if (numPoints_ & 1)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    ands i, numPoints, #1
    beq EndOf_LoopNumPoints1_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32
    LoopNumPoints1_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32:
        vld1.64 { q0x }, [aZ_h :64]
        vdiv.f64 q0x, vecOne, q0x
        vld1.64 { q4x }, [aX_h :64]
        vld1.64 { q6x }, [aY_h :64]
        vmul.f64 q4x, q4x, q0x
        vmul.f64 q6x, q6x, q0x
        vld1.64 { q0x }, [bX_ :64]
        vld1.64 { q2x }, [bY :64]
        vsub.f64 q4x, q4x, q0x
        vsub.f64 q6x, q6x, q2x
        vmul.f64 q4x, q4x, q4x
        vmul.f64 q6x, q6x, q6x
        vadd.f64 q4x, q4x, q6x
        vst1.64 { q4x }, [mse :64]
        EndOf_LoopNumPoints1_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32:
        @@ EndOf_LoopNumPoints1_CompVMathStatsMSE2DHomogeneous_64f_Asm_NEON32 @@


    .unreq aX_h
	.unreq aY_h
	.unreq aZ_h
	.unreq bX_
    .unreq bY
    .unreq mse
    .unreq numPoints

    .unreq i

    .unreq vecOne

    COMPV_GAS_RESTORE_NEON_REGS
    COMPV_GAS_UNSHADOW_ARGS 7
	COMPV_GAS_FUNCTION_EPILOG
	COMPV_GAS_FUNCTION_RETURN



@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ arg(0) -> COMPV_ALIGNED(NEON) const compv_float64_t* aX_h
@ arg(1) -> COMPV_ALIGNED(NEON) const compv_float64_t* aY_h
@ arg(2) -> COMPV_ALIGNED(NEON) const compv_float64_t* aZ_h
@ arg(3) -> COMPV_ALIGNED(NEON) const compv_float64_t* bX
@ arg(4) -> COMPV_ALIGNED(NEON) const compv_float64_t* bY
@ arg(5) -> COMPV_ALIGNED(NEON) compv_float64_t* mse
@ arg(6) -> compv_uscalar_t numPoints
COMPV_GAS_FUNCTION_DECLARE CompVMathStatsMSE2DHomogeneous_4_64f_Asm_NEON32
    COMPV_GAS_FUNCTION_PROLOG
    COMPV_GAS_SHADOW_ARGS_TO_STACK 7
	COMPV_GAS_SAVE_NEON_REGS

    @@ Load arguments @@
    ldm_args r0-r6
	aX_h .req r0
	aY_h .req r1
	aZ_h .req r2
	bX_ .req r3 @ bX is reserved name
    bY .req r4
    mse .req r5
    numPoints .req r6

    i .req r7

    vecOne .req q15y

    vmov.f64 vecOne, #1.000000e+00
    
    vld1.64 { q0, q1 }, [aZ_h :128]
    vdiv.f64 q0x, vecOne, q0x
    vdiv.f64 q0y, vecOne, q0y
    vdiv.f64 q1x, vecOne, q1x
    vdiv.f64 q1y, vecOne, q1y
    vld1.64 { q4, q5 }, [aX_h :128]
    vld1.64 { q6, q7 }, [aY_h :128]
    vmul.f64 q4x, q4x, q0x
    vmul.f64 q4y, q4y, q0y
    vmul.f64 q5x, q5x, q1x
    vmul.f64 q5y, q5y, q1y
    vmul.f64 q6x, q6x, q0x
    vmul.f64 q6y, q6y, q0y
    vmul.f64 q7x, q7x, q1x
    vmul.f64 q7y, q7y, q1y
    vld1.64 { q0, q1 }, [bX_ :128]
    vld1.64 { q2, q3 }, [bY :128]
    vsub.f64 q4x, q4x, q0x
    vsub.f64 q4y, q4y, q0y
    vsub.f64 q5x, q5x, q1x
    vsub.f64 q5y, q5y, q1y
    vsub.f64 q6x, q6x, q2x
    vsub.f64 q6y, q6y, q2y
    vsub.f64 q7x, q7x, q3x
    vsub.f64 q7y, q7y, q3y
    vmul.f64 q4x, q4x, q4x
    vmul.f64 q6x, q6x, q6x
    vmul.f64 q4y, q4y, q4y
    vmul.f64 q6y, q6y, q6y
    vmul.f64 q5x, q5x, q5x
    vmul.f64 q7x, q7x, q7x
    vmul.f64 q5y, q5y, q5y
    vmul.f64 q7y, q7y, q7y
    vadd.f64 q4x, q4x, q6x
    vadd.f64 q4y, q4y, q6y
    vadd.f64 q5x, q5x, q7x
    vadd.f64 q5y, q5y, q7y
    vst1.64 { q4, q5 }, [mse :128]

    .unreq aX_h
	.unreq aY_h
	.unreq aZ_h
	.unreq bX_
    .unreq bY
    .unreq mse
    .unreq numPoints

    .unreq i

    .unreq vecOne

    COMPV_GAS_RESTORE_NEON_REGS
    COMPV_GAS_UNSHADOW_ARGS 7
	COMPV_GAS_FUNCTION_EPILOG
	COMPV_GAS_FUNCTION_RETURN


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ arg(0) -> COMPV_ALIGNED(NEON) const compv_float64_t* data
@ arg(1) -> compv_uscalar_t count
@ arg(2) -> const compv_float64_t* mean1
@ arg(3) -> compv_float64_t* var1
COMPV_GAS_FUNCTION_DECLARE CompVMathStatsVariance_64f_Asm_NEON32
    COMPV_GAS_FUNCTION_PROLOG
	COMPV_GAS_SAVE_NEON_REGS

    @@ Load arguments @@
	data .req r0
	count .req r1
	mean1 .req r2
	var1 .req r3

    i .req r4

    vecMean .req q14y
    vecVar .req q15x
    vecCountMinus1 .req q15y

    veor vecVar, vecVar, vecVar
    sub r10, count, #1
    vmov s11, r10
    vcvt.f64.u32 vecCountMinus1, s11
    vld1.64 { vecMean }, [mean1]

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ for (i = 0; i < count - 7; i += 8)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    lsrs i, count, #3
    beq EndOf_LoopNumPoints8_CompVMathStatsVariance_64f_Asm_NEON32
    LoopNumPoints8_CompVMathStatsVariance_64f_Asm_NEON32:
        subs i, i, #1
        vld1.64 { q0, q1 }, [data :128]!
        vld1.64 { q2, q3 }, [data :128]!
        vsub.f64 q0x, q0x, vecMean
        vsub.f64 q0y, q0y, vecMean
        vsub.f64 q1x, q1x, vecMean
        vsub.f64 q1y, q1y, vecMean
        vsub.f64 q2x, q2x, vecMean
        vsub.f64 q2y, q2y, vecMean
        vsub.f64 q3x, q3x, vecMean
        vsub.f64 q3y, q3y, vecMean
        vmul.f64 q0x, q0x, q0x
        vmul.f64 q0y, q0y, q0y
        vmul.f64 q1x, q1x, q1x
        vmul.f64 q1y, q1y, q1y
        vmul.f64 q2x, q2x, q2x
        vmul.f64 q2y, q2y, q2y
        vmul.f64 q3x, q3x, q3x
        vmul.f64 q3y, q3y, q3y
        vadd.f64 q0x, q0x, q0y
        vadd.f64 q1x, q1x, q1y
        vadd.f64 q2x, q2x, q2y
        vadd.f64 q3x, q3x, q3y
        vadd.f64 q0x, q0x, q1x
        vadd.f64 q2x, q2x, q3x
        vadd.f64 vecVar, vecVar, q0x
        vadd.f64 vecVar, vecVar, q2x
        bne LoopNumPoints8_CompVMathStatsVariance_64f_Asm_NEON32
        EndOf_LoopNumPoints8_CompVMathStatsVariance_64f_Asm_NEON32:
        @@ EndOf_LoopNumPoints8_CompVMathStatsVariance_64f_Asm_NEON32 @@

    and r8, count, #7 @ modulo 8
    and r9, count, #3 @ modulo 4

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ .if (i < count - 3)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    lsrs i, r8, #2
    beq EndOf_LoopNumPoints4_CompVMathStatsVariance_64f_Asm_NEON32
    LoopNumPoints4_CompVMathStatsVariance_64f_Asm_NEON32:
        vld1.64 { q0, q1 }, [data :128]!
        vsub.f64 q0x, q0x, vecMean
        vsub.f64 q0y, q0y, vecMean
        vsub.f64 q1x, q1x, vecMean
        vsub.f64 q1y, q1y, vecMean
        vmul.f64 q0x, q0x, q0x
        vmul.f64 q0y, q0y, q0y
        vmul.f64 q1x, q1x, q1x
        vmul.f64 q1y, q1y, q1y
        vadd.f64 q0x, q0x, q0y
        vadd.f64 q1x, q1x, q1y
        vadd.f64 vecVar, vecVar, q0x
        vadd.f64 vecVar, vecVar, q1x
        EndOf_LoopNumPoints4_CompVMathStatsVariance_64f_Asm_NEON32:
        @@ EndOf_LoopNumPoints4_CompVMathStatsVariance_64f_Asm_NEON32 @@


    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ .if (i < count - 1)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    lsrs i, r9, #1
    beq EndOf_LoopNumPoints2_CompVMathStatsVariance_64f_Asm_NEON32
    LoopNumPoints2_CompVMathStatsVariance_64f_Asm_NEON32:
        vld1.64 { q0 }, [data :128]!
        vsub.f64 q0x, q0x, vecMean
        vsub.f64 q0y, q0y, vecMean
        vmul.f64 q0x, q0x, q0x
        vmul.f64 q0y, q0y, q0y
        vadd.f64 vecVar, vecVar, q0x
        vadd.f64 vecVar, vecVar, q0y
        EndOf_LoopNumPoints2_CompVMathStatsVariance_64f_Asm_NEON32:
        @@ EndOf_LoopNumPoints2_CompVMathStatsVariance_64f_Asm_NEON32 @@


    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ .if (count & 1)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    ands i, count, #1
    beq EndOf_LoopNumPoints1_CompVMathStatsVariance_64f_Asm_NEON32
    LoopNumPoints1_CompVMathStatsVariance_64f_Asm_NEON32:
        vld1.64 { q0x }, [data :64]
        vsub.f64 q0x, q0x, vecMean
        vmul.f64 q0x, q0x, q0x
        vadd.f64 vecVar, vecVar, q0x
        EndOf_LoopNumPoints1_CompVMathStatsVariance_64f_Asm_NEON32:
        @@ EndOf_LoopNumPoints1_CompVMathStatsVariance_64f_Asm_NEON32 @@

    vdiv.f64 vecVar, vecVar, vecCountMinus1
    vst1.64 { vecVar }, [var1]

    .unreq data
	.unreq count
	.unreq mean1
	.unreq var1

    .unreq i

    .unreq vecMean
    .unreq vecVar
    .unreq vecCountMinus1

    COMPV_GAS_RESTORE_NEON_REGS
	COMPV_GAS_FUNCTION_EPILOG
	COMPV_GAS_FUNCTION_RETURN



#endif /* defined(__arm__) && !defined(__aarch64__) */
