#include <linux/module.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux{DEVICE_NAME}.h>
#include <linux/gfp.h>
#include <linux/fdtable.h>

#define DEVICE_NAME "ori_dev"
#define DEVICE_PATH "/dev/ori_dev"
#define DEVICE_SIZE PAGE_SIZE

static struct cdev ori_cdev;
static dev_t dev_num;
static struct class *ori_class;
static struct file *swap_file;  // `/dev/{DEVICE_NAME}` を開くファイル構造体

// ページフォルトハンドラ
static vm_fault_t ori_fault(struct vm_fault *vmf) {
    struct page *page;

    // 新しいページを確保
    page = alloc_page(GFP_KERNEL);
    if (!page) return VM_FAULT_OOM;

    // 物理ページをユーザープロセスの仮想アドレスに対応付ける
    vmf->page = page;

    return 0;
}

// ページアウト時に `/dev{DEVICE_NAME}` へ書き込む
static int ori_page_mkwrite(struct vm_area_struct *vma, struct vm_fault *vmf) {
    loff_t pos = vmf->pgoff * PAGE_SIZE;
    struct page *page = vmf->page;
    void *page_data;

    if (!swap_file) return VM_FAULT_SIGBUS;

    // ページのデータをカーネル空間にマップ
    page_data = kmap(page);

    // `/dev{DEVICE_NAME}` に書き込み
    kernel_write(swap_file, page_data, PAGE_SIZE, &pos);

    kunmap(page);

    return 0;
}

// `vm_operations_struct` の定義
static const struct vm_operations_struct ori_vm_ops = {
    .fault = ori_fault,
    .page_mkwrite = ori_page_mkwrite,
};

// `mmap()` のハンドラ
static int ori_mmap(struct file *file, struct vm_area_struct *vma) {
    vma->vm_ops = &ori_vm_ops;
    return 0;
}

// `file_operations`
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .mmap = ori_mmap,
};

// デバイス初期化
static int __init ori_init(void) {
    int ret;

    // `/dev{DEVICE_NAME}` をオープン（書き込み用）
    swap_file = filp_open(DEVICE_PATH, O_RDWR | O_CREAT, 0644);
    if (IS_ERR(swap_file)) {
        printk(KERN_ERR "Failed to open %s \n", DEVICE_PATH);
        return PTR_ERR(swap_file);
    }

    // キャラクタデバイスの登録
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) return ret;

    cdev_init(&ori_cdev, &fops);
    ret = cdev_add(&ori_cdev, dev_num, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    ori_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(ori_class)) {
        cdev_del(&ori_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(ori_class);
    }

    if (!device_create(ori_class, NULL, dev_num, NULL, DEVICE_NAME)) {
        class_destroy(ori_class);
        cdev_del(&ori_cdev);
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    printk(KERN_INFO "ori_dev module loaded\n");
    return 0;
}

// デバイス終了処理
static void __exit ori_exit(void) {
    if (swap_file)
        filp_close(swap_file, NULL);

    device_destroy(ori_class, dev_num);
    class_destroy(ori_class);
    cdev_del(&ori_cdev);
    unregister_chrdev_region(dev_num, 1);

    printk(KERN_INFO "ori_dev module unloaded\n");
}

module_init(ori_init);
module_exit(ori_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A page-out triggered device driver");
