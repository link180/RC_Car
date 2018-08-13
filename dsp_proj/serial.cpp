/*
 * helloworld.c
 *
 *  Created on: Jul 16, 2018
 *      Author: howard
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <termios.h>
#include <fcntl.h>

int main(void)
{
    int fd;
    int ndx;
    int cnt;
    char buf[1024] = {0};
    struct termios newtio;
    struct pollfd   poll_events;
    int poll_state;

    fd = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY);
    if( 0 > fd )
    {
        printf("open error\n");
        return -1;
    }

    printf("fd = %d\n", fd);

    memset( &newtio, 0 , sizeof(newtio));

    newtio.c_cflag   = B9600 | CS8 | CLOCAL | CREAD;
    newtio.c_oflag  = 0;
    newtio.c_lflag  = 0;
    newtio.c_cc[VTIME]  = 0;
    newtio.c_cc[VMIN]   = 1;

    poll_events.fd  = fd;
    poll_events.events = POLLIN | POLLERR;
    poll_events.revents= 0;

    while(1)
    {
        poll_state = poll(
                        (struct pollfd*)&poll_events,
                        1,
                        1000
        );
        printf("poll_state = %d\n", poll_state);
        if(0 < poll_state)
        {
            if(poll_events.revents & POLLIN)
            {
                cnt = read(fd, buf, 1024);
                write(fd, buf, cnt);
                printf("data received - %d %s\n",cnt, buf);
                memset(buf, 0x0, sizeof(buf));
            }
            if(poll_events.revents & POLLERR)
            {
                printf("ERROR!! exit!");
                break;

            }
        }

    }
    close(fd);

    return 0;
}
