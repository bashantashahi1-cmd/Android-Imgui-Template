#include <imgui.h>
#include <imgui_impl_android.h>
#include <imgui_impl_opengl3.h>
#include <android/log.h>
#include <atomic>
#include "ioctl_client.h"

extern std::atomic<bool> g_show_overlay;

void* menu_thread(void*) {
    // hooked through eglSwapBuffers – we just wait here
    return nullptr;
}

// called from hooks.cpp after eglSwapBuffers
void render_overlay() {
    if (!g_show_overlay.load()) return;
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 550), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("🥔 Potato Hack", nullptr, ImGuiWindowFlags_NoCollapse)) {
        static bool spin = false;        static int sps = 30;
        static bool drag = true;         static int fov = 120;
        static bool recoil = false;
        static bool speed = false;       static float speed_mult = 1.5f;
        static bool tp = false;          static float tp_dist = 3.0f;
        static bool underground = false;

        if (ImGui::Checkbox("Spinbot", &spin)) send_ioctl(CMD_SPIN_TOGGLE, spin ? 1 : 0);
        ImGui::SameLine();
        if (ImGui::SliderInt("SPS", &sps, 1, 100)) send_ioctl(CMD_SPIN_SPEED, sps);

        if (ImGui::Checkbox("Drag Aimbot", &drag)) send_ioctl(CMD_DRAG_TOGGLE, drag ? 1 : 0);
        ImGui::SameLine();
        if (ImGui::SliderInt("FOV", &fov, 10, 180)) send_ioctl(CMD_DRAG_FOV, fov);

        if (ImGui::Checkbox("No Recoil", &recoil)) send_ioctl(CMD_RECOIL, recoil ? 1 : 0);

        if (ImGui::Checkbox("Speed Hack", &speed)) send_ioctl(CMD_SPEED, speed ? speed_mult : 0.0f);
        ImGui::SameLine();
        if (ImGui::SliderFloat("Mult", &speed_mult, 0.5f, 5.0f)) send_ioctl(CMD_SPEED, speed_mult);

        if (ImGui::Checkbox("TP Forward", &tp)) send_ioctl(CMD_TP_FORWARD, tp ? 1 : 0);
        ImGui::SameLine();
        if (ImGui::SliderFloat("Dist", &tp_dist, 1.0f, 5.0f)) send_ioctl(CMD_TP_DIST, tp_dist);

        if (ImGui::Checkbox("Underground", &underground)) send_ioctl(CMD_UNDERGROUND, underground ? 1 : 0);

        ImGui::Text("3-finger tap to toggle");
    }
    ImGui::End();
}
