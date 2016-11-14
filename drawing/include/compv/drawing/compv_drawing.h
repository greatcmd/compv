/* Copyright (C) 2016-2017 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#if !defined(_COMPV_DRAWING_DRAWING_H_)
#define _COMPV_DRAWING_DRAWING_H_

#include "compv/base/compv_config.h"
#include "compv/base/compv_common.h"
#include "compv/base/compv_obj.h"
#include "compv/base/parallel/compv_mutex.h"
#include "compv/base/parallel/compv_thread.h"
#include "compv/base/android/compv_android_native_activity.h"
#include "compv/drawing/compv_window.h"

COMPV_NAMESPACE_BEGIN()

#if COMPV_OS_ANDROID
struct CompVDrawingAndroidSavedState {
	float angle;
	int32_t x;
	int32_t y;
};
struct CompVDrawingAndroidEngine {
	bool animating;
	struct android_app* app;
	int32_t width;
	int32_t height;
	CompVDrawingAndroidSavedState state;
	struct {
		void *(COMPV_STDCALL *run_fun) (void *);
		void* user_data;
	} worker_thread;
};
#endif /* COMPV_OS_ANDROID */

class COMPV_DRAWING_API CompVDrawing : public CompVObj
{
	friend class CompVWindow;
protected:
	CompVDrawing();
public:
	virtual ~CompVDrawing();
	static COMPV_ERROR_CODE init();
	static COMPV_ERROR_CODE deInit();
	static COMPV_INLINE bool isInitialized() { return s_bInitialized; }
	static COMPV_INLINE bool isLoopRunning() { return s_bLoopRunning; }
	static COMPV_INLINE int getGLVersionMajor() { return s_iGLVersionMajor; }
	static COMPV_INLINE int getGLVersionMinor() { return s_iGLVersionMinor; }
#if COMPV_OS_ANDROID
	static COMPV_INLINE ANativeWindow* getAndroidNativeActivityWindow() { return s_AndroidEngine.app->window; }
#endif

	static size_t windowsCount();
	static COMPV_ERROR_CODE runLoop(void *(COMPV_STDCALL *WorkerThread) (void *) = NULL, void *userData = NULL);
	static COMPV_ERROR_CODE breakLoop();

private:
#if defined(HAVE_SDL_H)
	static COMPV_ERROR_CODE sdl_runLoop();
#endif
#if COMPV_OS_ANDROID
	static int32_t android_engine_handle_input(struct android_app* app, AInputEvent* event);
	static void android_engine_handle_cmd(struct android_app* app, int32_t cmd);
	static COMPV_ERROR_CODE android_runLoop(struct android_app* state);
#endif
	static COMPV_ERROR_CODE registerWindow(CompVPtr<CompVWindow* > window);
	static COMPV_ERROR_CODE unregisterWindow(CompVPtr<CompVWindow* > window);
	static COMPV_ERROR_CODE unregisterWindow(compv_window_id_t windowId);

private:
	static bool s_bInitialized;
	static bool s_bLoopRunning;
	static int s_iGLVersionMajor;
	static int s_iGLVersionMinor;
	COMPV_VS_DISABLE_WARNINGS_BEGIN(4251 4267)
	static std::map<compv_window_id_t, CompVPtr<CompVWindow* > > m_sWindows;
	static CompVPtr<CompVMutex* > s_WindowsMutex;
	static CompVPtr<CompVThread* > s_WorkerThread;
#if COMPV_OS_ANDROID
	static CompVDrawingAndroidEngine s_AndroidEngine;
#endif /* COMPV_OS_ANDROID */

	COMPV_VS_DISABLE_WARNINGS_END()
};

COMPV_NAMESPACE_END()

#endif /* _COMPV_DRAWING_DRAWING_H_ */