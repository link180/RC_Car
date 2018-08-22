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

typedef struct sockaddr_in  si; 
typedef struct sockaddr *   sp;

#define BUF_SIZE		32

int cam_proc_id;

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

#if 0
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
#endif

void set_tx_buf(char *buf, char *tx_buf, int prot_num)
{
    int i;
    char temp[16] = {0};
    //printf("buf = %s\n", buf);
    //printf("last = %x\n", buf[4]);

    //for(i = 0; buf[i]; i++)
    //  printf("%d\n", buf[i]);

    memmove(temp, &buf[2], 4);

    if(atoi(temp))
    {
        tx_buf[8] = prot_num;

        tx_buf[9] = buf[3];
        tx_buf[10] = buf[4];
        tx_buf[11] = buf[5];
        tx_buf[12] = buf[6];
    }

    can_calc_crc(tx_buf, 18);
    print_can_arr(tx_buf, 18);
}

void set_servo_tx_buf(char *buf, char *tx_buf, int prot_num)
{
    int i;
    char temp[16] = {0};

    memmove(temp, &buf[2], 3);

    // criteria = 0 ~ 180
    if(atoi(temp))
    {
        tx_buf[8] = prot_num;

        tx_buf[9] = buf[3];
        tx_buf[10] = buf[4];
        tx_buf[11] = buf[5];
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

void set_tx_emergency_buf(char *buf, char *tx_buf, int prot_num)
{
    tx_buf[8] = 13;
    tx_buf[9] = 1;
    tx_buf[10] = 5;
    tx_buf[11] = 0;
    tx_buf[12] = 0;

    can_calc_crc(tx_buf, 18);
    print_can_arr(tx_buf, 18);
}

void term_status(int status)
{
    if(WIFEXITED(status))
        printf("(exit)status : 0x%x\n", WEXITSTATUS(status));
    else if(WTERMSIG(status))
        printf("(signal)status : 0x%x, %s\n", status & 0x7f, WCOREDUMP(status) ? "core dumped" : "");
}

void check_status(int signo)
{
    int status;
    while(waitpid(-1, &status, WNOHANG) > 0)
        term_status(status);
}

int main(int argc, char **argv)
{
    pid_t cam_pid = -1;

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
				    case 0:
				        set_tx_emergency_buf(buf, can_tx_buf, 0);
				        //write(fd, can_tx_buf, 18);
				        printf("(0) Stop Motor\n");
				        memset(&can_tx_buf[8], 0x0, 8);
				        break;
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
						//write(fd, can_tx_buf, 18);
						printf("(7) Left Winker\n");
						//print_can_arr(can_tx_buf, 18);
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
					    set_servo_tx_buf(buf, can_tx_buf, 12);
					    //write(fd, can_tx_buf, 18);
						printf("(12) Specified Angle or PWM Duty(Servo)\n");
						memset(&can_tx_buf[8], 0x0, 8);
						break;
					case 13:
						//set_tx_buf(buf, can_tx_buf);
						set_tx_buf(buf, can_tx_buf, 13);
						//write(fd, can_tx_buf, 18);
						printf("(13) Specified Velocity or PWM Duty\n");
						memset(&can_tx_buf[8], 0x0, 8);
						break;
					case 14:
					    signal(SIGCHLD, check_status);
					    cam_pid = fork();

					    if(cam_pid > 0)
					        cam_proc_id = cam_pid;
					    else if(cam_pid == 0)
					    {
					        printf("(14) Camera On\n");
					        execlp("/home/root/workspace/ocv/cam_record", "cam_record", 0);
					    }

					    break;
					case 15:
					    if(cam_proc_id > 1)
					    {
					        kill(cam_proc_id, 9);
					        cam_proc_id = cam_pid = -1;
					    }
					    printf("(15) Camera Off\n");
					    break;
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

    return 0;
}
