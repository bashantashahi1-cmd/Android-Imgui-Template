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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Potato");
MODULE_DESCRIPTION("Kernel hack with ioctl");
MODULE_VERSION("1.0.0");

#define POTATO_IOCTL_SET_SPIN        _IOW('p', 1, int)
#define POTATO_IOCTL_SET_SPIN_SPEED  _IOW('p', 2, int)

static struct hack_config {
    bool spin;
    int spin_sps;
} cfg = {0};

static int major_num;
static struct class *cls = NULL;
static struct device *dev = NULL;
static struct task_struct *thread = NULL;

static long potato_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int ival;
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
        default:
            return -ENOTTY;
    }
    return 0;
}

static struct file_operations fops = {
    .unlocked_ioctl = potato_ioctl,
};

static int hack_thread(void *data) {
    pr_info("[Potato] Hack thread started\n");
    while (!kthread_should_stop()) {
        if (cfg.spin) {
            static float angle = 0;
            angle += cfg.spin_sps * 0.3f;
            if (angle > 360) angle -= 360;
            pr_info("[Potato] Spin angle: %f\n", angle);
        }
        msleep(100);
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
