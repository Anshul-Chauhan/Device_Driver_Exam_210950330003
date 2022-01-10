obj-m := ioctl_driver.o

KERNELDIR = /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)

default:
		$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
		gcc -o writer writer.c
		gcc -o reader reader.c
		gcc -o ioctl ioctl.c

install:
		$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
		$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
		rm writer reader ioctl
