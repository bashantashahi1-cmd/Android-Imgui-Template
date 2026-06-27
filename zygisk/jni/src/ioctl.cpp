#include "ioctl_client.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <android/log.h>
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "Ioctl", __VA_ARGS__)

static int fd = -1;
static void open_dev() {
    if (fd >= 0) return;
    fd = open("/dev/potato_hack", O_RDWR);
    if (fd < 0) LOGD("open failed");
}

void send_ioctl(int cmd, int val) {
    open_dev();
    if (fd < 0) return;
    unsigned long c = 0;
    switch (cmd) {
        case CMD_SPIN_TOGGLE: c = _IOW('p', 1, int); break;
        case CMD_SPIN_SPEED:  c = _IOW('p', 2, int); break;
        case CMD_DRAG_TOGGLE: c = _IOW('p', 3, int); break;
        case CMD_DRAG_FOV:    c = _IOW('p', 4, int); break;
        case CMD_RECOIL:      c = _IOW('p', 5, int); break;
        case CMD_TP_FORWARD:  c = _IOW('p', 7, int); break;
        case CMD_UNDERGROUND: c = _IOW('p', 9, int); break;
        case CMD_TP_ENEMY:    c = _IOW('p',10, int); break;
        default: return;
    }
    ioctl(fd, c, &val);
}

void send_ioctl(int cmd, float val) {
    open_dev();
    if (fd < 0) return;
    if (cmd == CMD_SPEED) ioctl(fd, _IOW('p', 6, float), &val);
    else if (cmd == CMD_TP_DIST) ioctl(fd, _IOW('p', 8, float), &val);
}
