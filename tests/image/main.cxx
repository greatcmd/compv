#include "../tests_common.h"

using namespace compv;

#define TAG_TEST_IMAGE "TestImage"

#define TEST_SCALE					0
#define TEST_PYRAMID				0
#define TEST_CHROMA_CONV			0
#define TEST_FEATURE_FAST			0
#define TEST_FEATURE_ORB			0
#define TEST_PATCH_MOMENTS			0
#define TEST_CCL					0
#define TEST_SOBEL					0
#define TEST_CANNY					1
#define TEST_ADAPT_THRESH			0
#define TEST_OTSU_THRESH			0
#define TEST_HOUGHSHT				0
#define TEST_HOUGHKHT				0
#define TEST_BRUTEFORCE				0
#define TEST_SPLIT3					0

/* Entry point function */
compv_main()
{
	COMPV_ERROR_CODE err = COMPV_ERROR_CODE_S_OK;
	{
        
		COMPV_CHECK_CODE_BAIL(err = compv_tests_init());
#if TEST_SCALE
		extern COMPV_ERROR_CODE scale();
		COMPV_CHECK_CODE_BAIL(err = scale(), TAG_TEST_IMAGE "Image scaling test failed");
#endif
#if TEST_PYRAMID
		extern COMPV_ERROR_CODE pyramid();
		COMPV_CHECK_CODE_BAIL(err = pyramid(), TAG_TEST_IMAGE "Image pyramid test failed");
#endif
#if TEST_CHROMA_CONV
		extern COMPV_ERROR_CODE chroma_conv();
		COMPV_CHECK_CODE_BAIL(err = chroma_conv(), TAG_TEST_IMAGE "Chroma conversion test failed");
#endif
#if TEST_FEATURE_FAST
		extern COMPV_ERROR_CODE feature_fast();
		COMPV_CHECK_CODE_BAIL(err = feature_fast(), TAG_TEST_IMAGE "FAST feature detection test failed");
#endif
#if TEST_FEATURE_ORB
		extern COMPV_ERROR_CODE feature_orb();
		COMPV_CHECK_CODE_BAIL(err = feature_orb(), TAG_TEST_IMAGE "ORB feature detection and description test failed");
#endif
#if TEST_PATCH_MOMENTS
		extern COMPV_ERROR_CODE patch_moments0110();
		COMPV_CHECK_CODE_BAIL(err = patch_moments0110(), TAG_TEST_IMAGE "Image moments test failed");
#endif
#if TEST_CCL
		extern COMPV_ERROR_CODE ccl();
		COMPV_CHECK_CODE_BAIL(err = ccl(), TAG_TEST_IMAGE "Connected component labeling test failed");
#endif

#if TEST_SOBEL
		extern COMPV_ERROR_CODE sobel();
		COMPV_CHECK_CODE_BAIL(err = sobel(), TAG_TEST_IMAGE "Sobel Edge dete test failed");
#endif
#if TEST_CANNY
		extern COMPV_ERROR_CODE canny();
		COMPV_CHECK_CODE_BAIL(err = canny(), TAG_TEST_IMAGE "Canny Edge dete test failed");
#endif
#if TEST_ADAPT_THRESH
		extern COMPV_ERROR_CODE adaptiveThreshold();
		COMPV_CHECK_CODE_BAIL(err = adaptiveThreshold(), TAG_TEST_IMAGE "Adaptive Threshold test failed");
#endif
#if TEST_OTSU_THRESH
		extern COMPV_ERROR_CODE otsuThreshold();
		COMPV_CHECK_CODE_BAIL(err = otsuThreshold(), TAG_TEST_IMAGE "Otsu Threshold test failed");
#endif
#if TEST_HOUGHSHT
		extern COMPV_ERROR_CODE houghsht();
		COMPV_CHECK_CODE_BAIL(err = houghsht(), TAG_TEST_IMAGE "HoughSht line dete test failed");
#endif
#if TEST_HOUGHKHT
		extern COMPV_ERROR_CODE houghkht();
		COMPV_CHECK_CODE_BAIL(err = houghkht(), TAG_TEST_IMAGE "HoughKht line dete test failed");
#endif

		
		
#if TEST_BRUTEFORCE
		extern COMPV_ERROR_CODE bruteforce();
		COMPV_CHECK_CODE_BAIL(err = bruteforce(), TAG_TEST_IMAGE "Bruteforce test failed");
#endif


#if TEST_SPLIT3
		extern COMPV_ERROR_CODE split3();
		COMPV_CHECK_CODE_BAIL(err = split3(), TAG_TEST_IMAGE "Split3 test failed");
#endif

	bail:
		COMPV_CHECK_CODE_ASSERT(err, TAG_TEST_IMAGE "Something went wrong!!");
		COMPV_CHECK_CODE_ASSERT(err = compv_tests_deInit());
	}

	COMPV_DEBUG_CHECK_FOR_MEMORY_LEAKS();

	COMPV_DEBUG_INFO_EX(TAG_TEST_IMAGE, "************* Program ended!!! *************");

	compv_main_return(static_cast<int>(err));
}
