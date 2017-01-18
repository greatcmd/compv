;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) 2016-2017 Doubango Telecom <https://www.doubango.org>   ;
; File author: Mamadou DIOP (Doubango Telecom, France).                 ;
; License: GPLv3. For commercial license please contact us.             ;
; Source code: https://github.com/DoubangoTelecom/compv                 ;
; WebSite: http://compv.org                                             ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%include "compv_common_x86.s"
%if COMPV_YASM_ABI_IS_64BIT
%include "compv_image_scale_bilinear_macros.s"

COMPV_YASM_DEFAULT_REL

global sym(CompVImageScaleBilinear_Asm_X64_SSE41)

section .data
	extern sym(kShuffleEpi8_Deinterleave_i32)

section .text

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; arg(0) -> const uint8_t* inPtr
; arg(1) -> compv_uscalar_t inWidth
; arg(2) -> compv_uscalar_t inHeight
; arg(3) -> compv_uscalar_t inStride,
; arg(4) -> COMPV_ALIGNED(SSE) uint8_t* outPtr
; arg(5) -> compv_uscalar_t outWidth
; arg(6) -> compv_uscalar_t outYStart
; arg(7) -> compv_uscalar_t outYEnd
; arg(8) -> COMPV_ALIGNED(SSE) compv_uscalar_t outStride
; arg(9) -> compv_uscalar_t sf_x
; arg(10) -> compv_uscalar_t sf_y
sym(CompVImageScaleBilinear_Asm_X64_SSE41)
	push rbp
	mov rbp, rsp
	COMPV_YASM_SHADOW_ARGS_TO_STACK 11
	COMPV_YASM_SAVE_XMM 15
	push rsi
	push rdi
	push rbx
	push r12
	push r13
	;push r14
	;push r15
	;; end prolog ;;

	; align stack and alloc memory
	COMPV_YASM_ALIGN_STACK 16, rax
	sub rsp, (10*16)
	%define vecy0                   rsp + 0
	%define vecy1                   vecy0 + 16
	%define vecZero                 vecy1 + 16
	%define vec0xff_epi32           vecZero + 16
	%define vec0xff_epi16           vec0xff_epi32 + 16
	%define vecSfxTimes16           vec0xff_epi16 + 16
	%define vecSFX0                 vecSfxTimes16 + 16
	%define vecSFX1                 vecSFX0 + 16
	%define vecSFX2                 vecSFX1 + 16
	%define vecSFX3                 vecSFX2 + 16

	%define vecNeighb0              xmm4
	%define vecNeighb1              xmm5
	%define vecNeighb2              xmm6
	%define vecNeighb3              xmm7
	%define vecX0                   xmm8
	%define vecX1                   xmm9
	%define vecX2                   xmm10
	%define vecX3                   xmm11
	%define vec4                    xmm12
	%define vec5                    xmm13
	%define vec6                    xmm14
	%define vec7                    xmm15
	%define vecDeinterleave         sym(kShuffleEpi8_Deinterleave_i32)

	%define arg_inPtr               arg(0)
	%define arg_inWidth             arg(1)
	%define arg_inHeight            arg(2)
	%define arg_inStride            arg(3)
	%define arg_outPtr              arg(4)
	%define arg_outWidth            arg(5)
	%define arg_outYStart           arg(6)
	%define arg_outYEnd             arg(7)
	%define arg_outStride           arg(8)
	%define arg_sf_x                arg(9)
	%define arg_sf_y                arg(10)

	%define outWidth                r8
	%define outYStart               r9
	%define outPtr                  r10
	%define outStride               r11
	%define sf_y                    r12
	%define inPtr                   r13

	mov outWidth, arg_outWidth
	mov outYStart, arg_outYStart
	mov outPtr, arg_outPtr
	mov outStride, arg_outStride
	mov sf_y, arg_sf_y
	mov inPtr, arg_inPtr

	; compute vecZero, vec0xff_epi32 and vec0xff_epi16
	pxor xmm0, xmm0
	pcmpeqd xmm1, xmm1
	pcmpeqw xmm2, xmm2
	psrld xmm1, 24
	psrlw xmm2, 8
	movdqa [vecZero], xmm0
	movdqa [vec0xff_epi32], xmm1
	movdqa [vec0xff_epi16], xmm2

	; compute vecSfxTimes16
	mov rax, arg_sf_x
	shl rax, 4
	movd xmm0, rax
	pshufd xmm0, xmm0, 0x0
	movdqa [vecSfxTimes16], xmm0

	; compute vecSFX0, vecSFX1, vecSFX2 and vecSFX3
	mov rsi, arg_sf_x
	xor rax, rax ; sf_x_ * 0
	lea rbx, [rsi * 4] ; sf_x_ * 4
	lea rcx, [rsi * 8] ; sf_x_ * 8
	lea rdx, [rbx + rcx] ; sf_x_ * 12
	mov [vecSFX0 + 0], dword eax
	mov [vecSFX1 + 0], dword ebx
	mov [vecSFX2 + 0], dword ecx
	mov [vecSFX3 + 0], dword edx
	lea rax, [rax + rsi]
	lea rbx, [rbx + rsi]
	lea rcx, [rcx + rsi]
	lea rdx, [rdx + rsi]
	mov [vecSFX0 + 4], dword eax
	mov [vecSFX1 + 4], dword ebx
	mov [vecSFX2 + 4], dword ecx
	mov [vecSFX3 + 4], dword edx
	lea rax, [rax + rsi]
	lea rbx, [rbx + rsi]
	lea rcx, [rcx + rsi]
	lea rdx, [rdx + rsi]
	mov [vecSFX0 + 8], dword eax
	mov [vecSFX1 + 8], dword ebx
	mov [vecSFX2 + 8], dword ecx
	mov [vecSFX3 + 8], dword edx
	lea rax, [rax + rsi]
	lea rbx, [rbx + rsi]
	lea rcx, [rcx + rsi]
	lea rdx, [rdx + rsi]
	mov [vecSFX0 + 12], dword eax
	mov [vecSFX1 + 12], dword ebx
	mov [vecSFX2 + 12], dword ecx
	mov [vecSFX3 + 12], dword edx

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; do
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.DoWhile
		mov rax, outYStart
		movd xmm0, rax
		pshufd xmm0, xmm0, 0x0 ; xmm0 = vecYStart
		shr rax, 8 ; rax = (outYStart >> 8) = nearestY 
		imul rax, arg_inStride ; rax = (nearestY * inStride)
		lea rbx, [inPtr + rax] ; rbx = inPtr_
		movdqa xmm1, [vec0xff_epi32]
		pand xmm0, xmm1 ; xmm0 = vecy0
		psubd xmm1, xmm0 ; xmm1 = vecy1
		packssdw xmm0, xmm0
		packssdw xmm1, xmm1
		movdqa [vecy0], xmm0
		movdqa [vecy1], xmm1
		movdqa vecX0, [vecSFX0]
		movdqa vecX1, [vecSFX1]
		movdqa vecX2, [vecSFX2]
		movdqa vecX3, [vecSFX3]

		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		; for (i = 0; i < outWidth; i += 16)
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		xor rsi, rsi ; rsi = i = 0x0
		.LoopWidth
			;;; set neighbs ;;;
			movdqa xmm0, vecX0
			movdqa xmm1, vecX1
			movdqa xmm2, vecX2
			movdqa xmm3, vecX3
			psrld xmm0, 8
			psrld xmm1, 8
			psrld xmm2, 8
			psrld xmm3, 8
			_mm_bilinear_set_neighbs_x86 xmm0, vecNeighb0, vecNeighb2, 0, 1, rbx ; overrides rdx, rdi, rax and rcx
			_mm_bilinear_set_neighbs_x86 xmm1, vecNeighb0, vecNeighb2, 2, 3, rbx ; overrides rdx, rdi, rax and rcx
			_mm_bilinear_set_neighbs_x86 xmm2, vecNeighb1, vecNeighb3, 0, 1, rbx ; overrides rdx, rdi, rax and rcx
			_mm_bilinear_set_neighbs_x86 xmm3, vecNeighb1, vecNeighb3, 2, 3, rbx ; overrides rdx, rdi, rax and rcx

			;;; Deinterleave neighbs ;;;
			pshufb vecNeighb0, [vecDeinterleave] ; 0,0,0,0,1,1,1,1
			pshufb vecNeighb1, [vecDeinterleave] ; 0,0,0,0,1,1,1,1
			pshufb vecNeighb2, [vecDeinterleave] ; 2,2,2,2,3,3,3,3
			pshufb vecNeighb3, [vecDeinterleave] ; 2,2,2,2,3,3,3,3
			movdqa xmm0, vecNeighb0
			movdqa xmm2, vecNeighb2
			punpcklqdq vecNeighb0, vecNeighb1    ; 0,0,0,0,0,0
			punpckhqdq xmm0, vecNeighb1          ; 1,1,1,1,1,1
			punpcklqdq vecNeighb2, vecNeighb3    ; 2,2,2,2,2,2
			punpckhqdq xmm2, vecNeighb3          ; 3,3,3,3,3,3
			movdqa vecNeighb1, xmm0
			movdqa vecNeighb3, xmm2

			; FIXME(dmi): starting here xmm2 = vecZero

			; compute x0 and x1 (first 8) and convert from epi32 and epi16
			movdqa xmm0, [vec0xff_epi32]
			movdqa xmm3, [vec0xff_epi32]
			movdqa xmm1, [vec0xff_epi16]
			pand xmm0, vecX0
			pand xmm3, vecX1
			packusdw xmm0, xmm3 ; xmm0 = vec0
			psubw xmm1, xmm0 ; xmm1 = vec1
			; compute vec4 = (neighb0 * x1) + (neighb1 * x0) -> 8 epi16
			movdqa vec4, vecNeighb0
			movdqa xmm3, vecNeighb1
			punpcklbw vec4, [vecZero]
			punpcklbw xmm3, [vecZero]
			pmullw vec4, xmm1
			pmullw xmm3, xmm0
			paddusw vec4, xmm3
			; compute vec5 = (neighb2 * x1) + (neighb3 * x0) -> 8 epi16
			movdqa vec5, vecNeighb2
			movdqa xmm3, vecNeighb3
			punpcklbw vec5, [vecZero]
			punpcklbw xmm3, [vecZero]
			pmullw vec5, xmm1
			pmullw xmm3, xmm0
			paddusw vec5, xmm3

			; compute x0 and x1 (second 8) and convert from epi32 and epi16
			movdqa xmm0, [vec0xff_epi32]
			movdqa xmm3, [vec0xff_epi32]
			movdqa xmm1, [vec0xff_epi16]
			pand xmm0, vecX2
			pand xmm3, vecX3
			packusdw xmm0, xmm3 ; xmm0 = vec0
			psubw xmm1, xmm0 ; xmm1 = vec1
			; compute vec6 = (neighb0 * x1) + (neighb1 * x0) -> 8 epi16
			movdqa vec6, vecNeighb0
			movdqa xmm3, vecNeighb1
			punpckhbw vec6, [vecZero]
			punpckhbw xmm3, [vecZero]
			pmullw vec6, xmm1
			pmullw xmm3, xmm0
			paddusw vec6, xmm3
			; compute vec7 = (neighb2 * x1) + (neighb3 * x0) -> #8 epi16
			movdqa vec7, vecNeighb2
			movdqa xmm3, vecNeighb3
			punpckhbw vec7, [vecZero]
			punpckhbw xmm3, [vecZero]
			pmullw vec7, xmm1
			pmullw xmm3, xmm0
			paddusw vec7, xmm3

			; Let''s say:
			;		A = ((neighb0 * x1) + (neighb1 * x0))
			;		B = ((neighb2 * x1) + (neighb3 * x0))
			; Then:
			;		A = vec4, vec6
			;		B = vec5, vec7
			;
			; We cannot use _mm_madd_epi16 to compute C and D because it operates on epi16 while A and B contain epu16 values

			movdqa xmm0, [vecy1]  ; xmm0 = vecy1
			movdqa xmm2, [vecy0]  ; xmm2 = vecy0
			movdqa xmm1, xmm0     ; xmm1 = vecy1
			movdqa xmm3, xmm2     ; xmm3 = vecy0

			; compute C = (y1 * A) >> 16
			pmulhuw xmm0, vec4
			pmulhuw xmm1, vec6

			; compute D = (y0 * B) >> 16
			pmulhuw xmm2, vec5
			pmulhuw xmm3, vec7

			; Compute R = (C + D)
			paddusw xmm0, xmm2
			paddusw xmm1, xmm3

			; Store the result
			packuswb xmm0, xmm1
			movdqa [outPtr + rsi], xmm0


			; move to next indices
			lea rsi, [rsi + 16]
			cmp rsi, outWidth
			paddd vecX0, [vecSfxTimes16]
			paddd vecX1, [vecSfxTimes16]
			paddd vecX2, [vecSfxTimes16]
			paddd vecX3, [vecSfxTimes16]
			
			;;
			jl .LoopWidth
			; end-of-LoopWidth

		;;
		lea outYStart, [outYStart + sf_y]
		cmp outYStart, arg_outYEnd
		lea outPtr, [outPtr + outStride]
		jl .DoWhile
		; end-of-DoWhile


	%undef vec4
	%undef vec5
	%undef vec6
	%undef vec7
	%undef vecy0
	%undef vecy1
	%undef vecZero
	%undef vec0xff_epi32
	%undef vec0xff_epi16
	%undef vecSfxTimes16
	%undef vecSFX0
	%undef vecSFX1
	%undef vecSFX2
	%undef vecSFX3

	%undef vecNeighb0
	%undef vecNeighb1
	%undef vecNeighb2
	%undef vecNeighb3
	%undef vecX0
	%undef vecX1
	%undef vecX2
	%undef vecX3
	%undef vec4                    
	%undef vec5                    
	%undef vec6                    
	%undef vec7                    
	%undef vecDeinterleave

	%undef arg_inPtr
	%undef arg_inWidth
	%undef arg_inHeight
	%undef arg_inStride
	%undef arg_outPtr
	%undef arg_outWidth
	%undef arg_outYStart
	%undef arg_outYEnd
	%undef arg_outStride
	%undef arg_sf_x
	%undef arg_sf_y

	%undef outWidth
	%undef outYStart
	%undef outPtr
	%undef outStride
	%undef sf_y
	%undef inPtr

	; free memory and unalign stack
	add rsp, (10*16)
	COMPV_YASM_UNALIGN_STACK

	;; begin epilog ;;
	;pop r15
	;pop r14
	pop r13
	pop r12
	pop rbx
	pop rdi
	pop rsi
	COMPV_YASM_RESTORE_XMM
	COMPV_YASM_UNSHADOW_ARGS
	mov rsp, rbp
	pop rbp
	ret

%endif ; COMPV_YASM_ABI_IS_64BIT