#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <wiringPi.h>

#define CLIENT_IP "192.168.1.7"
#define CLIENT_PORT 9999
#define MAXTIMINGS 83
#define DHTPIN 7
#define LED1 4
#define LED2 5
#define LED3 6

int dht11_dat[5] = {0, };

void read_dht11_dat(){

  uint8_t laststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0, i;
  uint8_t flag = HIGH;
  uint8_t state = 0;
  float f;

  dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

  pinMode(DHTPIN, OUTPUT);
  digitalWrite(DHTPIN, LOW);
  delay(18);

  digitalWrite(DHTPIN, HIGH);
  delayMicroseconds(38);
  digitalWrite(DHTPIN, INPUT);

  pinMode (LED1, OUTPUT);
  pinMode (LED2, OUTPUT);
  pinMode (LED3, OUTPUT);

  for(i=0; i<MAXTIMINGS; i++){
    counter = 0;
    while(digitalRead(DHTPIN) == laststate){
      counter++;
      delayMicroseconds(1);
      if(counter==200) break;
    }

    laststate = digitalRead(DHTPIN);
    if(counter == 200) break;
    if((i>=4) && (i%2==0)){
      dht11_dat[j/8] <<= 1;
      if(counter>24) dht11_dat[j/8]|=1;
      j++;
    }
  }
  if((j>=40) && (dht11_dat[4] == (dht11_dat[0]+dht11_dat[1]+dht11_dat[2]+dht11_dat[3]&0xff))){
    printf("humidity = %d.%d%% Temperature = %d.%d*C \n", dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3]);
  }
  else printf("Data get failed \n");

  if(dht11_dat[2] >= 23){
    digitalWrite(LED1, 1);
    digitalWrite(LED2, 0);
    digitalWrite(LED3, 0);
  }
  else if(dht11_dat[2] > 0 && dht11_dat[2]<= 22){
    digitalWrite(LED1, 0);
    digitalWrite(LED2, 0);
    digitalWrite(LED3, 1);
  }
  else{
    digitalWrite(LED1, 1);
    digitalWrite(LED2, 1);
    digitalWrite(LED3, 1);
    delayMicroseconds(40);
  }
}

int main(int argc, char **argv){

  int client_socket;
  struct sockaddr_in server_address;
  unsigned char Buff[250];
  int check;

  printf("dht11 RaspberryPi \n");
  if(wiringPiSetup() == -1) exit(1);

  client_socket = socket(PF_INET,SOCK_STREAM,0);

  if (client_socket == -1){
      printf("Client Socket ERROR");
      exit(0);
    }

  bzero((char *)&server_address, sizeof(server_address));

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(CLIENT_PORT);
  server_address.sin_addr.s_addr = inet_addr(CLIENT_IP);

  if(connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1){
      printf("Connect ERROR");
      exit(0);
  }

  printf("\nclient socket = [%d]\n\n",client_socket);


  while(1){

    read_dht11_dat();
    delay(500);

    write(client_socket, argv[1], dht11_dat[2]);
    printf("asd = %d, %d \n", argv[1], dht11_dat[2]);

/*
    check = read(client_socket,Buff,sizeof(Buff));
    if (client_socket == -1){

      printf("Disconnection Check\n");
      close(client_socket);
      break;

    }

    Buff[sizeof(Buff)] = '\0';
    printf(">> %s\n",Buff);*/
  }

  close(client_socket);

  return 0;
}
