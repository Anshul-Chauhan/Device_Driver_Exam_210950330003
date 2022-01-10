// Initialisation functions
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include "ioctl.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ANSHUL");

#define NAME IOCTLdevice

int NAME_open(struct inode *inode, struct file *filp);
int NAME_close(struct inode *inode, struct file *filp);
ssize_t NAME_write(struct file *filp, const char __user *Ubuf, size_t count, loff_t *f_pos);
ssize_t NAME_read(struct file *filp, char __user *Ubuf, size_t count, loff_t *f_pos);
long NAME_ioctl (struct file *filp, unsigned int command, unsigned long arg);

struct stats
{
    int size;
    char buff[50];
    int r_w;
};

struct stats ioctl;

struct file_operations fops =
{
    .open = NAME_open,
    .read = NAME_read,
    .write = NAME_write,
    .unlocked_ioctl = NAME_ioctl,
    .release = NAME_close,
};

struct cdev *my_cdev;

static int __init CharDevice_init(void)
{
    int result;
    int MAJOR, MINOR;
    dev_t DevNo;
    DevNo = MKDEV(259, 0); 
    MAJOR = MAJOR(DevNo);
    MINOR = MINOR(DevNo);
    printk(KERN_ALERT "The Major number is %d and the Minor number is %d.\n", MAJOR, MINOR);
    result = register_chrdev_region(DevNo, 1, "IOCTLdevice"); 
    if(result < 0)
    {
        printk(KERN_ALERT "Requested region is not obtainable.\n");
        return (-1);
    }

    my_cdev = cdev_alloc(); 
    my_cdev->ops = &fops;   

    result = cdev_add(my_cdev, DevNo, 1); 
    if(result < 0)
    {
        printk(KERN_ALERT "The char device has not been created.\n");
        unregister_chrdev_region(DevNo, 1);
        return (-1);
    }
    return 0;
}

void __exit CharDevice_exit(void)
{
    int MAJOR, MINOR;
    dev_t DevNo;
    DevNo = MKDEV(259, 0); 
    MAJOR = MAJOR(DevNo);
    MINOR = MINOR(DevNo);
    printk(KERN_ALERT "The Major number is %d and the Minor number is %d.\n", MAJOR, MINOR);
    unregister_chrdev_region(DevNo, 1); 
    cdev_del(my_cdev);
    printk(KERN_ALERT "All the resources which were allocated have been unregistered.\n");
    return;
}

int NAME_open(struct inode *inode, struct file *filp)
{
    printk(KERN_ALERT "In kernel open system call has been executed.\n");
    return 0;
}

int NAME_close(struct inode *inode, struct file *filp)
{
    printk(KERN_ALERT "In kernel release system call has been executed.\n");
    return 0;
}

// read functionality
ssize_t NAME_read(struct file *filp, char __user *Ubuf, size_t count, loff_t *f_pos)
{
    unsigned long result;
    ssize_t retval;
    result = copy_to_user((char *)Ubuf, (char *)ioctl.buff, sizeof(ioctl.buff));
    ioctl.r_w = 0;
    ioctl.size = sizeof(ioctl.buff); 
    if(result == 0)
    {
        printk(KERN_ALERT "Data successfully read.\n");
        retval = count;
        return retval;
    }
    else if(result > 0)
    {
        printk(KERN_ALERT "Partial data read.\n");
        retval = (count-result);
        return retval;
    }
    else
    {
        printk(KERN_ALERT "Error writing data to user.\n");
        retval = -EFAULT;
        return retval;
    }
}

// write functionality
ssize_t NAME_write(struct file *filp, const char __user *Ubuf, size_t count, loff_t *f_pos)
{
    unsigned long result;
    ssize_t retval;
    result = copy_from_user((char *)ioctl.buff, (char *)Ubuf, count);
    ioctl.r_w = 1; 
    ioctl.size = sizeof(ioctl.buff);
    if(result == 0)
    {
        printk(KERN_ALERT "Message from the user:  \n'%s'\n", ioctl.buff);
        printk(KERN_ALERT "Data successfully written.\n");
        retval = count;
        return retval;
    }
    else if(result > 0)
    {
        printk(KERN_ALERT "Message from the user:  \n'%s'\n", ioctl.buff);
        printk(KERN_ALERT "Partial data written.\n");
        retval = (count-result);
        return retval;
    }
    else
    {
        printk(KERN_ALERT "Error writing data to kernel.\n");
        retval = -EFAULT;
        return retval;
    }
}

long NAME_ioctl (struct file *filp, unsigned int command, unsigned long arg)
{
    unsigned int temp;
    char Ubuf[20];
    struct stats *status;
    printk("IOCTL function.\n");

    return 0;
}

module_init(CharDevice_init);
module_exit(CharDevice_exit);