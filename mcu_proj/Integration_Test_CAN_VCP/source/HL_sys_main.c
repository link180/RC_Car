#include <HL_etpwm.h>
#include <HL_hal_stdtypes.h>
#include <HL_reg_sci.h>
#include <HL_sci.h>
#include "HL_can.h"
#include "HL_gio.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
void send_data(uint8* msg, int length);
int main(void)
{
    uint8 rx_data[32] = {0,};
    uint8 tx_data[8]= {1, 1, 1, 1, 1, 1, 1, 1};
    int data = 0;
    int i, j;
    etpwmInit();
    sciInit();
    canInit();
    gioInit();

    etpwmStartTBCLK();
    canEnableErrorNotification(canREG1); //canIoSetDirection(canREG1, canMESSAGE_BOX1, canMESSAGE_BOX2);
    gioSetBit(gioPORTA, 0, 1);

    for(i=0;;i++){
        for(j=0;j<50000000;j++)
            ;

        canTransmit(canREG1, canMESSAGE_BOX1, (const uint8*)&tx_data[0]);
    }
    while(1)
    {
        while(!canIsRxMessageArrived(canREG1, canMESSAGE_BOX2))
            ;
//        {
            canGetData(canREG1, canMESSAGE_BOX2, (const uint8*)&rx_data[0]);


              switch(rx_data[0]){
                  case 13:
                      data = (rx_data[1])*1000 + (rx_data[2])*100 + (rx_data[3])*10 + (rx_data[4]);
                      data *= 1.25;
                      break;
                  case 7:
                      gioSetBit(gioPORTA, 0, rx_data[1]);
                      break;
              }
//            data = (rx_data[0] - 48)*1000 + (rx_data[1] - 48)*100 + (rx_data[2] - 48)*10 + (rx_data[3] -48);

//            data = atoi(rx_data);

            send_data(rx_data, 8);
            etpwmREG1->CMPA = data;

            memset(rx_data, 0, sizeof(rx_data));
            data = 0;
//        }
    }

    return 0;
}

void send_data(uint8* msg, int length)
{
    int i;
    for(i=0;i<length;i++)
        sciSendByte(sciREG1, msg[i] + 48);

    sciSendByte(sciREG1, '\r');
    sciSendByte(sciREG1, '\n');
}
