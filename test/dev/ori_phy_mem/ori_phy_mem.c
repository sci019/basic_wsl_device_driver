#include <linux/module.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "ori_dev"
#define DEVICE_SIZE PAGE_SIZE

static char device_buffer[DEVICE_SIZE];
static struct cdev ori_cdev;
static dev_t dev_number;

static int ori_open(struct inode *inode, struct file *file) {
    return 0;
}

static int ori_release(struct inode *inode, struct file *file) {
    return 0;
}

static int ori_mmap(struct file *file, struct vm_area_struct *vma) {
    unsigned long pfn = page_to_pfn(virt_to_page(device_buffer));
    return remap_pfn_range(vma, vma->vm_start, pfn, DEVICE_SIZE, vma->vm_page_prot);
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = ori_open,
    .release = ori_release,
    .mmap = ori_mmap,
};

static struct class *ori_class;

static int __init ori_init(void) {
    int ret;
    ret = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "Failed to allocate char device region\n");
        return ret;
    }
    cdev_init(&ori_cdev, &fops);
    ori_cdev.owner = THIS_MODULE;
    ret = cdev_add(&ori_cdev, dev_number, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev_number, 1);
        printk(KERN_ERR "Failed to add char device\n");
        return ret;
    }
    
    ori_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(ori_class)) {
        cdev_del(&ori_cdev);
        unregister_chrdev_region(dev_number, 1);
        printk(KERN_ERR "Failed to create device class\n");
        return PTR_ERR(ori_class);
    }

    if (!device_create(ori_class, NULL, dev_number, NULL, DEVICE_NAME)) {
        class_destroy(ori_class);
        cdev_del(&ori_cdev);
        unregister_chrdev_region(dev_number, 1);
        printk(KERN_ERR "Failed to create device\n");
        return -1;
    }

    printk(KERN_INFO "ori_dev module loaded\n");
    return 0;
}

static void __exit ori_exit(void) {
    device_destroy(ori_class, dev_number);
    class_destroy(ori_class);
    cdev_del(&ori_cdev);
    unregister_chrdev_region(dev_number, 1);
    printk(KERN_INFO "ori_dev module unloaded\n");
}

module_init(ori_init);
module_exit(ori_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple mmap-capable device driver");
