#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main()
{
    int fd;
    char Ubuf[] = "Hello! this is data from the writer.";

    fd = open("/dev/IOCTLdevice", O_RDWR, 0777);
    if(fd < 0)
    {
        printf("Error opening Device.\n");
        exit(1);
    }
    
    write(fd, Ubuf, sizeof(Ubuf));
    printf("Data from writer has successfully written into the kernel buffer.\n");

    close(fd);
    return 0;
}

