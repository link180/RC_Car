#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <signal.h>
#include <sys/wait.h>

typedef struct sockaddr_in  si; 
typedef struct sockaddr *   sp;

#define BUF_SIZE		32

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

int main(int argc, char **argv)
{
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

            while((len = read(clnt_sock, (char *)&buf, BUF_SIZE)) != 0)
            {
#if 0
				if(atoi(&buf[0]) == 1)
					printf("Turn Left\n");
				else
					printf("input = %s\n", buf);
#endif
				printf("buf = %s\n", buf);

				switch(atoi(&buf[0]))
				{
					case 1:
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
						printf("(7) Left Winker\n");
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
						printf("(13) Specified Angle or PWM Duty(Servo)\n");
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
