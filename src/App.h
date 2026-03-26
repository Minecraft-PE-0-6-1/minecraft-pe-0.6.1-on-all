#pragma once

#include <memory>
#ifdef __APPLE__
#define NO_EGL
#endif
#ifdef STANDALONE_SERVER
#define NO_EGL
#endif

#include <IPlatform.h>
#ifndef NO_EGL 
    #include <EGL/egl.h>
#endif


// typedef struct AppContext {
// #ifndef NO_EGL
// 	EGLDisplay display;
// 	EGLContext context;
// 	EGLSurface surface;
// #endif
// 	AppPlatform* platform;
// 	bool doRender;
// } AppContext;

class App {
protected:
	std::unique_ptr<IPlatform> m_platform;

public:
	static std::unique_ptr<IPlatform> CreatePlatform();

    App(std::unique_ptr<IPlatform> platform) : m_platform(std::move(platform)), m_finished(false), m_inited(false) {}
	App() = delete;
	virtual ~App() {}

	void run();

	bool isInited() { return m_inited; }

    virtual void audioEngineOn () {}
    virtual void audioEngineOff() {}
    
	virtual void destroy() {}

    virtual void loadState(void* state, int stateSize) {}
    virtual bool saveState(void** state, int* stateSize) { return false; }
    
	void swapBuffers();
// 	{
// #ifndef NO_EGL
// 		if (_context.doRender)
// 			eglSwapBuffers(_context.display, _context.surface);
// #endif
// 		m_platform->swapBuffers();
// 	}

	virtual void update() = 0;
	
	virtual void quit() { m_finished = true; }
	virtual bool wantToQuit() { return m_finished; }
	virtual bool handleBack(bool isDown) { return false; }

protected:
	virtual void init() { m_inited = true;}

private:
	bool m_inited = false;
	bool m_finished = false;
};