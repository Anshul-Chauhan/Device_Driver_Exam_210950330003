#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/semaphore.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ANSHUL");

#define NAME SemphCharDevice
char Kbuf[50];  // shared resourse
int condition = 0;
int MAJOR, MINOR;
dev_t DevNo;

struct semaphore semph;
static wait_queue_head_t MyWaitQueue;

int NAME_open(struct inode *inode, struct file *filp);
int NAME_release(struct inode *inode, struct file *filp);
ssize_t NAME_write(struct file *filp, const char __user *Ubuf, size_t count, loff_t *f_pos);
ssize_t NAME_read(struct file *filp, char __user *Ubuf, size_t count, loff_t *f_pos);

struct file_operations fops =
{
    .open = NAME_open,
    .read = NAME_read,
    .write = NAME_write,
    .release = NAME_release,
};

struct cdev *my_cdev;

static int __init CharDevice_init(void)
{
    int result;
    result = alloc_chrdev_region(&DevNo, 0, 2, "SemphCharDevice");
    if(result < 0)
    {
        printk(KERN_ALERT "Requested region is not obtainable.\n");
        return (-1);
    }
    MAJOR = MAJOR(DevNo);
    MINOR = MINOR(DevNo);
    printk(KERN_ALERT "The Major number is %d and the Minor number is %d.\n", MAJOR, MINOR);
    my_cdev = cdev_alloc(); 
    my_cdev->ops = &fops; 

    result = cdev_add(my_cdev, DevNo, 2); 
    if(result < 0)
    {
        printk(KERN_ALERT "The char device has not been created.\n");
        unregister_chrdev_region(DevNo, 2);
        return (-1);
    }

    sema_init(&semph, 1);              // Initialising the semaphore
    init_waitqueue_head(&MyWaitQueue); // Initialising wait queue
    return 0;
}

void __exit CharDevice_exit(void)
{
    unregister_chrdev_region(DevNo, 2); 
    cdev_del(my_cdev);
    printk(KERN_ALERT "All the resources which were allocated have been unregistered.\n");
    return;
}

int NAME_open(struct inode *inode, struct file *filp)
{
    if(MINOR(inode->i_rdev) == 0)
    {
        printk(KERN_ALERT "Device 1: In kernel open system call has been executed.\n");
        return 0;
    }
    else if(MINOR(inode->i_rdev) == 1)
    {
        printk(KERN_ALERT "Device 2: In kernel open system call has been executed.\n");
        return 0;
    }
    else 
    {
        return (-1);
    }
}

int NAME_release(struct inode *inode, struct file *filp)
{
    printk(KERN_ALERT "In kernel release system call has been executed.\n");
    return 0;
}

ssize_t NAME_read(struct file *filp, char __user *Ubuf, size_t count, loff_t *f_pos)
{
    unsigned long result;
    ssize_t retval;
    result = copy_to_user((char *)Ubuf, (char *)Kbuf, sizeof(Kbuf)); 
    condition = 0;
    wake_up(&MyWaitQueue);
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

ssize_t NAME_write(struct file *filp, const char __user *Ubuf, size_t count, loff_t *f_pos)
{
    unsigned long result;
    ssize_t retval;
    down(&semph);
    wait_event(MyWaitQueue, condition == 0);
    result = copy_from_user((char *)Kbuf, (char *)Ubuf, count); 
    condition = 1;
    up(&semph);
    printk("Releasing semaphore.\n");
    if(result == 0)
    {
        printk(KERN_ALERT "Message from the user:  \n'%s'\n", Kbuf);
        printk(KERN_ALERT "Data successfully written.\n");
        retval = count;
        return retval;
    }
    else if(result > 0)
    {
        printk(KERN_ALERT "Message from the user:  \n'%s'\n", Kbuf);
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

module_init(CharDevice_init);
module_exit(CharDevice_exit);