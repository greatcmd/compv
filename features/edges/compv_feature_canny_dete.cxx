/* Copyright (C) 2016-2017 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/

#include "compv/features/edges/compv_feature_canny_dete.h"
#include "compv/math/compv_math_convlt.h"
#include "compv/math/compv_math_utils.h"
#include "compv/compv_engine.h"
#include "compv/compv_gauss.h"

COMPV_NAMESPACE_BEGIN()

static const int COMPV_FEATURE_DETE_CANNY_GAUSS_KERN_SIZE = 5;
static const float COMPV_FEATURE_DETE_CANNY_GAUSS_KERN_SIGMA = 1.4f; // FIXME: 1.4

#define COMPV_FEATURE_DETE_CANNY_THRESHOLD_LOW	(0.68f)
#define COMPV_FEATURE_DETE_CANNY_THRESHOLD_HIGH	(COMPV_FEATURE_DETE_CANNY_THRESHOLD_LOW * 2.f)

static const float kTangentPiOver8 = 0.414213568f; // tan(22.5)
static const int32_t kTangentPiOver8Int = static_cast<int32_t>(kTangentPiOver8 * (1 << 16));
static const float kTangentPiTimes3Over8 = 2.41421366f; // tan(67.5)
static const int32_t kTangentPiTimes3Over8Int = static_cast<int32_t>(kTangentPiTimes3Over8 * (1 << 16));

CompVEdgeDeteCanny::CompVEdgeDeteCanny()
	: CompVEdgeDete(COMPV_CANNY_ID)
	, m_nImageWidth(0)
	, m_nImageHeight(0)
	, m_nImageStride(0)
	, m_fThresholdLow(COMPV_FEATURE_DETE_CANNY_THRESHOLD_LOW)
	, m_fThresholdHigh(COMPV_FEATURE_DETE_CANNY_THRESHOLD_HIGH)
	, m_pGx(NULL)
	, m_pGy(NULL)
	, m_pG(NULL)
	, m_pNms(NULL)
{

}

CompVEdgeDeteCanny::~CompVEdgeDeteCanny()
{
	CompVMem::free((void**)&m_pGx);
	CompVMem::free((void**)&m_pGy);
	CompVMem::free((void**)&m_pG);
	CompVMem::free((void**)&m_pNms);
}

// overrides CompVSettable::set
COMPV_ERROR_CODE CompVEdgeDeteCanny::set(int id, const void* valuePtr, size_t valueSize)
{
	COMPV_CHECK_EXP_RETURN(valuePtr == NULL || valueSize == 0, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
	switch (id) {
	case COMPV_CANNY_SET_FLOAT_THRESHOLD_LOW:{
		COMPV_CHECK_EXP_RETURN(valueSize != sizeof(float) || *((float*)valuePtr) <= 0.f, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
		m_fThresholdLow = *((float*)valuePtr);
		return COMPV_ERROR_CODE_S_OK;
	}
	case COMPV_CANNY_SET_FLOAT_THRESHOLD_HIGH:{
		COMPV_CHECK_EXP_RETURN(valueSize != sizeof(float) || *((float*)valuePtr) <= 0.f, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
		m_fThresholdHigh = *((float*)valuePtr);
		return COMPV_ERROR_CODE_S_OK;
	}
	default:
		return CompVSettable::set(id, valuePtr, valueSize);
	}
}

// overrides CompVEdgeDete::process
COMPV_ERROR_CODE CompVEdgeDeteCanny::process(const CompVPtr<CompVImage*>& image, CompVPtrArray(uint8_t)& edges)
{
	COMPV_CHECK_EXP_RETURN(!image || image->getPixelFormat() != COMPV_PIXEL_FORMAT_GRAYSCALE, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
	COMPV_CHECK_EXP_RETURN(m_fThresholdLow >= m_fThresholdHigh, COMPV_ERROR_CODE_E_INVALID_STATE);
	COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED(); // MT

	// Force memory realloc if image size changes
	if (!m_pGx || !m_pGy || !m_pG || !m_pNms || image->getWidth() != m_nImageWidth || image->getHeight() != m_nImageHeight || image->getStride() != m_nImageStride) {
		CompVMem::free((void**)&m_pGx);
		CompVMem::free((void**)&m_pGy);
		CompVMem::free((void**)&m_pG);
		CompVMem::free((void**)&m_pNms);
		m_nImageWidth = static_cast<size_t>(image->getWidth());
		m_nImageHeight = static_cast<size_t>(image->getHeight());
		m_nImageStride = static_cast<size_t>(image->getStride());
	}

	// Create edges buffer
	// edges must have same stride than m_pG (required by scaleAndClip) and image (required by gaussian blur)
	COMPV_CHECK_CODE_RETURN(CompVArray<uint8_t>::newObj(&edges, m_nImageHeight, m_nImageWidth, COMPV_SIMD_ALIGNV_DEFAULT, m_nImageStride));

	// FIXME(dmi): combine gaussianblur with sobel/scharr to one op
	// Gaussian Blurr
#if 0
	if (m_gblurKernFxp) {
		// Fixed-point
		COMPV_CHECK_CODE_RETURN(m_gblur->convlt1_fxp((uint8_t*)image->getDataPtr(), (int)m_nImageWidth, (int)m_nImageStride, (int)m_nImageHeight, m_gblurKernFxp->ptr(), m_gblurKernFxp->ptr(), COMPV_FEATURE_DETE_CANNY_GAUSS_KERN_SIZE, (uint8_t*)edges->ptr()));
	}
	else {
		// Floating-point
		COMPV_CHECK_CODE_RETURN(m_gblur->convlt1((uint8_t*)image->getDataPtr(), (int)m_nImageWidth, (int)m_nImageStride, (int)m_nImageHeight, m_gblurKernFloat->ptr(), m_gblurKernFloat->ptr(), COMPV_FEATURE_DETE_CANNY_GAUSS_KERN_SIZE, (uint8_t*)edges->ptr()));
	}
#endif

	// FIXME(dmi): use Scharr instead of sobel
	// FIXME(dmi): restore gaussian blur
	// Convolution
	// Convolution + gradient + mean can be packed for multithreading
	COMPV_CHECK_CODE_RETURN((CompVMathConvlt::convlt1<uint8_t, int16_t, int16_t>((const uint8_t*)image->getDataPtr(), m_nImageWidth, m_nImageStride, m_nImageHeight, &CompVSobelGx_vt[0], &CompVSobelGx_hz[0], 3, m_pGx)));
	COMPV_CHECK_CODE_RETURN((CompVMathConvlt::convlt1<uint8_t, int16_t, int16_t>((const uint8_t*)image->getDataPtr(), m_nImageWidth, m_nImageStride, m_nImageHeight, &CompVSobelGx_hz[0], &CompVSobelGx_vt[0], 3, m_pGy)));

	// Compute mean and get tLow and tMin
	// TODO(dmi): add support for otsu and median
	uint8_t mean = 1;
	COMPV_CHECK_CODE_RETURN((CompVMathUtils::mean<uint8_t, uint32_t>((const uint8_t*)image->getDataPtr(), m_nImageWidth, m_nImageHeight, m_nImageStride, mean)));
	const uint16_t tLow = static_cast<uint16_t>(mean * m_fThresholdLow);
	const uint16_t tHigh = static_cast<uint16_t>(mean * m_fThresholdHigh);

	// Compute gradiant using L1 distance.
	//!\\ Important: NMS function requires distance metric to be L1 instead of L2 (see comment in nms())
	// FIXME'dmi): gmax not needed
	uint16_t gmax = 1;
	COMPV_CHECK_CODE_RETURN((CompVMathUtils::gradientL1<int16_t, uint16_t>(m_pGx, m_pGy, m_pG, gmax, m_nImageWidth, m_nImageHeight, m_nImageStride)));

#if 0
	// scale (normalization)
	float scale = 255.f / float(gmax);
	uint8_t* edgesPtr = const_cast<uint8_t*>(edges->ptr());
	COMPV_CHECK_CODE_RETURN((CompVMathUtils::scaleAndClip<uint16_t, float, uint8_t>(m_pG, scale, edgesPtr, 0, 255, m_nImageWidth, m_nImageHeight, m_nImageStride)));
#endif

	// NMS
	COMPV_CHECK_CODE_RETURN(nms(edges, tLow));
	// Hysteresis
	hysteresis(edges, tLow, tHigh);

	return COMPV_ERROR_CODE_S_OK;
}

// NonMaximaSuppression
COMPV_ERROR_CODE CompVEdgeDeteCanny::nms(CompVPtrArray(uint8_t)& edges, uint16_t tLow)
{
	// Private function -> do not check input parameters
	COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED(); // MT
	if (!m_pNms) {
		m_pNms = (uint8_t*)CompVMem::calloc(edges->rows() * edges->cols(), sizeof(uint8_t));
		COMPV_CHECK_EXP_RETURN(!m_pNms, COMPV_ERROR_CODE_E_OUT_OF_MEMORY);
	}

	size_t row, col;
	const size_t maxRows = edges->rows() - 1, maxCols = edges->cols() - 1;
	const int16_t *gx = m_pGx + m_nImageStride + 1, *gy = m_pGy + m_nImageStride + 1;
	uint16_t *g = m_pG + m_nImageStride + 1;
	const uint16_t *top = g - m_nImageStride, *bottom = g + m_nImageStride, *left = g - 1, *right = g + 1;
	uint8_t *nms = m_pNms + m_nImageWidth + 1;
	uint8_t *e = const_cast<uint8_t*>(edges->ptr(1));
	int32_t gxInt, gyInt, absgyInt, absgxInt;
	const int stride = static_cast<const int>(m_nImageStride);
	const int pad = static_cast<const int>(m_nImageStride - m_nImageWidth) + 2;

	// non-maximasupp is multi-threaded and we will use this property to zero the edge buffer with low cost (compared to edges->zeroall() which is not MT)
	// TODO(dmi): perform next op only if startIndex == 0
	CompVMem::zero(e - m_nImageStride, m_nImageWidth); // zero first line
	CompVMem::zero(e + ((maxRows - 1) * m_nImageStride), m_nImageWidth); // zero last line

	// mark points to supp
	for (row = 1; row < maxRows; ++row) {
		CompVMem::zero(e, m_nImageWidth);
		for (col = 1; col < maxCols; ++col, ++nms, ++gx, ++gy, ++g, ++top, ++bottom, ++left, ++right) {
			// The angle is guessed using the first quadrant ([0-45] degree) only then completed.
			// We want "arctan(gy/gx)" to be within [0, 22.5], [22.5, 67.5], ]67.5, inf[, with 67.5 = (45. + 22.5)
			//!\\ All NMS values must be set to reset all values (no guarantee it contains zeros).
			
			// "theta = arctan(gy/gx)" -> "tan(theta) = gy/gx" -> compare "gy/gx" with "tan(quadrants)"
			// Instead of comparing "abs(gy/gx)" with "tanTheta" we will compare "abs(gy)" with "tanTheta*abs(gx)" to avoid division.
			
			if (*g >= tLow) {
				gxInt = static_cast<int32_t>(*gx);
				gyInt = static_cast<int32_t>(*gy);
				absgyInt = ((gyInt ^ (gyInt >> 31)) - (gyInt >> 31)) << 16;
				absgxInt = ((gxInt ^ (gxInt >> 31)) - (gxInt >> 31));
				if (absgyInt < (kTangentPiOver8Int * absgxInt)) { // angle = "0� / 180�"
					if (*left > *g || *right > *g) {
						*nms = 1;
					}
				}
				else if (absgyInt < (kTangentPiTimes3Over8Int * absgxInt)) { // angle = "45� / 225�" or "135 / 315"
					const int c = (gxInt ^ gyInt) < 0 ? 1 - stride : 1 + stride;
					if (g[-c] > *g || g[c] > *g) {
						*nms = 1;
					}
				}
				else { // angle = "90� / 270�"
					if (*top > *g || *bottom > *g) {
						*nms = 1;
					}
				}
			}
		}
		top += pad;
		bottom += pad;
		left += pad;
		right += pad;
		gx += pad;
		gy += pad;
		g += pad;
		e += m_nImageStride;
		nms += 2;
	}

	// supp marked points
	nms = m_pNms + m_nImageWidth + 1;
	g = m_pG + m_nImageStride + 1;
	for (row = 1; row < maxRows; ++row) {
		for (col = 1; col < maxCols; ++col, ++g, ++nms) {
			if (*nms) {
				*g = 0;
				*nms = 0;
			}
		}
		nms += 2;
		g += pad;
	}

	return COMPV_ERROR_CODE_S_OK;
}

static COMPV_INLINE void connectEdge(uint8_t* pixel, const uint16_t* g, size_t rowIdx, size_t colIdx, int stride, size_t maxRows, size_t maxCols, uint16_t tLow)
{	
	// Private function -> do not check input parameters
	if (rowIdx && colIdx && rowIdx < maxRows && colIdx < maxCols) {
		*pixel = 0xff; // set as strong edge
		if (pixel[-1] ^ 0xff && g[-1] >= tLow) { // left
			connectEdge(pixel - 1, g - 1, rowIdx, colIdx - 1, stride, maxRows, maxCols, tLow);
		}
		if (pixel[1] ^ 0xff && g[1] >= tLow) { // right
			connectEdge(pixel + 1, g + 1, rowIdx, colIdx + 1, stride, maxRows, maxCols, tLow);
		}
		if (pixel[-stride - 1] ^ 0xff && g[-stride - 1] >= tLow) { // left-top
			connectEdge(pixel - stride - 1, g - stride - 1, rowIdx - 1, colIdx - 1, stride, maxRows, maxCols, tLow);
		}
		if (pixel[-stride] ^ 0xff && g[-stride] >= tLow) { // top
			connectEdge(pixel - stride, g - stride, rowIdx - 1, colIdx, stride, maxRows, maxCols, tLow);
		}
		if (pixel[-stride + 1] ^ 0xff && g[-stride + 1] >= tLow) { // right-top
			connectEdge(pixel - stride + 1, g - stride + 1, rowIdx - 1, colIdx + 1, stride, maxRows, maxCols, tLow);
		}
		if (pixel[stride - 1] ^ 0xff && g[stride - 1] >= tLow) { // left-bottom
			connectEdge(pixel + stride - 1, g + stride - 1, rowIdx + 1, colIdx - 1, stride, maxRows, maxCols, tLow);
		}
		if (pixel[stride] ^ 0xff && g[stride] >= tLow) { // bottom
			connectEdge(pixel + stride, g + stride, rowIdx + 1, colIdx, stride, maxRows, maxCols, tLow);
		}
		if (pixel[stride + 1] ^ 0xff && g[stride + 1] >= tLow) { // right-bottom
			connectEdge(pixel + stride + 1, g + stride + 1, rowIdx + 1, colIdx + 1, stride, maxRows, maxCols, tLow);
		}
	}
}

void CompVEdgeDeteCanny::hysteresis(CompVPtrArray(uint8_t)& edges, uint16_t tLow, uint16_t tHigh)
{
	// Private function -> do not check input parameters
	COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED();  // MT

	size_t row, col;
	const size_t maxRows = edges->rows() - 1, maxCols = edges->cols() - 1;
	const uint16_t *g = m_pG + m_nImageStride;
	uint8_t* e = const_cast<uint8_t*>(edges->ptr(1));
	const int stride = static_cast<const int>(m_nImageStride);
	for (row = 1; row < maxRows; ++row) {
		for (col = 1; col < maxCols; ++col) {
			if (g[col] >= tHigh) { // strong edge
				connectEdge((e + col), (g + col), row, col, stride, maxRows, maxCols, tLow);
			}
		}
		g += m_nImageStride;
		e += m_nImageStride;
	}
}

COMPV_ERROR_CODE CompVEdgeDeteCanny::newObj(CompVPtr<CompVEdgeDete* >* dete)
{
	COMPV_CHECK_EXP_RETURN(!dete, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
	CompVPtr<CompVEdgeDeteCanny* >dete_ = new CompVEdgeDeteCanny();
	COMPV_CHECK_EXP_RETURN(!dete_, COMPV_ERROR_CODE_E_OUT_OF_MEMORY);
	COMPV_CHECK_CODE_RETURN(CompVConvlt<float>::newObj(&dete_->m_gblur));
	if (CompVEngine::isMathFixedPoint()) {
		COMPV_CHECK_CODE_RETURN(CompVGaussKern<float>::buildKern1_fxp(&dete_->m_gblurKernFxp, COMPV_FEATURE_DETE_CANNY_GAUSS_KERN_SIZE, COMPV_FEATURE_DETE_CANNY_GAUSS_KERN_SIGMA));
	}
	if (!dete_->m_gblurKernFxp) {
		COMPV_CHECK_CODE_RETURN(CompVGaussKern<float>::buildKern1(&dete_->m_gblurKernFloat, COMPV_FEATURE_DETE_CANNY_GAUSS_KERN_SIZE, COMPV_FEATURE_DETE_CANNY_GAUSS_KERN_SIGMA));
	}

	*dete = *dete_;
	return COMPV_ERROR_CODE_S_OK;
}

COMPV_NAMESPACE_END()