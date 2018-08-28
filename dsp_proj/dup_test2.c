#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(void)
{
    int fd;
    char buf[256] = {0};
    fd = open("a.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    close(1);
    dup(fd);
    system("ps -ef | grep init | grep -v grep | awk '{print $2}'");
    return 0;
}
