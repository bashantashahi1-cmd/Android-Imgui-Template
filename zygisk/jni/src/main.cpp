#include <zygisk.hpp>
#include <android/log.h>
#include <pthread.h>
#include <string>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "FFMod", __VA_ARGS__)

using zygisk::Api;
using zygisk::AppSpecializeArgs;

extern void* menu_thread(void*);
extern void gesture_loop();

class ModModule : public zygisk::ModuleBase {
public:
    void onLoad(Api* api, JNIEnv* env) override { this->api = api; this->env = env; LOGD("Loaded"); }
    void onAppSpecialize(AppSpecializeArgs* args) override {
        auto& info = args->app_data_dir;
        if (info) {
            std::string dir = info->get();
            if (dir.find("com.dts.freefireth") != std::string::npos ||
                dir.find("com.dts.freefiremax") != std::string::npos) {
                LOGD("Target: %s", dir.c_str());
                pthread_t t;
                pthread_create(&t, nullptr, menu_thread, nullptr);
                pthread_detach(t);
                // gesture thread
                pthread_create(&t, nullptr, [](void*) -> void* { gesture_loop(); return nullptr; }, nullptr);
                pthread_detach(t);
            }
        }
    }
private:
    Api* api;
    JNIEnv* env;
};

REGISTER_ZYGISK_MODULE(ModModule)
