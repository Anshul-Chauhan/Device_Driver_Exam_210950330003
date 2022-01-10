#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <asm/ioctl.h>
#include "ioctl.h"

struct stats
{
    int size;
    int buff[50];
    int r_w;
};

int main()
{
    int fd, result;
    struct stats status;
    fd = open("/dev/IOCTLdevice", O_RDWR, 0777);
    if(fd < 0)
    {
        printf("Error opening device.\n");
        exit(1);
    }

    result = ioctl(fd, GETSTATS, &status);
    if(result < 0)
    {
        printf("IOCTL error.\n");
        return (-1);
    }
    printf("size: %d\n", status.size);
    printf("Buffer data: %d\n", status.buff);
    printf("Read write status: %d\n", status.r_w);

    close(fd);
    return 0;
}