/*
 * BMP280.h
 *
 *  Created on: 2018. 8. 26.
 *      Author: HyunwooPark
 */

#ifndef INCLUDE_BMP280_H_
#define INCLUDE_BMP280_H_


/* Include Files */
#include "HL_sys_common.h"
#include "HL_system.h"
#include "HL_etpwm.h"
#include "HL_sci.h"
#include "HL_gio.h"
#include "HL_i2c.h"
#include "HL_rti.h"

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define UART sciREG1

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
#define BMP280_ADDRESS           0x77
/*=========================================================================*/

/*=========================================================================
    REGISTERS
 -----------------------------------------------------------------------*/

#define    BMP280_REGISTER_DIG_T1        0x88
#define    BMP280_REGISTER_DIG_T2        0x8A
#define    BMP280_REGISTER_DIG_T3        0x8C

#define    BMP280_REGISTER_DIG_P1        0x8E
#define    BMP280_REGISTER_DIG_P2        0x90
#define    BMP280_REGISTER_DIG_P3        0x92
#define    BMP280_REGISTER_DIG_P4        0x94
#define    BMP280_REGISTER_DIG_P5        0x96
#define    BMP280_REGISTER_DIG_P6        0x98
#define    BMP280_REGISTER_DIG_P7        0x9A
#define    BMP280_REGISTER_DIG_P8        0x9C
#define    BMP280_REGISTER_DIG_P9        0x9E

#define  BMP280_REGISTER_DIG_H1          0xA1
#define  BMP280_REGISTER_DIG_H2          0xE1
#define  BMP280_REGISTER_DIG_H3          0xE3
#define  BMP280_REGISTER_DIG_H4          0xE4
#define  BMP280_REGISTER_DIG_H5          0xE5
#define  BMP280_REGISTER_DIG_H6          0xE7

#define    BMP280_REGISTER_CHIPID        0xD0
#define    BMP280_REGISTER_VERSION       0xD1
#define    BMP280_REGISTER_SOFTRESET     0xE0

#define    BMP280_REGISTER_CAL26         0xE1  // R calibration stored in 0xE1-0xF0

#define    BMP280_REGISTER_CONTROL       0xF4
#define    BMP280_REGISTER_CONFIG        0xF5
#define    BMP280_REGISTER_PRESSUREDATA  0xF7
#define    BMP280_REGISTER_TEMPDATA      0xFA
#define    BMP280_REGISTER_HUMIDDATA     0xFD


/*=========================================================================
    CALIBRATION DATA
    -----------------------------------------------------------------------*/
typedef struct
{
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;

    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;

    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
} bmp280_calib_data;



void sciDisplayText(sciBASE_t *sci, uint8 *text, uint32 len);

/*I2C communication read, write*/
uint16_t read8(uint8 regAddr);
uint16_t read16(uint8 regAddr);
uint16_t read16_LE(uint8_t reg);
int16_t readS16(uint8_t reg);
int16_t readS16_LE(uint8_t reg);
uint32_t read24(uint8_t regAddr);

void writeByte(uint8 devAddr, uint8 regAddr, uint8 data);
void wait(uint32 delay);
void disp_set(char *str);

void bmp280_readCoefficients(void);
void BMP280_Init(void);

float bmp280_readAltitude(float seaLevel);
float bmp280_readHumidity(void);
float bmp280_readPressure(void);
float bmp280_readTemperature(void);


uint32_t t_fine; //must be global
bmp280_calib_data _bmp280_calib;
float init_atmos = 0.0;
float init_alt = 0.0;

uint16_t read8(uint8 regAddr){

        uint8 data;

        i2cSetSlaveAdd(i2cREG2, BMP280_ADDRESS);
        i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
        i2cSetCount(i2cREG2, 1);
        i2cSetMode(i2cREG2, I2C_MASTER);
        i2cSetStop(i2cREG2);
        i2cSetStart(i2cREG2);
        i2cSendByte(i2cREG2, regAddr);

        while(i2cIsBusBusy(i2cREG2) == true)
            ;
        while(i2cIsStopDetected(i2cREG2) == 0)
            ;

        i2cSetDirection(i2cREG2, I2C_RECEIVER);
        i2cSetCount(i2cREG2, 1);
        i2cSetMode(i2cREG2, I2C_MASTER);
        i2cSetStart(i2cREG2);

        i2cReceive(i2cREG2, 1, &data);
        i2cSetStop(i2cREG2);

        while(i2cIsBusBusy(i2cREG2) == true)
            ;
        while(i2cIsStopDetected(i2cREG2) == 0)
            ;
        i2cClearSCD(i2cREG2);

        return (uint16_t)data;
}

uint16_t read16(uint8 regAddr){

        uint16_t val;
        uint8_t data[2];

        i2cSetSlaveAdd(i2cREG2, BMP280_ADDRESS);
        i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
        i2cSetCount(i2cREG2, 1);
        i2cSetMode(i2cREG2, I2C_MASTER);
        i2cSetStop(i2cREG2);
        i2cSetStart(i2cREG2);
        i2cSendByte(i2cREG2, regAddr);

        while(i2cIsBusBusy(i2cREG2) == true)
            ;
        while(i2cIsStopDetected(i2cREG2) == 0)
            ;

        i2cSetDirection(i2cREG2, I2C_RECEIVER);
        i2cSetCount(i2cREG2, 2);
        i2cSetMode(i2cREG2, I2C_MASTER);
        i2cSetStart(i2cREG2);

        i2cReceive(i2cREG2, 2, data);
        i2cSetStop(i2cREG2);

        while(i2cIsBusBusy(i2cREG2) == true)
            ;
        while(i2cIsStopDetected(i2cREG2) == 0)
            ;
        i2cClearSCD(i2cREG2);

        val = data[0];
        val <<= 8;
        val |= data[1];

        return val;
}

uint16_t read16_LE(uint8_t reg)
{
  uint16_t temp = read16(reg);
  return (temp >> 8) | (temp << 8);
}

int16_t readS16(uint8_t reg)
{
  return (int16_t)read16(reg);

}

int16_t readS16_LE(uint8_t reg)
{
  return (int16_t)read16_LE(reg);

}

uint32_t read24(uint8_t regAddr){

    uint32_t val;
    uint8_t data[3];

    i2cSetSlaveAdd(i2cREG2, BMP280_ADDRESS);
    i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
    i2cSetCount(i2cREG2, 1);
    i2cSetMode(i2cREG2, I2C_MASTER);
    i2cSetStop(i2cREG2);
    i2cSetStart(i2cREG2);
    i2cSendByte(i2cREG2, regAddr);

    while(i2cIsBusBusy(i2cREG2) == true)
        ;
    while(i2cIsStopDetected(i2cREG2) == 0)
        ;

    i2cSetDirection(i2cREG2, I2C_RECEIVER);
    i2cSetCount(i2cREG2, 2);
    i2cSetMode(i2cREG2, I2C_MASTER);
    i2cSetStart(i2cREG2);

    i2cReceive(i2cREG2, 2, data);
    i2cSetStop(i2cREG2);

    while(i2cIsBusBusy(i2cREG2) == true)
        ;
    while(i2cIsStopDetected(i2cREG2) == 0)
        ;
    i2cClearSCD(i2cREG2);

    val = data[0];
    val <<= 8;
    val |= data[1];
    val <<= 8;
    val |= data[2];

    return val;
}



void writeByte(uint8 devAddr, uint8 regAddr, uint8 data){

    i2cSetSlaveAdd(i2cREG2, devAddr);
    i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
    i2cSetCount(i2cREG2, 2);
    i2cSetMode(i2cREG2, I2C_MASTER);
    i2cSetStop(i2cREG2); //마스터 모드에서만 사용이 가능하다. 한번 멈춰주고 받아야 한다 안정화.
    i2cSetStart(i2cREG2);
    i2cSendByte(i2cREG2, regAddr);
    i2cSend(i2cREG2, 1, &data); //i2cReceive

    while(i2cIsBusBusy(i2cREG2) == true); //버스의 상태를 나타낸다 (다른 슬레이브 or 마스터가 사용 중인지 확인)
    while(i2cIsStopDetected(i2cREG2) == 0); // 송신 중이던 객체가 다 전송하고 정지 신호를 보냈는지 확인한다.
    i2cClearSCD(i2cREG2); //stop 비트를 초기화 한다. (다른 객체가 정지 신호를 보냈으니 1로 되있으므로 그래서 다른 통신이 실행되면, 1로 정지비트를 쓸 수 있게 초기화 하는 기능)

    wait(100000);
}


void wait(uint32 delay)
{
    int i;

    for (i = 0; i < delay; i++)
        ;
}

void disp_set(char *str)
{
    char txt_buf[256] = {0};
    unsigned int buf_len;
    sprintf(txt_buf, str);
    buf_len = strlen(txt_buf);
    sciDisplayText(sciREG1, (uint8 *)txt_buf, buf_len);
    wait(100000);
}

void sciDisplayText(sciBASE_t *sci, uint8 *text, uint32 len)
{
    while(len--)
    {
        while((UART->FLR & 0x4) == 4)
            ;
        sciSendByte(UART, *text++);
    }
}


void bmp280_readCoefficients(void)
{

  _bmp280_calib.dig_T1 = read16_LE(BMP280_REGISTER_DIG_T1);
  _bmp280_calib.dig_T2 = readS16_LE(BMP280_REGISTER_DIG_T2);
  _bmp280_calib.dig_T3 = readS16_LE(BMP280_REGISTER_DIG_T3);

  _bmp280_calib.dig_P1 = read16_LE(BMP280_REGISTER_DIG_P1);
  _bmp280_calib.dig_P2 = readS16_LE(BMP280_REGISTER_DIG_P2);
  _bmp280_calib.dig_P3 = readS16_LE(BMP280_REGISTER_DIG_P3);
  _bmp280_calib.dig_P4 = readS16_LE(BMP280_REGISTER_DIG_P4);
  _bmp280_calib.dig_P5 = readS16_LE(BMP280_REGISTER_DIG_P5);
  _bmp280_calib.dig_P6 = readS16_LE(BMP280_REGISTER_DIG_P6);
  _bmp280_calib.dig_P7 = readS16_LE(BMP280_REGISTER_DIG_P7);
  _bmp280_calib.dig_P8 = readS16_LE(BMP280_REGISTER_DIG_P8);
  _bmp280_calib.dig_P9 = readS16_LE(BMP280_REGISTER_DIG_P9);

  _bmp280_calib.dig_H1 = read8(BMP280_REGISTER_DIG_H1);
  _bmp280_calib.dig_H2 = readS16_LE(BMP280_REGISTER_DIG_H2);
  _bmp280_calib.dig_H3 = read8(BMP280_REGISTER_DIG_H3);
  _bmp280_calib.dig_H4 = (read8(BMP280_REGISTER_DIG_H4) << 4) | (read8(BMP280_REGISTER_DIG_H4+1) & 0xF);
  _bmp280_calib.dig_H5 = (read8(BMP280_REGISTER_DIG_H5+1) << 4) | (read8(BMP280_REGISTER_DIG_H5) >> 4);
  _bmp280_calib.dig_H6 = (int8_t)read8(BMP280_REGISTER_DIG_H6);

}

void BMP280_Init(void){
    if (read8(BMP280_REGISTER_CHIPID) != 0x58){
        disp_set("Wrong chipId");
        while(1);
    }
    bmp280_readCoefficients();
    writeByte(BMP280_ADDRESS, BMP280_REGISTER_CONTROL, 0xBF);  // 1011 1111
}

float bmp280_readTemperature(void)
{
  int32_t adc_T = read24(BMP280_REGISTER_TEMPDATA);

  int32_t var1, var2;

  adc_T >>= 4;

  var1  = ((((adc_T>>3) - ((int32_t)_bmp280_calib.dig_T1 <<1))) *
           ((int32_t)_bmp280_calib.dig_T2)) >> 11;

  var2  = (((((adc_T>>4) - ((int32_t)_bmp280_calib.dig_T1)) *
             ((adc_T>>4) - ((int32_t)_bmp280_calib.dig_T1))) >> 12) *
           ((int32_t)_bmp280_calib.dig_T3)) >> 14;

  t_fine = var1 + var2;

  float T  = (t_fine * 5 + 128) >> 8;

  return T/100;
}

float bmp280_readPressure(void)
{
    int64_t var1, var2, p;

    // Must be done first to get the t_fine variable set up
    bmp280_readTemperature();

    int32_t adc_P = read24(BMP280_REGISTER_PRESSUREDATA);
    adc_P >>= 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_bmp280_calib.dig_P6;
    var2 = var2 + ((var1*(int64_t)_bmp280_calib.dig_P5)<<17);
    var2 = var2 + (((int64_t)_bmp280_calib.dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)_bmp280_calib.dig_P3)>>8) +
      ((var1 * (int64_t)_bmp280_calib.dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)_bmp280_calib.dig_P1)>>33;

    if (var1 == 0) {
      return 0;  // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p<<31) - var2)*3125) / var1;
    var1 = (((int64_t)_bmp280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)_bmp280_calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)_bmp280_calib.dig_P7)<<4);

    return (float)p/256;
}

float bmp280_readHumidity(void)
{

  bmp280_readTemperature(); // must be done first to get t_fine

  int32_t adc_H = read16(BMP280_REGISTER_HUMIDDATA);

  int32_t v_x1_u32r;

  v_x1_u32r = (t_fine - ((int32_t)76800));

  v_x1_u32r = (((((adc_H << 14) - (((int32_t)_bmp280_calib.dig_H4) << 20) -
                  (((int32_t)_bmp280_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
               (((((((v_x1_u32r * ((int32_t)_bmp280_calib.dig_H6)) >> 10) *
                    (((v_x1_u32r * ((int32_t)_bmp280_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                  ((int32_t)2097152)) * ((int32_t)_bmp280_calib.dig_H2) + 8192) >> 14));

  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                             ((int32_t)_bmp280_calib.dig_H1)) >> 4));

  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
  float h = (v_x1_u32r>>12);
  return  h / 1024.0;
}

float bmp280_readAltitude(float seaLevel)
{
  // Equation taken from BMP180 datasheet (page 16):
  //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

  // Note that using the equation from wikipedia can give bad results
  // at high altitude.  See this thread for more information:
  //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064
  int i;

  float atmospheric = 0.0;
  float altitude = 0.0;
  for(i=0; i<100; i++){
      atmospheric += bmp280_readPressure() / 100.0F;
  }
  atmospheric /= 100.0F;
  altitude = 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
  if((altitude - init_alt) < 0){
      return 0.0F;
  }
  return  altitude - init_alt;
}

void init_altitude(float seaLevel){
    int i;

    for(i=0; i<1000; i++){
        init_atmos += bmp280_readPressure() / 100.0F;
    }
    init_atmos /= 1000.0F;
    init_alt = 44330.0 * (1.0 - pow(init_atmos / seaLevel, 0.1903));
}

#endif /* INCLUDE_BMP280_H_ */
