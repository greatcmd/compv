/* Copyright (C) 2016-2018 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#include "compv/base/math/intrin/x86/compv_math_trig_intrin_sse2.h"

#if COMPV_ARCH_X86 && COMPV_INTRINSIC
#include "compv/base/intrin/x86/compv_intrin_sse.h"
#include "compv/base/compv_simd_globals.h"
#include "compv/base/math/compv_math.h"
#include "compv/base/compv_debug.h"

COMPV_NAMESPACE_BEGIN()

void CompVMathTrigFastAtan2_32f_Intrin_SSE2(COMPV_ALIGNED(SSE) const compv_float32_t* y, COMPV_ALIGNED(SSE) const compv_float32_t* x, COMPV_ALIGNED(SSE) compv_float32_t* r, const compv_float32_t* scale1, compv_uscalar_t width, compv_uscalar_t height, COMPV_ALIGNED(SSE) compv_uscalar_t stride)
{
	COMPV_DEBUG_INFO_CHECK_SSE2();
	COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("ASM AVX-FMA3 version is faster");
	static const __m128 vecAtan2_eps = _mm_load_ps(kAtan2Eps_32f);
	static const __m128 vecAtan2_p1 = _mm_load_ps(kAtan2P1_32f);
	static const __m128 vecAtan2_p3 = _mm_load_ps(kAtan2P3_32f);
	static const __m128 vecAtan2_p5 = _mm_load_ps(kAtan2P5_32f);
	static const __m128 vecAtan2_p7 = _mm_load_ps(kAtan2P7_32f);
	static const __m128 vecAtan2_zero = _mm_setzero_ps();
	static const __m128 vecAtan2_plus90 = _mm_set1_ps(90.f);
	static const __m128 vecAtan2_plus180 = _mm_set1_ps(180.f);
	static const __m128 vecAtan2_plus360 = _mm_set1_ps(360.f);
	static const __m128 vecAtan2_sign = _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff)); // used to compute fabs, not needed for ARM NEON
	const __m128 vecScale = _mm_set1_ps(*scale1);
	for (compv_uscalar_t j = 0; j < height; ++j) {
		for (compv_uscalar_t i = 0; i < width; i += 4) {
			// ax = std::abs(x[i]), ay = std::abs(y[i]);
			__m128 vecAx = _mm_and_ps(_mm_load_ps(&x[i]), vecAtan2_sign);
			__m128 vecAy = _mm_and_ps(_mm_load_ps(&y[i]), vecAtan2_sign);

			// if (ax >= ay) vec1 = ay, vec2 = ax;
			// else vec1 = ax, vec2 = ay;
			__m128 vecMask = _mm_cmpge_ps(vecAx, vecAy);
			__m128 vec1 = _mm_and_ps(vecMask, vecAy);
			__m128 vec2 = _mm_and_ps(vecMask, vecAx);
			vec1 = _mm_or_ps(vec1, _mm_andnot_ps(vecMask, vecAx));
			vec2 = _mm_or_ps(vec2, _mm_andnot_ps(vecMask, vecAy));

			// c = vec1 / (vec2 + atan2_eps)
			// c2 = c*c
			const __m128 vecC = _mm_div_ps(vec1, _mm_add_ps(vec2, vecAtan2_eps));
			const __m128 vecC2 = _mm_mul_ps(vecC, vecC);

			// a = (((atan2_p7*c2 + atan2_p5)*c2 + atan2_p3)*c2 + atan2_p1)*c
			__m128 vec0 = _mm_add_ps(_mm_mul_ps(vecAtan2_p7, vecC2), vecAtan2_p5); // TODO(dmi): AVX/NEON: Use fusedMulAdd
			vec0 = _mm_add_ps(_mm_mul_ps(vec0, vecC2), vecAtan2_p3); // TODO(dmi): AVX/NEON: Use fusedMulAdd
			vec0 = _mm_add_ps(_mm_mul_ps(vec0, vecC2), vecAtan2_p1); // TODO(dmi): AVX/NEON: Use fusedMulAdd
			vec0 = _mm_mul_ps(vec0, vecC);
			
			// if (!(ax >= ay)) a = 90 - a
			vec1 = _mm_andnot_ps(vecMask, _mm_sub_ps(vecAtan2_plus90, vec0));
			vec0 = _mm_or_ps(_mm_and_ps(vecMask, vec0), vec1);

			// if (x[i] < 0) a = 180.f - a
			vecMask = _mm_cmplt_ps(_mm_load_ps(&x[i]), vecAtan2_zero);
			vec1 = _mm_and_ps(vecMask, _mm_sub_ps(vecAtan2_plus180, vec0));
			vec0 = _mm_or_ps(_mm_andnot_ps(vecMask, vec0), vec1);

			// if (y[i + k] < 0) a = 360.f - a
			vecMask = _mm_cmplt_ps(_mm_load_ps(&y[i]), vecAtan2_zero);
			vec1 = _mm_and_ps(vecMask, _mm_sub_ps(vecAtan2_plus360, vec0));
			vec0 = _mm_or_ps(_mm_andnot_ps(vecMask, vec0), vec1);

			// r[i] = a * scale
			_mm_store_ps(&r[i], _mm_mul_ps(vec0, vecScale));
		}
		y += stride;
		x += stride;
		r += stride;
	}
}

COMPV_NAMESPACE_END()

#endif /* COMPV_ARCH_X86 && COMPV_INTRINSIC */