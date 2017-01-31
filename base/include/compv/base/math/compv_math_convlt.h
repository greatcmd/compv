/* Copyright (C) 2016-2017 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#if !defined(_COMPV_BASE_MATH_CONVLT_H_)
#define _COMPV_BASE_MATH_CONVLT_H_

#include "compv/base/compv_config.h"
#include "compv/base/compv_common.h"
#include "compv/base/compv_mem.h"
#include "compv/base/compv_mat.h"
#include "compv/base/math/compv_math.h"
#include "compv/base/parallel/compv_parallel.h"

COMPV_NAMESPACE_BEGIN()

class COMPV_BASE_API CompVMathConvlt
{
public:
	// Convolution using separable kernel
	// sizeof(outPtr) should be computed using CompVMathConvlt::outputSizeInBytes()
	template <typename InputType = uint8_t, typename KernelType = compv_float32_t, typename OutputType = uint8_t>
	static COMPV_ERROR_CODE convlt1(const InputType* dataPtr, size_t dataWidth, size_t dataHeight, size_t dataStride, const KernelType* vtKernPtr, const KernelType* hzKernPtr, size_t kernSize, OutputType*& outPtr, size_t dataBorder = 0) {
		return CompVMathConvlt::convlt1_private<InputType, KernelType, OutputType>(dataPtr, dataWidth, dataHeight, dataStride, vtKernPtr, hzKernPtr, kernSize, outPtr, dataBorder, false);
	}

	static COMPV_ERROR_CODE convlt1FixedPoint(const uint8_t* dataPtr, size_t dataWidth, size_t dataHeight, size_t dataStride, const int16_t* vtKernPtr, const int16_t* hzKernPtr, size_t kernSize, uint8_t*& outPtr, size_t dataBorder = 0) {
		return CompVMathConvlt::convlt1_private<uint8_t, int16_t, uint8_t>(dataPtr, dataWidth, dataHeight, dataStride, vtKernPtr, hzKernPtr, kernSize, outPtr, dataBorder, true);
	}

	template <typename InputType = uint8_t, typename KernelType = compv_float32_t, typename OutputType = uint8_t>
	static void convlt1Hz(const InputType* inPtr, OutputType* outPtr, size_t width, size_t height, size_t stride, const KernelType* hzKernPtr, size_t kernSize, bool resetBorders = true) {
		CompVMathConvlt::convlt1Hz_private<InputType, KernelType, OutputType>(inPtr, outPtr, width, height, stride, hzKernPtr, kernSize, resetBorders, false);
	}

	static void convlt1HzFixedPoint(const uint8_t* inPtr, uint8_t* outPtr, size_t width, size_t height, size_t stride, const int16_t* hzKernPtr, size_t kernSize, bool resetBorders = true) {
		CompVMathConvlt::convlt1Hz_private<uint8_t, int16_t, uint8_t>(inPtr, outPtr, width, height, stride, hzKernPtr, kernSize, resetBorders, true);
	}

	template <typename InputType = uint8_t, typename KernelType = compv_float32_t, typename OutputType = uint8_t>
	static void convlt1Vt(const InputType* inPtr, OutputType* outPtr, size_t width, size_t height, size_t stride, const KernelType* vtKernPtr, size_t kernSize, bool resetTopBorder = true, bool resetBottomBorder = true) {
		CompVMathConvlt::convlt1Vt_private<InputType, KernelType, OutputType>(inPtr, outPtr, width, height, stride, vtKernPtr, kernSize, resetTopBorder, resetBottomBorder, false);
	}

	static void convlt1VtFixedPoint(const uint8_t* inPtr, uint8_t* outPtr, size_t width, size_t height, size_t stride, const int16_t* vtKernPtr, size_t kernSize, bool resetTopBorder = true, bool resetBottomBorder = true) {
		CompVMathConvlt::convlt1Vt_private<uint8_t, int16_t, uint8_t>(inPtr, outPtr, width, height, stride, vtKernPtr, kernSize, resetTopBorder, resetBottomBorder, false);
	}

	// Convolution using no separable kernel
	// sizeof(outPtr) should be at least equal to (dataHeight * dataStride)
	template <typename InputType = uint8_t, typename KernelType = compv_float32_t, typename OutputType = uint8_t>
	static COMPV_ERROR_CODE convlt2(const InputType* dataPtr, size_t dataWidth, size_t dataStride, size_t dataHeight, const KernelType* kernPtr, size_t kernSize, OutputType** outPtr, size_t dataBorder = 0) {
		return CompVMathConvlt::convlt2_private<InputType, KernelType, OutputType>(dataPtr, dataWidth, dataStride, dataHeight, kernPtr, kernSize, outPtr, dataBorder, false);
	}

	static COMPV_ERROR_CODE convlt2FixedPoint(const uint8_t* dataPtr, size_t dataWidth, size_t dataStride, size_t dataHeight, const int16_t* kernPtr, size_t kernSize, uint8_t** outPtr, size_t dataBorder = 0) {
		return CompVMathConvlt::convlt2_private<uint8_t, int16_t, uint8_t>(dataPtr, dataWidth, dataStride, dataHeight, kernPtr, kernSize, outPtr, dataBorder, false);
	}

	template <typename OutputType = uint8_t>
	static size_t outputSizeInBytes(size_t dataStride, size_t dataHeight, size_t dataBorder = 0) {
		return ((dataHeight + (dataBorder << 1)) * (dataStride + (dataBorder << 1))) * sizeof(OutputType);
	}

	// kernel should be normalized
	template <typename KernelType = compv_float32_t>
	static COMPV_ERROR_CODE fixedPointKernel(CompVMatPtr normalizedKernel, CompVMatPtrPtr fixedPointKernel) {
		COMPV_CHECK_EXP_RETURN(!normalizedKernel || !fixedPointKernel, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
		COMPV_CHECK_CODE_RETURN((CompVMat::newObjAligned<int16_t>(fixedPointKernel, normalizedKernel->rows(), normalizedKernel->cols())));
		const KernelType* aPtr;
		int16_t* bPtr;
		for (size_t row = 0; row < normalizedKernel->rows(); ++row) {
			aPtr = normalizedKernel->ptr<const KernelType>();
			bPtr = (*fixedPointKernel)->ptr<int16_t>();
			for (size_t col = 0; col < normalizedKernel->cols(); ++col) {
				bPtr[col] = static_cast<int16_t>(aPtr[col] * 0x7ffff);
			}
		}
		return COMPV_ERROR_CODE_S_OK;
	}

private:
	// Convolution using separable kernel
	// sizeof(outPtr) must be computed using CompVMathConvlt::outputSizeInBytes()
	template <typename InputType = uint8_t, typename KernelType = compv_float32_t, typename OutputType = uint8_t>
	static COMPV_ERROR_CODE convlt1_private(const InputType* dataPtr, size_t dataWidth, size_t dataHeight, size_t dataStride, const KernelType* vtKernPtr, const KernelType* hzKernPtr, size_t kernSize, OutputType*& outPtr, size_t dataBorder = 0, bool fixedPoint = false) {
		// Check inputs
		COMPV_CHECK_EXP_RETURN(!dataPtr || !outPtr || (dataWidth < kernSize * 2) || (dataHeight < kernSize * 2) || (dataStride < dataWidth) || !vtKernPtr || !hzKernPtr || dataBorder < 0 || !(kernSize & 1), COMPV_ERROR_CODE_E_INVALID_PARAMETER);

		// The realloc_aligned() implementation memcpy() old data which is slow. Prefer, free_aligned() followed by malloc_aligned()

		/* Alloc memory */
		size_t neededSize = CompVMathConvlt::outputSizeInBytes<OutputType>(dataStride, dataHeight, dataBorder);
		bool outPtrAllocated = false;
		if (!outPtr) {
			outPtr = reinterpret_cast<OutputType*>(CompVMem::malloc(neededSize));
			COMPV_CHECK_EXP_RETURN(!outPtr, COMPV_ERROR_CODE_E_OUT_OF_MEMORY);
			outPtrAllocated = true;
		}
		// Allocate tmp memory
		OutputType* imgTmp = NULL;
		imgTmp = reinterpret_cast<OutputType*>(CompVMem::malloc(neededSize));
		if (!imgTmp) {
			if (outPtrAllocated) {
				CompVMem::free(reinterpret_cast<void**>(&outPtr));
			}
			COMPV_CHECK_CODE_RETURN(COMPV_ERROR_CODE_E_OUT_OF_MEMORY, "Failed to allocate temporary memory");
		}
		size_t threadsCount;
		CompVThreadDispatcherPtr threadDisp = CompVParallel::threadDispatcher();
		size_t maxThreads = threadDisp ? static_cast<size_t>(threadDisp->threadsCount()) : 0;

		// Compute number of threads
		threadsCount = (threadDisp && !threadDisp->isMotherOfTheCurrentThread())
			? COMPV_MATH_MIN(maxThreads, (dataHeight / (kernSize << 1))) /* at least "rowsOverlapCount" */
			: 1;

		// Check if if overlaping is small
		const bool bKernelSizeTooHighForMT = kernSize > (static_cast<size_t>(dataHeight / threadsCount) >> 1);
		if (bKernelSizeTooHighForMT) {
			COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("Kernel size too high for MT");
		}

		if (threadsCount > 1 && !bKernelSizeTooHighForMT) {
			CompVAsyncTaskIds taskIds;
			const size_t rowsOverlapCount = ((kernSize >> 1) << 1); // (kernelRadius times 2)
			const size_t rowsOverlapPad = rowsOverlapCount * dataStride;
			const size_t countAny = dataHeight / threadsCount;
			const size_t countLast = dataHeight - ((threadsCount - 1) * countAny);
			const InputType* inPtr_ = dataPtr;
			OutputType* tmpPtr_ = imgTmp;
			OutputType* outPtr_ = outPtr;
			taskIds.reserve(threadsCount);
			auto funcPtrFirst = [&](const InputType* ptrIn, OutputType* ptrOut, OutputType* ptrTmp, size_t h) -> COMPV_ERROR_CODE {
				CompVMathConvlt::convlt1Hz_private<InputType, KernelType, OutputType>(ptrIn, ptrTmp, dataWidth, h + rowsOverlapCount, dataStride, hzKernPtr, kernSize, true, fixedPoint);
				CompVMathConvlt::convlt1Vt_private<OutputType, KernelType, OutputType>(ptrTmp, ptrOut, dataWidth, h + rowsOverlapCount, dataStride, vtKernPtr, kernSize, true, false, fixedPoint);
				return COMPV_ERROR_CODE_S_OK;
			};
			auto funcPtrOthers = [&](const InputType* ptrIn, OutputType* ptrOut, OutputType* ptrTmp, size_t h, bool last) -> COMPV_ERROR_CODE {
				CompVMathConvlt::convlt1Hz_private<InputType, KernelType, OutputType>(ptrIn - rowsOverlapPad, ptrTmp - rowsOverlapPad, dataWidth, h + rowsOverlapCount, dataStride, hzKernPtr, kernSize, true, fixedPoint);
				CompVMathConvlt::convlt1Vt_private<OutputType, KernelType, OutputType>(ptrTmp - rowsOverlapPad, ptrOut - rowsOverlapPad, dataWidth, h + rowsOverlapCount, dataStride, vtKernPtr, kernSize, false, last, fixedPoint);
				return COMPV_ERROR_CODE_S_OK;
			};
			/* first */
			COMPV_CHECK_CODE_RETURN(threadDisp->invoke(std::bind(funcPtrFirst, inPtr_, outPtr_, tmpPtr_, countAny), taskIds));
			inPtr_ += countAny * dataStride;
			tmpPtr_ += countAny * dataStride;
			outPtr_ += countAny * dataStride;
			/* others */
			for (int32_t threadIdx = 1; threadIdx < threadsCount - 1; ++threadIdx) {
				COMPV_CHECK_CODE_RETURN(threadDisp->invoke(std::bind(funcPtrOthers, inPtr_, outPtr_, tmpPtr_, countAny, false), taskIds));
				inPtr_ += countAny * dataStride;
				tmpPtr_ += countAny * dataStride;
				outPtr_ += countAny * dataStride;
			}
			/* last */
			COMPV_CHECK_CODE_RETURN(threadDisp->invoke(std::bind(funcPtrOthers, inPtr_, outPtr_, tmpPtr_, countLast, true), taskIds));
			/* wait */
			COMPV_CHECK_CODE_RETURN(threadDisp->wait(taskIds));
		}
		else {
			CompVMathConvlt::convlt1Hz_private<InputType, KernelType, OutputType>(dataPtr, imgTmp, dataWidth, dataHeight, dataStride, hzKernPtr, kernSize, true, fixedPoint);
			CompVMathConvlt::convlt1Vt_private<OutputType, KernelType, OutputType>(imgTmp, outPtr, dataWidth, dataHeight, dataStride, vtKernPtr, kernSize, true, true, fixedPoint);
		}

		CompVMem::free(reinterpret_cast<void**>(&imgTmp));

		return COMPV_ERROR_CODE_S_OK;
	}

	template <typename InputType = uint8_t, typename KernelType = compv_float32_t, typename OutputType = uint8_t>
	static void convlt1Hz_private(const InputType* inPtr, OutputType* outPtr, size_t width, size_t height, size_t stride, const KernelType* hzKernPtr, size_t kernSize, bool resetBorders = true, bool fixedPoint = false) {
		size_t ker_size_div2 = (kernSize >> 1);
		size_t imgpad = ((stride - width) + ker_size_div2 + ker_size_div2);
		// Set hz borders to zero
		// We must not accept garbage in the border (could be used by the calling function -e.g to find the max value for normalization)
		if (resetBorders) {
			OutputType *outPtr0 = outPtr, *outPtr1 = outPtr + (width - ker_size_div2);
			for (size_t row = 0; row < height; ++row) {
				for (size_t col = 0; col < ker_size_div2; ++col) {
					outPtr0[col] = 0, outPtr1[col] = 0;
				}
				outPtr0 += stride;
				outPtr1 += stride;
			}
		}
		// Perform horizontal convolution
		CompVMathConvlt::convlt1VtHz_private<InputType, KernelType, OutputType>(inPtr, outPtr + ker_size_div2, static_cast<size_t>(width - ker_size_div2 - ker_size_div2), height, 1, imgpad, hzKernPtr, kernSize, fixedPoint);
	}

	template <typename InputType = uint8_t, typename KernelType = compv_float32_t, typename OutputType = uint8_t>
	static void convlt1Vt_private(const InputType* inPtr, OutputType* outPtr, size_t width, size_t height, size_t stride, const KernelType* vtKernPtr, size_t kernSize, bool resetTopBorder = true, bool resetBottomBorder = true, bool fixedPoint = false) {
		size_t ker_size_div2 = (kernSize >> 1);
		size_t imgpad = (stride - width);
		// Set top and bottom vert borders to zero
		// We must not accept garbage in the border (coul be used by the calling function -e.g to find the max value for normalization)
		const size_t bSize = (ker_size_div2 * stride) * sizeof(OutputType);
		if (resetTopBorder) {
			CompVMem::zero(outPtr, bSize);
		}
		if (resetBottomBorder) {
			CompVMem::zero(outPtr + ((height - ker_size_div2) * stride), bSize);
		}
		// Perform vertical convolution
		CompVMathConvlt::convlt1VtHz_private<InputType, KernelType, OutputType>(inPtr, outPtr + (ker_size_div2 * stride), width, (height - ker_size_div2 - ker_size_div2), stride, imgpad, vtKernPtr, kernSize, fixedPoint);
	}

	// Convolution using no separable kernel
	// sizeof(outPtr) must be at least equal to (dataHeight * dataStride)
	template <typename InputType = uint8_t, typename KernelType = compv_float32_t, typename OutputType = uint8_t>
	static COMPV_ERROR_CODE convlt2_private(const InputType* dataPtr, size_t dataWidth, size_t dataStride, size_t dataHeight, const KernelType* kernPtr, size_t kernSize, OutputType** outPtr, size_t dataBorder = 0, bool fixedPoint = false) {
		COMPV_CHECK_CODE_RETURN(COMPV_ERROR_CODE_E_NOT_IMPLEMENTED); // see deprecated code
		return COMPV_ERROR_CODE_S_OK;
	}


	template <typename InputType, typename KernelType, typename OutputType>
	static void convlt1VtHz_private(const InputType* inPtr, OutputType* outPtr, size_t width, size_t height, size_t stride, size_t pad, const KernelType* vthzKernPtr, size_t kernSize, bool fixedPoint = false) {
		if (fixedPoint) {
			if (std::is_same<InputType, uint8_t>::value && std::is_same<OutputType, uint8_t>::value && std::is_same<KernelType, int16_t>::value) {
				CompVMathConvlt::convlt1VtHzFixedPoint_C(reinterpret_cast<const uint8_t*>(inPtr), reinterpret_cast<uint8_t*>(outPtr), width, height, stride, pad, reinterpret_cast<const int16_t*>(vthzKernPtr), kernSize);
				return;
			}
			else {
				COMPV_DEBUG_ERROR_EX("CompVMathConvlt", "Type mismatch for fixed point implementation");
			}
		}
		CompVMathConvlt::convlt1VtHz_C<InputType, KernelType, OutputType>(inPtr, outPtr, width, height, stride, pad, vthzKernPtr, kernSize);
	}

	template <typename InputType, typename KernelType, typename OutputType>
	static void convlt1VtHz_C(const InputType* inPtr, OutputType* outPtr, size_t width, size_t height, size_t stride, size_t pad, const KernelType* vthzKernPtr, size_t kernSize) {
		COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("No SIMD or GPU implementation found");
		size_t i, j, k, row;
		OutputType sum;

		for (j = 0; j < height; ++j) {
			for (i = 0; i < width; ++i) {
				sum = static_cast<OutputType>(*inPtr **vthzKernPtr);
				for (row = 1, k = stride; row < kernSize; ++row, k += stride) {
					sum += static_cast<OutputType>(inPtr[k] * vthzKernPtr[row]);
				}
				*outPtr = static_cast<OutputType>(sum);
				++inPtr;
				++outPtr;
			}
			inPtr += pad;
			outPtr += pad;
		}
	}
	
	static void convlt1VtHzFixedPoint_C(const uint8_t* inPtr, uint8_t* outPtr, size_t width, size_t height, size_t stride, size_t pad, const int16_t* vthzKernPtr, size_t kernSize) {
		COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("No SIMD or GPU implementation found");
		size_t i, j, k, row;
		int sum;

		for (j = 0; j < height; ++j) {
			for (i = 0; i < width; ++i) {
				sum = static_cast<int>(*inPtr **vthzKernPtr) >> 16;
				for (row = 1, k = stride; row < kernSize; ++row, k += stride) {
					sum += static_cast<int>(inPtr[k] * vthzKernPtr[row]) >> 16;
				}
				*outPtr = static_cast<uint8_t>(sum);
				++inPtr;
				++outPtr;
			}
			inPtr += pad;
			outPtr += pad;
		}
	}
};

COMPV_NAMESPACE_END()

#endif /* _COMPV_BASE_MATH_CONVLT_H_ */