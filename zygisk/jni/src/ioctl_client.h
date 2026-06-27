#ifndef IOCTL_CLIENT_H
#define IOCTL_CLIENT_H

enum Cmd {
    CMD_SPIN_TOGGLE,
    CMD_SPIN_SPEED,
    CMD_DRAG_TOGGLE,
    CMD_DRAG_FOV,
    CMD_RECOIL,
    CMD_SPEED,
    CMD_TP_FORWARD,
    CMD_UNDERGROUND
};

void send_ioctl(int cmd, int val);
void send_ioctl(int cmd, float val);

#endif
