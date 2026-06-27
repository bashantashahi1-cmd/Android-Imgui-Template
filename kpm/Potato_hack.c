// potato_hack.c – Standalone KPM (no KernelPatch headers needed)
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define DEVICE_NAME "potato_hack"
#define CLASS_NAME  "potato"

// KPM metadata (using standard module macros)
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Potato");
MODULE_DESCRIPTION("Kernel hack with ioctl");
MODULE_VERSION("1.0.0");

// ioctl commands
#define POTATO_IOCTL_SET_SPIN        _IOW('p', 1, int)
#define POTATO_IOCTL_SET_SPIN_SPEED  _IOW('p', 2, int)
#define POTATO_IOCTL_SET_DRAG        _IOW('p', 3, int)
#define POTATO_IOCTL_SET_FOV         _IOW('p', 4, int)
#define POTATO_IOCTL_SET_RECOIL      _IOW('p', 5, int)
#define POTATO_IOCTL_SET_SPEED       _IOW('p', 6, float)
#define POTATO_IOCTL_SET_TP_FORWARD  _IOW('p', 7, int)
#define POTATO_IOCTL_SET_TP_DIST     _IOW('p', 8, float)
#define POTATO_IOCTL_SET_UNDERGROUND _IOW('p', 9, int)

static struct hack_config {
    bool spin; int spin_sps;
    bool drag; int fov;
    bool recoil;
    bool spread;
    bool speed; float speed_mult;
    bool tp_forward; float tp_dist;
    bool underground;
} cfg = {0};

static int major_num;
static struct class *cls = NULL;
static struct device *dev = NULL;
static struct task_struct *thread = NULL;

static long potato_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int ival; float fval;
    switch (cmd) {
        case POTATO_IOCTL_SET_SPIN:
            if (copy_from_user(&ival, (void __user *)arg, sizeof(ival))) return -EFAULT;
            cfg.spin = !!ival;
            pr_info("[Potato] Spin = %d\n", cfg.spin);
            break;
        case POTATO_IOCTL_SET_SPIN_SPEED:
            if (copy_from_user(&ival, (void __user *)arg, sizeof(ival))) return -EFAULT;
            cfg.spin_sps = ival;
            break;
        case POTATO_IOCTL_SET_DRAG:
            if (copy_from_user(&ival, (void __user *)arg, sizeof(ival))) return -EFAULT;
            cfg.drag = !!ival;
            break;
        case POTATO_IOCTL_SET_FOV:
            if (copy_from_user(&ival, (void __user *)arg, sizeof(ival))) return -EFAULT;
            cfg.fov = ival;
            break;
        case POTATO_IOCTL_SET_RECOIL:
            if (copy_from_user(&ival, (void __user *)arg, sizeof(ival))) return -EFAULT;
            cfg.recoil = !!ival;
            break;
        case POTATO_IOCTL_SET_SPEED:
            if (copy_from_user(&fval, (void __user *)arg, sizeof(fval))) return -EFAULT;
            cfg.speed = (fval > 0.1f);
            cfg.speed_mult = fval;
            break;
        case POTATO_IOCTL_SET_TP_FORWARD:
            if (copy_from_user(&ival, (void __user *)arg, sizeof(ival))) return -EFAULT;
            cfg.tp_forward = !!ival;
            break;
        case POTATO_IOCTL_SET_TP_DIST:
            if (copy_from_user(&fval, (void __user *)arg, sizeof(fval))) return -EFAULT;
            cfg.tp_dist = fval;
            break;
        case POTATO_IOCTL_SET_UNDERGROUND:
            if (copy_from_user(&ival, (void __user *)arg, sizeof(ival))) return -EFAULT;
            cfg.underground = !!ival;
            break;
        default:
            return -ENOTTY;
    }
    return 0;
}

static struct file_operations fops = {
    .unlocked_ioctl = potato_ioctl,
};

static void write_mem(uintptr_t addr, void *data, size_t len) {
    pr_info("[Potato] Write to 0x%lx len=%zu\n", addr, len);
}

static int hack_thread(void *data) {
    pr_info("[Potato] Hack thread started\n");
    while (!kthread_should_stop()) {
        if (cfg.spin) {
            static float angle = 0;
            angle += cfg.spin_sps * 0.3f;
            if (angle > 360) angle -= 360;
            uintptr_t yaw_addr = 0x796000000 + 0x1A4; // REPLACE WITH REAL OFFSET
            write_mem(yaw_addr, &angle, sizeof(float));
        }
        msleep(50);
    }
    return 0;
}

static int __init potato_init(void) {
    pr_info("[Potato] KPM loading...\n");
    major_num = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_num < 0) return major_num;
    
    cls = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(cls)) {
        unregister_chrdev(major_num, DEVICE_NAME);
        return PTR_ERR(cls);
    }
    
    dev = device_create(cls, NULL, MKDEV(major_num, 0), NULL, DEVICE_NAME);
    if (IS_ERR(dev)) {
        class_destroy(cls);
        unregister_chrdev(major_num, DEVICE_NAME);
        return PTR_ERR(dev);
    }
    
    thread = kthread_run(hack_thread, NULL, "potato_hack");
    if (IS_ERR(thread)) {
        device_destroy(cls, MKDEV(major_num, 0));
        class_destroy(cls);
        unregister_chrdev(major_num, DEVICE_NAME);
        return PTR_ERR(thread);
    }
    
    pr_info("[Potato] KPM loaded (major=%d)\n", major_num);
    return 0;
}

static void __exit potato_exit(void) {
    pr_info("[Potato] KPM unloading...\n");
    if (thread) kthread_stop(thread);
    device_destroy(cls, MKDEV(major_num, 0));
    class_destroy(cls);
    unregister_chrdev(major_num, DEVICE_NAME);
}

module_init(potato_init);
module_exit(potato_exit);
