#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <termios.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int fd, ndx, cnt, poll_state;
	char buf[128] = {0};
	struct termios newtio;
	struct pollfd poll_events;

	if(argc != 2)
	{
		printf("Usage: exe /dev/ttyUSBx\n");
		exit(-1);
	}

	fd = open(argv[1], O_RDWR | O_NOCTTY);

	if(fd < 0)
	{
		printf("Open Error\n");
		exit(-1);
	}

	printf("fd = %d\n", fd);

	memset(&newtio, 0, sizeof(newtio));

	newtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 1;

	poll_events.fd = fd;
	poll_events.events = POLLIN | POLLERR;
	poll_events.revents = 0;

	for(;;)
	{
		poll_state = poll((struct pollfd *)&poll_events, 1, 1000);
		printf("poll_state = %d\n", poll_state);

		if(poll_state > 0)
		{
			if(poll_events.revents & POLLIN)
			{
				cnt = read(fd, buf, 128);
				write(fd, buf, cnt);
				printf("Received Data(%d) : %s\n", cnt, buf);
				memset(buf, 0x0, sizeof(buf));
			}
			if(poll_events.revents & POLLERR)
			{
				printf("Error Occurance!!!\n");
				break;
			}
		}
	}

	close(fd);

	return 0;
}
