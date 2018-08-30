/*
 * MS5611.h
 *
 *  Created on: 2018. 8. 28.
 *      Author: HyunwooPark
 */

#ifndef INCLUDE_MS5611_H_
#define INCLUDE_MS5611_H_

/* Include Files */
#include "HL_sys_common.h"
#include "HL_system.h"
#include "HL_sci.h"
#include "HL_i2c.h"

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define UART sciREG1
#define I2C  i2cREG1
#define MS5611_ADDRESS                0x77

#define MS5611_CMD_ADC_READ           0x00
#define MS5611_CMD_RESET              0x1E
#define MS5611_CMD_CONV_D1            0x40
#define MS5611_CMD_CONV_D2            0x50
#define MS5611_CMD_READ_PROM          0xA2


typedef enum
{
    MS5611_ULTRA_HIGH_RES = 0x08,
    MS5611_HIGH_RES = 0x06,
    MS5611_STANDARD = 0x04,
    MS5611_LOW_POWER = 0x02,
    MS5611_ULTRA_LOW_POWER = 0x00
} ms5611_osr_t;

uint16_t fc[6];
uint8_t ct;
uint8_t uosr;
int32_t TEMP2;
int64_t OFF2, SENS2;

void sciDisplayText(sciBASE_t *sci, uint8 *text, uint32 len);

/*I2C communication read, write*/
uint16_t read16(uint8_t regAddr);
uint32_t read24(uint8_t regAddr);

void i2c_write(uint8 devAddr, uint8 reg);

void wait(uint32 delay);
void disp_set(char *str);

void Init_MS5611(ms5611_osr_t osr);
void setOversampling(ms5611_osr_t osr);
ms5611_osr_t getOversampling(void);
void read_MS5611_PROM(void);
uint32_t readRawTemperature(void);
uint32_t readRawPressure(void);
int32_t readPressure(bool compensation);
double readTemperature(bool compensation);
double getSeaLevel(double pressure, double altitude);
double getAltitude(double pressure, double seaLevelPressure);


void Init_MS5611(ms5611_osr_t osr)
{
    i2c_write(MS5611_ADDRESS, MS5611_CMD_RESET);
    setOversampling(osr);
    wait(100);
    read_MS5611_PROM();

}


// Set oversampling value
void setOversampling(ms5611_osr_t osr)
{
    switch (osr)
    {
    case MS5611_ULTRA_LOW_POWER:
        ct = 10;
        break;
    case MS5611_LOW_POWER:
        ct = 20;
        break;
    case MS5611_STANDARD:
        ct = 30;
        break;
    case MS5611_HIGH_RES:
        ct = 50;
        break;
    case MS5611_ULTRA_HIGH_RES:
        ct = 100;
        break;
    }

    uosr = osr;
}


// Get oversampling value
ms5611_osr_t getOversampling(void)
{
    return (ms5611_osr_t) uosr;
}

void read_MS5611_PROM(void)
{
    uint8_t offset;
    for (offset = 0; offset < 6; offset++)
    {
        fc[offset] = read16(MS5611_CMD_READ_PROM + (offset * 2));
    }
}

uint32_t readRawTemperature(void)
{

    i2c_write(MS5611_ADDRESS, MS5611_CMD_CONV_D2 + uosr);

    wait(ct);

    return read24(MS5611_CMD_ADC_READ);
}

uint32_t readRawPressure(void)
{

    i2c_write(MS5611_ADDRESS, MS5611_CMD_CONV_D1 + uosr );

    wait(ct);

    return read24(MS5611_CMD_ADC_READ);
}

int32_t readPressure(bool compensation)
{
    uint32_t D1;

    uint32_t D2;
    int32_t dT;

    int64_t OFF;
    int64_t SENS;
    int32_t TEMP;
    uint32_t P;

    D1 = readRawPressure();
    D2 = readRawTemperature();
    dT = D2 - ((uint32_t)fc[4] * 256);
    OFF = (int64_t)fc[1] * 65536 + (int64_t)fc[3] * (dT / 128);
    SENS = (int64_t)fc[0] * 32768 + (int64_t)fc[2] * (dT / 256);

    if (compensation)
    {
        TEMP = 2000 + ((int64_t)dT * fc[5]) / 8388608;

        OFF2 = 0;
        SENS2 = 0;

        if (TEMP < 2000)
        {
            OFF2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 2;
            SENS2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 4;
        }

        if (TEMP < -1500)
        {
            OFF2 = OFF2 + 7 * ((TEMP + 1500) * (TEMP + 1500));
            SENS2 = SENS2 + 11 * ((TEMP + 1500) * (TEMP + 1500)) / 2;
        }

        OFF = OFF - OFF2;
        SENS = SENS - SENS2;
    }

    P = (D1 * SENS / 2097152 - OFF) / 32768;

    return P;
}

double readTemperature(bool compensation)
{
    uint32_t D2;
    int32_t dT ;

    int32_t TEMP;

    D2 = readRawTemperature();
    dT = D2 - ((uint32_t)fc[4] * 256);
    TEMP = 2000 + ((int64_t) dT * fc[5]) / 8388608;
    TEMP2 = 0;

    if (compensation)
    {
        if (TEMP < 2000)
        {
            TEMP2 = (dT * dT) / (2 << 30);
        }
    }

    TEMP = TEMP - TEMP2;

    return ((double) TEMP / 100);
}

double getAltitude(double pressure, double seaLevelPressure)
{
    return (44330.0 * (1.0 - pow(pressure / seaLevelPressure , 0.1902949f)));
}

// Calculate sea level from Pressure given on specific altitude
double getSeaLevel(double pressure, double altitude)
{
    return ((double)pressure / pow(1.0f - ((double)altitude / 44330.0f), 5.255f));
}

uint16_t read16(uint8_t regAddr)
{

    uint16_t val;
    uint8_t data[2];

    i2c_write(MS5611_ADDRESS, regAddr);

//    i2cSetSlaveAdd(I2C, MS5611_ADDRESS);
    i2cSetDirection(I2C, I2C_RECEIVER);
    i2cSetCount(I2C, 2);
    i2cSetMode(I2C, I2C_MASTER);
    i2cSetStart(I2C);
    i2cReceive(I2C, 2, data);
    i2cSetStop(I2C);

    while (i2cIsBusBusy(I2C) == true)
        ;
    while (i2cIsStopDetected(I2C) == 0)
        ;
    i2cClearSCD(I2C);

    val = data[0] << 8 | data[1];

    return val;
}

uint32_t read24(uint8_t regAddr)
{

    uint32_t val;
    uint8_t data[3];

    i2c_write(MS5611_ADDRESS, regAddr);

 //   i2cSetSlaveAdd(I2C, MS5611_ADDRESS);
    i2cSetDirection(I2C, I2C_RECEIVER);
    i2cSetCount(I2C, 3);
    i2cSetMode(I2C, I2C_MASTER);
    i2cSetStart(I2C);

    i2cReceive(I2C, 3, data);
    i2cSetStop(I2C);

    while (i2cIsBusBusy(I2C) == true)
        ;
    while (i2cIsStopDetected(I2C) == 0)
        ;
    i2cClearSCD(I2C);

    val = ((int32_t)data[0] << 16) | ((int32_t)data[1] << 8) | data[2];

    return val;
}

void i2c_write(uint8 devAddr, uint8_t reg)
{

    i2cSetSlaveAdd(I2C, devAddr);
    i2cSetDirection(I2C, I2C_TRANSMITTER);
    i2cSetCount(I2C, 1);
    i2cSetMode(I2C, I2C_MASTER);
    i2cSetStop(I2C); //마스터 모드에서만 사용이 가능하다. 한번 멈춰주고 받아야 한다 안정화.
    i2cSetStart(I2C);
    i2cSendByte(I2C, reg);
//    i2cSend(i2cREG2, 1, &data); //i2cReceive

    while (i2cIsBusBusy(I2C) == true)
        ; //버스의 상태를 나타낸다 (다른 슬레이브 or 마스터가 사용 중인지 확인)
    while (i2cIsStopDetected(I2C) == 0)
        ; // 송신 중이던 객체가 다 전송하고 정지 신호를 보냈는지 확인한다.
    i2cClearSCD(I2C); //stop 비트를 초기화 한다. (다른 객체가 정지 신호를 보냈으니 1로 되있으므로 그래서 다른 통신이 실행되면, 1로 정지비트를 쓸 수 있게 초기화 하는 기능)

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
    char txt_buf[256] = { 0 };
    unsigned int buf_len;
    sprintf(txt_buf, str);
    buf_len = strlen(txt_buf);
    sciDisplayText(sciREG1, (uint8 *) txt_buf, buf_len);
    wait(100000);
}

void sciDisplayText(sciBASE_t *sci, uint8 *text, uint32 len)
{
    while (len--)
    {
        while ((UART->FLR & 0x4) == 4)
            ;
        sciSendByte(UART, *text++);
    }
}

#endif /* INCLUDE_MS5611_H_ */
