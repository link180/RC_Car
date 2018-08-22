#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <signal.h>
#include <sys/wait.h>

#include <sys/stat.h>
#include <errno.h>

#include <linux/unistd.h>
#include <semaphore.h>

typedef struct sockaddr_in  si; 
typedef struct sockaddr *   sp;

#define BUF_SIZE		32

int proc_num;
sem_t bin_sem;

void term_status(int status)
{
    if(WIFEXITED(status))
        printf("(exit)status: 0x%x\n", WEXITSTATUS(status));
    else if(WTERMSIG(status))
        printf("(signal)status: 0x%x, %s\n", status & 0x7f, WCOREDUMP(status) ? "core dumped" : "");
}

void check_daemon(int signo)
{
    int status;

    while(waitpid(-1, &status, WNOHANG) > 0)
        term_status(status);
}

int make_daemon_monitor(void)
{
    int tid = syscall(__NR_gettid);

    printf("tid = %d\n", tid);
    pid_t pid = fork();

    if(pid > 0)
    {
        signal(SIGCHLD, check_daemon);
        printf("Daemon Monitor Process(Cell Phone Wi-Fi) On\n");
        for(;;)
        {
            printf("Monitor(parent)\n");
            usleep(500000);
        }
    }
    else if(pid == 0)
    {
        execlp("/home/root/workspace/daemon/test_daemon", "test_daemon", 0);
        //execlp("/home/root/workspace/ocv/cam_record", "cam_record", 0);
    }
    else
    {
        perror("fork()");
        exit(-1);
    }

    return 0;
}

void err_handler(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

void read_cproc(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("Removed proc id: %d\n", pid);
}

void print_can_arr(char *buf, int num)
{
	int i;

	printf("can buf = ");

	for(i = 0; i < num; i++)
		printf("%3x", buf[i]);

	printf("\n");
}

void can_calc_crc(char *buf, int num)
{
	int i, sum = 0;

	for(i = 2; i < num - 2; i++)
		sum += buf[i];

	buf[16] = sum;
}

void set_tx_buf(char *buf, char *tx_buf, int prot_num)
{
	int i;
	char temp[16] = {0};
	//printf("buf = %s\n", buf);
	//printf("last = %x\n", buf[4]);

	//for(i = 0; buf[i]; i++)
	//	printf("%d\n", buf[i]);

	memmove(temp, &buf[3], 1);

	tx_buf[9] = 1;

	if(atoi(temp))
	{
		tx_buf[8] = prot_num;
		tx_buf[10] = atoi(temp);
	}

	can_calc_crc(tx_buf, 18);
	print_can_arr(tx_buf, 18);
}

void set_tx_winker_buf(char *buf, char *tx_buf, int prot_num)
{
	int i;
	char temp[16] = {0};

	memmove(temp, &buf[2], 1);

	tx_buf[8] = prot_num;
	tx_buf[9] = atoi(temp);

	can_calc_crc(tx_buf, 18);
	print_can_arr(tx_buf, 18);
}

int main(int argc, char **argv)
{
    make_daemon_monitor();

    for(;;)
        ;

    return 0;
}

#if 0
int main(int argc, char **argv)
{
	char *dev = "/dev/ttyUSB0";

	int i, fd, cnt = 0;
	char can_buf[BUF_SIZE] = {0};
	char temp[16] = {0};
	char can_tx_buf[BUF_SIZE] = {0x2, 0x0, 0x8, 0x40, 0x1, 0x0, 0x0, 0x0,
				 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3};
	struct termios newtio;
	struct pollfd poll_events;
	int poll_state;

    int serv_sock, clnt_sock, len, state;
    char buf[BUF_SIZE] = {0};

	char tx_buf[BUF_SIZE] = "Success\n";
	int test_len = strlen(tx_buf);

    si serv_addr, clnt_addr;
    struct sigaction act;
    socklen_t addr_size;
    pid_t pid;
    
    if(argc != 2)
    {   
        printf("use: %s <port>\n", argv[0]);
        exit(1);
    }

	fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
	//fd = open(dev, O_RDWR | O_NOCTTY);

	printf("fd = %d\n", fd);

	if(fd < 0)
	{
		printf("Open Error\n");
		exit(-1);
	}

	memset(&newtio, 0, sizeof(newtio));
	newtio.c_cflag = B921600 | CS8 | CLOCAL | CREAD;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 1;

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);
	fcntl(fd, F_SETFL, FNDELAY);

	poll_events.fd = fd;
	poll_events.events = POLLIN | POLLERR;
	poll_events.revents = 0;

    act.sa_handler = read_cproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0);

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    if(serv_sock == -1)
        err_handler("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (sp)&serv_addr, sizeof(serv_addr)) == -1)
        err_handler("bind() error");

	if(listen(serv_sock, 5) == -1)
        err_handler("listen() error");

    for(;;)
    {
        addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (sp)&clnt_addr, &addr_size);

        if(clnt_sock == -1)
            continue;
        else
            puts("New Client Connected!\n");

        pid = fork();

        if(pid == -1)
        {
            close(clnt_sock);
            continue;
        }

        if(!pid)
        {
            close(serv_sock);

	    read(fd, can_buf, 18);
	    //printf("can_buf = %s\n", can_buf);

            while((len = read(clnt_sock, (char *)&buf, BUF_SIZE)) != 0)
            {
#if 0
				if(atoi(&buf[0]) == 1)
					printf("Turn Left\n");
				else
					printf("input = %s\n", buf);
#endif
				memmove(temp, buf, 2);
				printf("temp = %s\n", temp);

				switch(atoi(temp))
				{
					case 1:
						//write(fd, buf[0], 1);
						//write(fd, tx_buf, 18);
						printf("(1) Turn Left\n");
						break;
					case 2:
						printf("(2) Turn Right\n");
						break;
					case 3:
						printf("(3) Go Straight\n");
						break;
					case 4:
						printf("(4) Go backward\n");
						break;
					case 5:
						printf("(5) Stop\n");
						break;
					case 6:
						printf("(6) Collision Warn\n");
						break;
					case 7:
						set_tx_winker_buf(buf, can_tx_buf, 7);
						write(fd, can_tx_buf, 18);
						printf("(7) Left Winker\n");
						print_can_arr(can_tx_buf, 18);
						memset(&can_tx_buf[8], 0x0, 8);
						break;
					case 8:
						printf("(8) Right Winker\n");
						break;
					case 9:
						printf("(9) Head Light\n");
						break;
					case 10:
						printf("(10) DSP Image Meta Data\n");
						break;
					case 11:
						printf("(11) FPGA Lidar Meta Data\n");
						break;
					case 12:
						printf("(12) Specified Velocity or PWM Duty\n");
						break;
					case 13:
						//set_tx_buf(buf, can_tx_buf);
						set_tx_buf(buf, can_tx_buf, 13);
						write(fd, can_tx_buf, 18);
						printf("(13) Specified Angle or PWM Duty(Servo)\n");
						memset(&can_tx_buf[8], 0x0, 8);
						break;
				}

				poll_state = poll((struct pollfd *)&poll_events, 1, 1000);
				if(poll_state > 0)
				{
					if(poll_events.revents & POLLIN)
					{
						cnt = read(fd, can_buf, BUF_SIZE);
						can_buf[cnt - 1] = '\0';
						usleep(1000);
						printf("data received - %d (%s)\n", cnt, dev);

						for(i = 0; i < cnt; i++)
							printf("%x ", can_buf[i]);
						printf("\n");
					}
					if(poll_events.revents & POLLERR)
					{
						printf("Communication Error\n");
						break;
					}
				}

				memset(buf, 0x0, sizeof(buf));

                write(clnt_sock, (char *)&tx_buf, test_len);
            }

            close(clnt_sock);
            puts("Client Disconnected!\n");
            return 0;
        }
        else
            close(clnt_sock);
    }
    close(serv_sock);
	close(fd);

    return 0;
}
#endif
