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
#include <string.h>
#include <stdio.h>


#include "HL_sys_common.h"
#include "HL_sys_core.h"
#include "HL_i2c.h"


#define LCD_ADDRESS 0x3F

void lcd_sned_string(char *str);
void lcd_init(void);
void lcd_send_cmd(char cmd);
void lcd_send_data(char data);

void lcd_send_string (char *str)
{
    while (*str) lcd_send_data (*str++);
}


int main(void)
{
    volatile int i;

    for(i = 0; i< 10000000; i++);

    i2cInit();

    for(i = 0; i< 10000000; i++);
        lcd_init();

        while(1)
        {
                lcd_send_cmd(0x80);
                lcd_send_string("   Handsome   ");
                lcd_send_cmd(0xc0);
                lcd_send_string("   SiYunKim   ");
                for(i=0; i<80000000; i++);
                lcd_send_cmd(0x01);
        }
}


/* USER CODE BEGIN (4) */
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
    i2cSend(i2cREG2, cnt, data_t);

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

void lcd_init(void)
{
    lcd_send_cmd(0x02);
    lcd_send_cmd(0x28);
    lcd_send_cmd(0x0c);
    lcd_send_cmd(0x80);
}


/* USER CODE END */
