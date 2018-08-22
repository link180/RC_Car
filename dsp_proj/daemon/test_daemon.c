#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#define FIX_SIZE        10
#define STATUS_SIZE     9

int daemon_init(void)
{
    int i;

    if(fork() > 0)
        exit(0);

    setsid();
    chdir("/");
    umask(0);

    for(i = 0; i < 64; i++)
        close(i);

    signal(SIGCHLD, SIG_IGN);

    return 0;
}

int main(void)
{
    int fd;
    char buf[256] = {0};

    daemon_init();

    fd = open("/home/root/workspace/daemon/config.ini", O_RDWR, 0644);

    read(fd, buf, sizeof(buf));
    lseek(fd, 10, SEEK_SET);
    write(fd, "1\n", 2);

    for(;;)
        ;

    return 0;
}
