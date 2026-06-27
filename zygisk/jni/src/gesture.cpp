#include <thread>
#include <atomic>
#include <android/log.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

extern std::atomic<bool> g_show_overlay;

void gesture_loop() {
    // find touch device
    char dev_path[256];
    int found = 0;
    for (int i = 0; i < 20; i++) {
        snprintf(dev_path, sizeof(dev_path), "/dev/input/event%d", i);
        int fd = open(dev_path, O_RDONLY | O_NONBLOCK);
        if (fd < 0) continue;
        close(fd);
        // simplified: assume first event is touch (you can check with getevent -i)
        found = 1;
        break;
    }
    if (!found) return;
    // For demo, we use a simple file-based toggle (you can expand to real getevent)
    while (true) {
        // check if /data/local/tmp/ff_gesture_3 exists
        if (access("/data/local/tmp/ff_gesture_3", F_OK) == 0) {
            unlink("/data/local/tmp/ff_gesture_3");
            g_show_overlay = !g_show_overlay.load();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}
