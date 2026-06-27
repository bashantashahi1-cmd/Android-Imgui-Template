#include <dobby/dobby.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/log.h>
#include <dlfcn.h>

extern void render_overlay();

typedef EGLBoolean (*eglSwapBuffers_t)(EGLDisplay, EGLSurface);
static eglSwapBuffers_t orig = nullptr;

EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    // init ImGui context once (using the game's window)
    // for brevity, assume ImGui_ImplAndroid_Init has been called elsewhere.
    render_overlay();
    return orig(dpy, surface);
}

__attribute__((constructor))
void hook_init() {
    void* handle = dlopen("libEGL.so", RTLD_LAZY);
    if (!handle) return;
    auto func = (eglSwapBuffers_t)dlsym(handle, "eglSwapBuffers");
    if (func) {
        DobbyHook(func, (void*)hook_eglSwapBuffers, (void**)&orig);
        __android_log_print(ANDROID_LOG_DEBUG, "Hooks", "Hooked eglSwapBuffers");
    }
}
