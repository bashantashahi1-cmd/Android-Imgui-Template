LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ffmod
LOCAL_SRC_FILES := src/main.cpp src/menu.cpp src/ioctl_client.cpp src/gesture.cpp src/hooks.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src/imgui $(LOCAL_PATH)/src/imgui/backends $(LOCAL_PATH)/src/dobby
LOCAL_STATIC_LIBRARIES := imgui android_native_app_glue
LOCAL_LDLIBS := -landroid -lEGL -lGLESv2 -llog -lz
LOCAL_CPPFLAGS := -std=c++17 -fno-rtti -fno-exceptions -O2
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := imgui
LOCAL_SRC_FILES := src/imgui/imgui.cpp src/imgui/imgui_draw.cpp src/imgui/imgui_widgets.cpp src/imgui/imgui_tables.cpp src/imgui/backends/imgui_impl_android.cpp src/imgui/backends/imgui_impl_opengl3.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src/imgui $(LOCAL_PATH)/src/imgui/backends
LOCAL_CPPFLAGS := -std=c++17 -O2
include $(BUILD_STATIC_LIBRARY)
