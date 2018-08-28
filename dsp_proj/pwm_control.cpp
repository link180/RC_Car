#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <termios.h>                   // B115200, CS8 등 상수 정의
#include <fcntl.h>                     // O_RDWR , O_NOCTTY 등의 상수 정의
void serial(char* dev);
int main( void)
{
    char* dev0 ="/dev/ttyUSB0";
    char* dev1 ="/dev/ttyUSB1";
    pid_t pid;

    pid = fork();
    if(pid > 0)
    {
        serial(dev0);
    }
    else if(pid == 0)
    {
        serial(dev1);
    }

   // serial(dev0);
    return 0;
}
void serial(char* dev)
{
    int    fd;
       int    ndx;
       int    cnt;
       char   buf[1024] = {0,};
       char   tx_buf[1024] = {0x2, 0x0, 0x8, 0x40, 0x0, 0x0, 0x0, 0x0, 0x13, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x65, 0x3};
       int tx_cnt;
       struct termios    newtio;
       struct pollfd     poll_events;      // 체크할 event 정보를 갖는 struct
       int    poll_state;

       // 시리얼 포트를 open
       fd = open( dev, O_RDWR | O_NOCTTY | O_NONBLOCK );        // 디바이스를 open 한다.
       if ( 0 > fd)
       {
          printf("%s open error\n", dev);
          exit(-1);
       }

       // 시리얼 포트 통신 환경 설정

       memset( &newtio, 0, sizeof(newtio) );
       newtio.c_cflag       = B921600 | CS8 | CLOCAL | CREAD;
       newtio.c_oflag       = 0;
       newtio.c_lflag       = 0;
       newtio.c_cc[VTIME]   = 0;
       newtio.c_cc[VMIN]    = 1;

       tcflush  (fd, TCIFLUSH );
       tcsetattr(fd, TCSANOW, &newtio );
       fcntl(fd, F_SETFL, FNDELAY);


       // poll 사용을 위한 준비

       poll_events.fd        = fd;
       poll_events.events    = POLLIN | POLLERR;          // 수신된 자료가 있는지, 에러가 있는지
       poll_events.revents   = 0;


       // 자료 송수신

       while ( 1)
       {
          //tx_cnt = write(fd, tx_buf, 18);
          //printf("tx_cnt = %d\n",tx_cnt);
          poll_state = poll(                               // poll()을 호출하여 event 발생 여부 확인
                             (struct pollfd*)&poll_events, // event 등록 변수
                                                       1,  // 체크할 pollfd 개수
                                                    1000   // time out 시간
                           );

          if ( 0 < poll_state)                             // 발생한 event 가 있음
          {
             if ( poll_events.revents & POLLIN)            // event 가 자료 수신?
             {
                cnt = read( fd, buf, 1024);
                buf[cnt-1] = 0x3;
                usleep(1000);

                write( fd, buf, cnt);

                printf( "data received - %d (%s)\n0x ", cnt, dev);
                for(int i = 0;i < cnt; i++)
                    printf("%x ",buf[i]);
                printf("\n");
             }
             if ( poll_events.revents & POLLERR)      // event 가 에러?
             {
                printf( "통신 라인에 에러가 발생, 프로그램 종료");
                break;
             }
          }
       }
       close( fd);

}
