#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(void)
{
    int fd;
    char buf[256] = {0};
    fd = open("a.txt", O_RDONLY);
    close(0);
    dup(fd);
    scanf("%s", buf);
    printf("print it\n");
    printf("buf = %s\n", buf);
    return 0;
}
