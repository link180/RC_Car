#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define IN			0
#define	OUT			1

#define GPIO_MAP_SIZE		0x10000

#define GPIO_DATA_OFFSET	0x00
#define GPIO_TRI_OFFSET		0x04
#define GPIO2_DATA_OFFSET	0x00
#define GPIO2_TRI_OFFSET	0x04

void usage(void)
{
	printf("*argv[0] -d <UIO_DEV_FILE> -i | -o <VALUE>\n");
	printf(" -d UIO device file - ex) /dev/uio0");
	printf(" -i Input from GPIO\n");
	printf(" -o <VALUE> Output to GPIO\n");
}

int main(int argc, char *argv[])
{
	int c, fd, value, direction = IN;
	char *uiod;
	void *ptr;

	printf("GPIO UIO Test\n");

	while((c = getopt(argc, argv, "d:io:h")) != -1)
	{
		switch(c)
		{
			case 'd':
				uiod = optarg;
				break;
			case 'i':
				direction = IN;
				break;
			case 'o':
				direction = OUT;
				value = atoi(optarg);
				break;
			case 'h':
				usage();
				return 0;
			default:
				printf("Invalid Option: %c\n", (char)c);
				usage();
				return -1;
		}
	}

	fd = open(uiod, O_RDWR);

	if(fd < 1)
	{
		perror(argv[0]);
		printf("Invalid UIO Device File: %s\n", uiod);
		usage();
		return -1;
	}

	ptr = mmap(NULL, GPIO_MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(direction == IN)
	{
		*((unsigned *)(ptr + GPIO_TRI_OFFSET)) = 255;
		value = *((unsigned *)(ptr + GPIO_DATA_OFFSET));
		printf("%s: Input: %08x\n", argv[0], value);
	}
	else
	{
		*((unsigned *)(ptr + GPIO_TRI_OFFSET)) = 0;
		*((unsigned *)(ptr + GPIO_DATA_OFFSET)) = value;
	}

	munmap(ptr, GPIO_MAP_SiZE);

	return 0;
}
