#include <compv/compv_api.h>

using namespace compv;

CompVWindowPtr window;

static void* COMPV_STDCALL WorkerThread(void* arg);

compv_main()
{
	COMPV_ERROR_CODE err;

	// Change debug level to INFO before starting
	CompVDebugMgr::setLevel(COMPV_DEBUG_LEVEL_INFO);
	
	// Init the modules
	COMPV_CHECK_CODE_BAIL(err = CompVInit());

	// Create "Hello world!" window
	COMPV_CHECK_CODE_BAIL(err = CompVWindow::newObj(&window, 670, 580, "Hello world!"));

	// Start ui runloop
	COMPV_CHECK_CODE_BAIL(err = CompVDrawing::runLoop(WorkerThread));

bail:
	if (COMPV_ERROR_CODE_IS_NOK(err)) {
		COMPV_DEBUG_ERROR("Something went wrong!!");
	}

	window = NULL;
	
	// DeInit the modules
	COMPV_CHECK_CODE_ASSERT(err = CompVDeInit());
	// Make sure we freed all allocated memory
	COMPV_ASSERT(CompVMem::isEmpty());
	// Make sure we freed all allocated objects
	COMPV_ASSERT(CompVObj::isEmpty());

	compv_main_return(0);
}

static void* COMPV_STDCALL WorkerThread(void* arg)
{
#if 1 // Matching
	COMPV_ERROR_CODE err;
	CompVMatPtr mat[3];
	CompVMatchingSurfaceLayerPtr matchingSurfaceLayer;
	CompVViewportPtr ptrViewPort;
	CompVMVPPtr ptrMVP;
	static int count = 0;
	char buff_[33] = { 0 };

	COMPV_CHECK_CODE_BAIL(err = CompVImageDecoder::decodeFile(COMPV_PATH_FROM_NAME("girl.jpg"), &mat[0]));
	COMPV_CHECK_CODE_BAIL(err = CompVImageDecoder::decodeFile(COMPV_PATH_FROM_NAME("valve_original.jpg"), &mat[1]));
	COMPV_CHECK_CODE_BAIL(err = CompVImageDecoder::decodeFile(COMPV_PATH_FROM_NAME("mandekalou.jpg"), &mat[2]));

	COMPV_CHECK_CODE_BAIL(err = window->addMatchingLayerSurface(&matchingSurfaceLayer));

	while (CompVDrawing::isLoopRunning()) {
		snprintf(buff_, sizeof(buff_), "%d", static_cast<int>(count));
		std::string text = "Hello doubango telecom [" + std::string(buff_) + "]";
		COMPV_CHECK_CODE_BAIL(err = window->beginDraw());
		COMPV_CHECK_CODE_BAIL(matchingSurfaceLayer->drawMatches(mat[0/*(count + 0) % 3*/], mat[1/*(count + 0) % 3*/]));		
		COMPV_CHECK_CODE_BAIL(err = matchingSurfaceLayer->blit());
		COMPV_CHECK_CODE_BAIL(err = window->endDraw());

		++count;
	}

bail:
	return NULL;
#elif 1 // Single
	COMPV_ERROR_CODE err;
	CompVMatPtr mat[3];
	CompVSingleSurfaceLayerPtr singleSurfaceLayer;
	CompVRendererPtr ptrImageRenderer;
	CompVViewportPtr ptrViewPort;
	CompVMVPPtr ptrMVP;
	static int count = 0;
	char buff_[33] = { 0 };
	
	COMPV_CHECK_CODE_BAIL(err = CompVImageDecoder::decodeFile(COMPV_PATH_FROM_NAME("girl.jpg"), &mat[0]));
	COMPV_CHECK_CODE_BAIL(err = CompVImageDecoder::decodeFile(COMPV_PATH_FROM_NAME("valve_original.jpg"), &mat[1]));
	COMPV_CHECK_CODE_BAIL(err = CompVImageDecoder::decodeFile(COMPV_PATH_FROM_NAME("mandekalou.jpg"), &mat[2]));

	COMPV_CHECK_CODE_BAIL(err = window->addSingleLayerSurface(&singleSurfaceLayer));

	// Set viewport
	COMPV_CHECK_CODE_BAIL(err = CompVViewport::newObj(&ptrViewPort, CompViewportSizeFlags::makeDynamicAspectRatio()));
	COMPV_CHECK_CODE_BAIL(err = singleSurfaceLayer->surface()->setViewport(ptrViewPort));

	// Set MVP
	COMPV_CHECK_CODE_BAIL(err = CompVMVP::newObjProjection2D(&ptrMVP));
	COMPV_CHECK_CODE_BAIL(err = ptrMVP->model()->matrix()->scale(CompVDrawingVec3f(1.f, 1.f, 1.f)));
	COMPV_CHECK_CODE_BAIL(err = ptrMVP->view()->setCamera(CompVDrawingVec3f(0.f, 0.f, 1.f), CompVDrawingVec3f(0.f, 0.f, 0.f), CompVDrawingVec3f(0.f, 1.f, 0.f)));
	COMPV_CHECK_CODE_BAIL(err = ptrMVP->projection2D()->setOrtho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f));
	COMPV_CHECK_CODE_BAIL(err = singleSurfaceLayer->surface()->setMVP(ptrMVP));

	while (CompVDrawing::isLoopRunning()) {
		snprintf(buff_, sizeof(buff_), "%d", static_cast<int>(count));
		std::string text = "Hello doubango telecom [" + std::string(buff_) + "]";
		COMPV_CHECK_CODE_BAIL(err = window->beginDraw());
		COMPV_CHECK_CODE_BAIL(err = singleSurfaceLayer->surface()->drawImage(mat[0/*(count + 0) % 3*/], &ptrImageRenderer));
		//COMPV_CHECK_CODE_BAIL(err = singleSurfaceLayer->surface()->drawText(text.c_str(), text.length(), 463, 86));
		COMPV_CHECK_CODE_BAIL(err = ptrImageRenderer->drawText(text.c_str(), text.length(), 463, 86));
		COMPV_CHECK_CODE_BAIL(err = singleSurfaceLayer->blit());
		COMPV_CHECK_CODE_BAIL(err = window->endDraw());

		//COMPV_CHECK_EXP_BAIL(COMPV_ERROR_CODE_IS_NOK(err = window->beginDraw()) && err != COMPV_ERROR_CODE_W_WINDOW_CLOSED, err);
		//COMPV_CHECK_EXP_BAIL(COMPV_ERROR_CODE_IS_NOK(err = window->surface()->drawImage(mat[count % 3])) && err != COMPV_ERROR_CODE_W_WINDOW_CLOSED, err);
		//COMPV_CHECK_EXP_BAIL(COMPV_ERROR_CODE_IS_NOK(err = window->surface()->drawText(text.c_str(), text.length())) && err != COMPV_ERROR_CODE_W_WINDOW_CLOSED, err);
		//COMPV_CHECK_EXP_BAIL(COMPV_ERROR_CODE_IS_NOK(err = window->endDraw()) && err != COMPV_ERROR_CODE_W_WINDOW_CLOSED, err);

		++count;
	}

bail:
	return NULL;
#endif
}
