/** @file HL_sys_main.c 
*   @brief Application main file
*   @date 07-July-2017
*   @version 04.07.00
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2016 Texas Instruments Incorporated - www.ti.com  
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */

#include "HL_sys_common.h"

/* USER CODE BEGIN (1) */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "HL_hal_stdtypes.h"
#include "HL_reg_sci.h"
#include "HL_can.h"
#include "HL_sci.h"
#include "HL_i2c.h"
#include "HL_adc.h"
#include "os_mpu_wrappers.h"
#include "os_projdefs.h"
#include "os_semphr.h"
#include "os_task.h"
#include "os_queue.h"
#include "HL_gio.h"
#include "HL_eqep.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define LCD_ADDRESS 0x3F
#define UNIT_POSITION_X 60000000U


adcData_t counter;
uint8 msg[32] = {0};
uint32 value;
uint32 count = 0;

uint16 deltaT = 0U;
float velocity = 0U;
double tmp = 0U;
uint8 vel[4] = {0};
uint8 rx_data = {0};
char num;

xTaskHandle xTask1Handle;
//xTaskHandle xTask2Handle;
xTaskHandle xTask3Handle;
xTaskHandle xTask4Handle;

QueueHandle_t mutex;


void lcd_send_cmd(char cmd)
{
    volatile unsigned int cnt = 4;
    unsigned char data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd&0xf0);
    data_l = ((cmd<<4)&0xf0);
    data_t[0] = data_u|0x0C;  //en=1, rs=0
    data_t[1] = data_u|0x08;  //en=0, rs=0
    data_t[2] = data_l|0x0C;  //en=1, rs=0
    data_t[3] = data_l|0x08;  //en=0, rs=0

    i2cSetSlaveAdd(i2cREG2, LCD_ADDRESS);
    i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
    i2cSetCount(i2cREG2, cnt+1);
    i2cSetMode(i2cREG2, I2C_MASTER);
    i2cSetStop(i2cREG2);
    i2cSetStart(i2cREG2);
    i2cSendByte(i2cREG2, LCD_ADDRESS);
    i2cSend(i2cREG2, cnt, data_t);  //can을 사용하면 여기서 막힘

    while(i2cIsBusBusy(i2cREG2)==true);
    while(i2cIsStopDetected(i2cREG2)==0);
    i2cClearSCD(i2cREG2);

    for(cnt = 0; cnt < 1000000; cnt++);
}

void lcd_send_data(char data)
{
    volatile unsigned int cnt = 4;
    char data_u, data_l;
    uint8_t data_t[4];

    data_u = (data&0xf0);
    data_l = ((data <<4)&0xf0);
    data_t[0] = data_u|0x0D;  //en=1, rs=0
    data_t[1] = data_u|0x09;  //en=0, rs=0
    data_t[2] = data_l|0x0D;  //en=1, rs=0
    data_t[3] = data_l|0x09;  //en=0, rs=0

    i2cSetSlaveAdd(i2cREG2, LCD_ADDRESS);
    i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
    i2cSetCount(i2cREG2, cnt+1);
    i2cSetMode(i2cREG2, I2C_MASTER);
    i2cSetStop(i2cREG2);
    i2cSetStart(i2cREG2);
    i2cSendByte(i2cREG2, LCD_ADDRESS);
    i2cSend(i2cREG2, cnt, data_t);

    while(i2cIsBusBusy(i2cREG2)==true);
    while(i2cIsStopDetected(i2cREG2)==0);
    i2cClearSCD(i2cREG2);

    for(cnt = 0; cnt < 1000000; cnt++);
}

void lcd_send_string (char *str)
{
    while (*str) lcd_send_data (*str++);
}

void lcd_init(void)
{
    lcd_send_cmd(0x02);
    lcd_send_cmd(0x28);
    lcd_send_cmd(0x0c);
    lcd_send_cmd(0x80);
}

void parsing(float velocity)
{
    uint32 temp = 0;

    velocity /= 1000000;
    temp = velocity;

    vel[0] = temp / 1000;
    temp %= 1000;
    vel[1] = temp / 100;
    temp %= 100;
    vel[2] = temp / 10;
    temp %= 10;
    vel[3] = temp;

    vel[0] += 48;
    vel[1] += 48;
    vel[2] += 48;
    vel[3] += 48;
}

void led(int bri)
{
    if (bri <= 90)
        gioSetBit(gioPORTB, 2, 1);
    else
        gioSetBit(gioPORTB, 2, 0);
}
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */


// LCD
void vTask1(void *pbParameters)
{
    volatile int i;

    lcd_init();

    for(;;)
    {
        //xSemaphoreTake(mutex, portMAX_DELAY);
        lcd_send_cmd(0x80);
        lcd_send_string("Protocol:");

        lcd_send_cmd(0x89);
        lcd_send_string("joseph");

        lcd_send_cmd(0x01);
        //xSemaphoreGive(mutex);

        //vTaskDelay(10);
    }
}

// eqep 측정
void vTask2(void *pbParameters)
{
    uint8 msg3[32] = {'T', 'a', 's', 'k', '2', '\r', '\n'};

    for(;;)
    {
        xSemaphoreTake(mutex, portMAX_DELAY);
        if((eqepREG1->QEPSTS & 0x80U) != 0U)
        {
            deltaT = eqepREG1->QCPRD;

            tmp = (double)((double)deltaT / (10000000.0 / 8.0));

            velocity = (double) (UNIT_POSITION_X/tmp);

            parsing(velocity);

            eqepREG1->QEPSTS |= 0x80U;
        }

        xSemaphoreGive(mutex);
        //vTaskDelay(10);
    }
}

// DSP->MCU CAN 데이터 받아서 할 일 선택
void vTask3(void *pbParameters)
{
    uint8 msg3[32] = {'T', 'a', 's', 'k', '3', '\r', '\n'};

    for(;;)
    {
        //vTaskDelay(10000000);
        xSemaphoreTake(mutex, portMAX_DELAY);
        if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX2))
        {
            canGetData(canREG1, canMESSAGE_BOX2, (uint8 *) &rx_data);
            sciSendByte(sciREG1, rx_data+48);

            switch(rx_data)
            {
            case 1: //좌회전
                gioToggleBit(gioPORTA, 0);
                //vTaskDelay(500);
                break;

            case 2: //우회전
                gioToggleBit(gioPORTA, 1);
                //vTaskDelay(500);
                break;

            case 4: //후진
                gioToggleBit(gioPORTA, 2);
                //vTaskDelay(500);
                break;

            case 5: //정지
                gioToggleBit(gioPORTA, 3);
                //vTaskDelay(500);
                break;

            case 7: //좌측깜박이
                gioToggleBit(gioPORTA, 4);
                //vTaskDelay(500);
                break;

            case 8: //우측깜박이
                gioToggleBit(gioPORTA, 6);
                //vTaskDelay(500);
                break;

            default:
                gioToggleBit(gioPORTA, 7);
                //vTaskDelay(500);
                break;
            }
        }
        xSemaphoreGive(mutex);
    }
}

// 전조등 ADC 입력 (HW)
void vTask4(void *pbParameters)
{
    while (1)
    {
        gioSetBit(gioPORTB, 0, 1);
        gioSetBit(gioPORTB, 1, 1);
        while (adcIsConversionComplete(adcREG1, adcGROUP1) == 0);

        adcGetData(adcREG1, adcGROUP1, &counter);

//        sprintf(msg, "value = %d\r\n", counter.value);
//        sciSend(sciREG1, strlen(msg), msg);

        led(counter.value);
//        vTaskDelay(80);
        gioSetBit(gioPORTB, 0, 0);
    }
}

/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
    uint32 i;
    gioInit();
    sciInit();
    etpwmInit();
    canInit();
    QEPInit();
    adcInit();

    adcStartConversion(adcREG1, adcGROUP1);
    eqepEnableCounter(eqepREG1);
    eqepEnableUnitTimer(eqepREG1);
    eqepEnableCapture(eqepREG1);

    etpwmStartTBCLK();

    gioSetBit(gioPORTB, 0, 0); //adc 트리거로 사용

    for(i = 0; i< 10000000; i++);

    i2cInit();

    for(i = 0; i< 10000000; i++);

    vSemaphoreCreateBinary(mutex);

    //lcd_send_cmd(0x01);


    if(xTaskCreate(vTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, 1, &xTask1Handle) != pdTRUE)
        while(1);
//    if(xTaskCreate(vTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1, &xTask2Handle) != pdTRUE)
//        while(1);
    if(xTaskCreate(vTask3, "Task3", configMINIMAL_STACK_SIZE, NULL, 1, &xTask3Handle) != pdTRUE)
        while(1);
    if(xTaskCreate(vTask4, "Task4", configMINIMAL_STACK_SIZE, NULL, 1, &xTask4Handle) != pdTRUE)
        while(1);

    vTaskStartScheduler();

    while(1);

/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */
/* USER CODE END */
