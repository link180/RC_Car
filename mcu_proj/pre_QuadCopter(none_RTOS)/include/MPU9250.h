/*
 * MPU9250.h
 *
 *  Created on: 2018. 8. 1.
 *      Author: HyunwooPark
 */

#ifndef INCLUDE_MPU9250_H_
#define INCLUDE_MPU9250_H_

/* Include Files */
#include "HL_sys_common.h"
#include "HL_system.h"
#include "HL_etpwm.h"
#include "HL_sci.h"
#include "HL_gio.h"
#include "HL_i2c.h"
#include "Hl_rti.h"
#include "HL_het.h"
#include "HL_ecap.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

#define UART sciREG1
#define UART2 sciREG2
#define UART_MPU6050 sciREG3
#define UART_CONTROLLER sciREG4

#define    MPU9250_ADDRESS            0x68
#define    MAG_ADDRESS                0x0C

#define    GYRO_FULL_SCALE_250_DPS    0x00
#define    GYRO_FULL_SCALE_500_DPS    0x08
#define    GYRO_FULL_SCALE_1000_DPS   0x10
#define    GYRO_FULL_SCALE_2000_DPS   0x18

#define    ACC_FULL_SCALE_2_G        0x00
#define    ACC_FULL_SCALE_4_G        0x08
#define    ACC_FULL_SCALE_8_G        0x10
#define    ACC_FULL_SCALE_16_G       0x18


#define R2D  180 / M_PI
#define D2R  0.0174533
#define GYRO2DEGREE_PER_SEC  65.5

float pitch;
float roll;
float yaw;

/*조종기에서 사용하기위한 용도*/
float init_pitch;
float init_roll;
float init_yaw;

//Magnetometer Registers
#define AK8963_ADDRESS   0x0C
#define WHO_AM_I_AK8963  0x00 // should return 0x48
#define INFO             0x01
#define AK8963_ST1       0x02  // data ready status bit 0
#define AK8963_XOUT_L    0x03  // data
#define AK8963_XOUT_H    0x04
#define AK8963_YOUT_L    0x05
#define AK8963_YOUT_H    0x06
#define AK8963_ZOUT_L    0x07
#define AK8963_ZOUT_H    0x08
#define AK8963_ST2       0x09  // Data overflow bit 3 and data read error status bit 2
#define AK8963_CNTL      0x0A  // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define AK8963_ASTC      0x0C  // Self test control
#define AK8963_I2CDIS    0x0F  // I2C disable
#define AK8963_ASAX      0x10  // Fuse ROM x-axis sensitivity adjustment value
#define AK8963_ASAY      0x11  // Fuse ROM y-axis sensitivity adjustment value
#define AK8963_ASAZ      0x12  // Fuse ROM z-axis sensitivity adjustment value

#define SELF_TEST_X_GYRO 0x00
#define SELF_TEST_Y_GYRO 0x01
#define SELF_TEST_Z_GYRO 0x02

/*#define X_FINE_GAIN      0x03 // [7:0] fine gain
 #define Y_FINE_GAIN      0x04
 #define Z_FINE_GAIN      0x05
 #define XA_OFFSET_H      0x06 // User-defined trim values for accelerometer
 #define XA_OFFSET_L_TC   0x07
 #define YA_OFFSET_H      0x08
 #define YA_OFFSET_L_TC   0x09
 #define ZA_OFFSET_H      0x0A
 #define ZA_OFFSET_L_TC   0x0B */

#define SELF_TEST_X_ACCEL 0x0D
#define SELF_TEST_Y_ACCEL 0x0E
#define SELF_TEST_Z_ACCEL 0x0F

#define SELF_TEST_A      0x10

#define XG_OFFSET_H      0x13  // User-defined trim values for gyroscope
#define XG_OFFSET_L      0x14
#define YG_OFFSET_H      0x15
#define YG_OFFSET_L      0x16
#define ZG_OFFSET_H      0x17
#define ZG_OFFSET_L      0x18
#define SMPLRT_DIV       0x19
#define CONFIG           0x1A
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define ACCEL_CONFIG2    0x1D
#define LP_ACCEL_ODR     0x1E
#define WOM_THR          0x1F

#define MOT_DUR          0x20  // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms
#define ZMOT_THR         0x21  // Zero-motion detection threshold bits [7:0]
#define ZRMOT_DUR        0x22  // Duration counter threshold for zero motion interrupt generation, 16 Hz rate, LSB = 64 ms

#define FIFO_EN          0x23
#define I2C_MST_CTRL     0x24
#define I2C_SLV0_ADDR    0x25
#define I2C_SLV0_REG     0x26
#define I2C_SLV0_CTRL    0x27
#define I2C_SLV1_ADDR    0x28
#define I2C_SLV1_REG     0x29
#define I2C_SLV1_CTRL    0x2A
#define I2C_SLV2_ADDR    0x2B
#define I2C_SLV2_REG     0x2C
#define I2C_SLV2_CTRL    0x2D
#define I2C_SLV3_ADDR    0x2E
#define I2C_SLV3_REG     0x2F
#define I2C_SLV3_CTRL    0x30
#define I2C_SLV4_ADDR    0x31
#define I2C_SLV4_REG     0x32
#define I2C_SLV4_DO      0x33
#define I2C_SLV4_CTRL    0x34
#define I2C_SLV4_DI      0x35
#define I2C_MST_STATUS   0x36
#define INT_PIN_CFG      0x37
#define INT_ENABLE       0x38
#define DMP_INT_STATUS   0x39  // Check DMP interrupt
#define INT_STATUS       0x3A
#define ACCEL_XOUT_H     0x3B
#define ACCEL_XOUT_L     0x3C
#define ACCEL_YOUT_H     0x3D
#define ACCEL_YOUT_L     0x3E
#define ACCEL_ZOUT_H     0x3F
#define ACCEL_ZOUT_L     0x40
#define TEMP_OUT_H       0x41
#define TEMP_OUT_L       0x42
#define GYRO_XOUT_H      0x43
#define GYRO_XOUT_L      0x44
#define GYRO_YOUT_H      0x45
#define GYRO_YOUT_L      0x46
#define GYRO_ZOUT_H      0x47
#define GYRO_ZOUT_L      0x48
#define EXT_SENS_DATA_00 0x49
#define EXT_SENS_DATA_01 0x4A
#define EXT_SENS_DATA_02 0x4B
#define EXT_SENS_DATA_03 0x4C
#define EXT_SENS_DATA_04 0x4D
#define EXT_SENS_DATA_05 0x4E
#define EXT_SENS_DATA_06 0x4F
#define EXT_SENS_DATA_07 0x50
#define EXT_SENS_DATA_08 0x51
#define EXT_SENS_DATA_09 0x52
#define EXT_SENS_DATA_10 0x53
#define EXT_SENS_DATA_11 0x54
#define EXT_SENS_DATA_12 0x55
#define EXT_SENS_DATA_13 0x56
#define EXT_SENS_DATA_14 0x57
#define EXT_SENS_DATA_15 0x58
#define EXT_SENS_DATA_16 0x59
#define EXT_SENS_DATA_17 0x5A
#define EXT_SENS_DATA_18 0x5B
#define EXT_SENS_DATA_19 0x5C
#define EXT_SENS_DATA_20 0x5D
#define EXT_SENS_DATA_21 0x5E
#define EXT_SENS_DATA_22 0x5F
#define EXT_SENS_DATA_23 0x60
#define MOT_DETECT_STATUS 0x61
#define I2C_SLV0_DO      0x63
#define I2C_SLV1_DO      0x64
#define I2C_SLV2_DO      0x65
#define I2C_SLV3_DO      0x66
#define I2C_MST_DELAY_CTRL 0x67
#define SIGNAL_PATH_RESET  0x68
#define MOT_DETECT_CTRL  0x69
#define USER_CTRL        0x6A  // Bit 7 enable DMP, bit 3 reset DMP
#define PWR_MGMT_1       0x6B // Device defaults to the SLEEP mode
#define PWR_MGMT_2       0x6C
#define DMP_BANK         0x6D  // Activates a specific bank in the DMP
#define DMP_RW_PNT       0x6E  // Set read/write pointer to a specific start address in specified DMP bank
#define DMP_REG          0x6F  // Register in DMP from which to read or to which to write
#define DMP_REG_1        0x70
#define DMP_REG_2        0x71
#define FIFO_COUNTH      0x72
#define FIFO_COUNTL      0x73
#define FIFO_R_W         0x74
#define WHO_AM_I_MPU9250 0x75 // Should return 0x71
#define XA_OFFSET_H      0x77
#define XA_OFFSET_L      0x78
#define YA_OFFSET_H      0x7A
#define YA_OFFSET_L      0x7B
#define ZA_OFFSET_H      0x7D
#define ZA_OFFSET_L      0x7E

// Using the MSENSR-9250 breakout board, ADO is set to 0
// Seven-bit device address is 110100 for ADO = 0 and 110101 for ADO = 1
#define ADO 0
#if ADO
#define MPU9250_ADDRESS 0x69  // Device address when ADO = 1
#else
#define MPU9250_ADDRESS 0x68  // Device address when ADO = 0
#define AK8963_ADDRESS 0x0C   //  Address of magnetometer
#endif

#define AHRS true         // set to false for basic data read
#define SerialDebug true  // set to true to get Serial output for debugging


enum Ascale {
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_16G
};

enum Gscale {
  GFS_250DPS = 0,
  GFS_500DPS,
  GFS_1000DPS,
  GFS_2000DPS
};

enum Mscale {
  MFS_14BITS = 0, // 0.6 mG per LSB
  MFS_16BITS      // 0.15 mG per LSB
};

uint8 Gscale = GFS_500DPS;
uint8 Ascale = AFS_2G;
uint8 Mscale = MFS_16BITS; // Choose either 14-bit or 16-bit magnetometer resolution
uint8 Mmode = 0x02;        // 2 for 8 Hz, 6 for 100 Hz continuous magnetometer data read



float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values



int16_t accelCount[3];  // Stores the 16-bit signed accelerometer sensor output
int16_t gyroCount[3];   // Stores the 16-bit signed gyro sensor output
int16_t magCount[3];    // Stores the 16-bit signed magnetometer sensor output

float magCalibration[3] = {0, 0, 0} , magbias[3] = {0, 0, 0};  // Factory mag calibration and mag bias
float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0};// Bias corrections for gyro and accelerometer


float aRes, gRes, mRes;      // scale resolutions per LSB for the sensors


//Declaring some global variables for roll, pitch
int16_t g_xyz[3];
long gyro_x_cal, gyro_y_cal, gyro_z_cal;
boolean set_gyro_angles;

int16_t a_xyz[3];
long acc_total_vector;
float angle_roll_acc, angle_pitch_acc;

float angle_pitch, angle_roll;
float angle_pitch_output, angle_roll_output;

float t = 0.0025;

void sciDisplayText(sciBASE_t *sci, uint8 *text, uint32 len);

/*I2C communication read, write*/
uint8 readByte(uint8 devAddr, uint8 regAddr);
void readBytes(uint8 devAddr, uint8 regAddr, uint8 length, uint8 *data);
void writeByte(uint8 devAddr, uint8 regAddr, uint8 data);


void wait(uint32 delay);

/*init setting*/
void initMPU9250(void);
void initAK8963(float * destination);
void calibrateMPU9250(float * dest1, float * dest2);

/*transformation method*/
void getAres(void);
void getMres(void);
void getGres(void);

/*read data from mpu9250*/
void readAccelData(int16_t* destination);
void readGyroData(int16_t * destination);
void readMagData(int16_t * destination);
void get_YPR(void);
void get_offset_value(void);

uint8 readByte(uint8 devAddr, uint8 regAddr){

        uint8 data;
        i2cSetSlaveAdd(i2cREG2, devAddr);
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

        return data;
}

void readBytes(uint8 devAddr, uint8 regAddr, uint8 length, uint8 *data){

        i2cSetSlaveAdd(i2cREG2, devAddr);
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
        i2cSetCount(i2cREG2, length);
        i2cSetMode(i2cREG2, I2C_MASTER);
        i2cSetStart(i2cREG2);

        i2cReceive(i2cREG2, length, data);
        i2cSetStop(i2cREG2);

        while(i2cIsBusBusy(i2cREG2) == true)
            ;
        while(i2cIsStopDetected(i2cREG2) == 0)
            ;
        i2cClearSCD(i2cREG2);
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

// Function which accumulates gyro and accelerometer data after device initialization. It calculates the average
// of the at-rest readings and then loads the resulting offsets into accelerometer and gyro bias registers.
void calibrateMPU9250(float * dest1, float * dest2){
    uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
    uint16_t ii, packet_count, fifo_count;
    int32_t gyro_bias[3]  = {
      0, 0, 0    }
    , accel_bias[3] = {
      0, 0, 0    };

    // reset device
    writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
    wait(200);

    // get stable time source; Auto select clock source to be PLL gyroscope reference if ready
    // else use the internal oscillator, bits 2:0 = 001
    writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x01);
    writeByte(MPU9250_ADDRESS, PWR_MGMT_2, 0x00);
    wait(200);

    // Configure device for bias calculation
    writeByte(MPU9250_ADDRESS, INT_ENABLE, 0x00);   // Disable all interrupts
    writeByte(MPU9250_ADDRESS, FIFO_EN, 0x00);      // Disable FIFO
    writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x00);   // Turn on internal clock source
    writeByte(MPU9250_ADDRESS, I2C_MST_CTRL, 0x00); // Disable I2C master
    writeByte(MPU9250_ADDRESS, USER_CTRL, 0x00);    // Disable FIFO and I2C master modes
    writeByte(MPU9250_ADDRESS, USER_CTRL, 0x0C);    // Reset FIFO and DMP
    wait(200);

    // Configure MPU6050 gyro and accelerometer for bias calculation
    writeByte(MPU9250_ADDRESS, CONFIG, 0x01);      // Set low-pass filter to 188 Hz
    writeByte(MPU9250_ADDRESS, SMPLRT_DIV, 0x00);  // Set sample rate to 1 kHz
    writeByte(MPU9250_ADDRESS, GYRO_CONFIG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
    writeByte(MPU9250_ADDRESS, ACCEL_CONFIG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity

    uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
    uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g

    // Configure FIFO to capture accelerometer and gyro data for bias calculation
    writeByte(MPU9250_ADDRESS, USER_CTRL, 0x40);   // Enable FIFO
    writeByte(MPU9250_ADDRESS, FIFO_EN, 0x78);     // Enable gyro and accelerometer sensors for FIFO  (max size 512 bytes in MPU-9150)
    wait(200); // accumulate 40 samples in 40 milliseconds = 480 bytes

    // At end of sample accumulation, turn off FIFO sensor read
    writeByte(MPU9250_ADDRESS, FIFO_EN, 0x00);        // Disable gyro and accelerometer sensors for FIFO
    readBytes(MPU9250_ADDRESS, FIFO_COUNTH, 2, &data[0]); // read FIFO sample count
    fifo_count = ((uint16_t)data[0] << 8) | data[1];
    packet_count = fifo_count/12;// How many sets of full gyro and accelerometer data for averaging

    for (ii = 0; ii < packet_count; ii++) {
      int16_t accel_temp[3] = {
        0, 0, 0        }
      , gyro_temp[3] = {
        0, 0, 0        };
      readBytes(MPU9250_ADDRESS, FIFO_R_W, 12, &data[0]); // read data for averaging
      accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;  // Form signed 16-bit integer for each sample in FIFO
      accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
      accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;
      gyro_temp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  ) ;
      gyro_temp[1]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  ) ;
      gyro_temp[2]  = (int16_t) (((int16_t)data[10] << 8) | data[11]) ;

      accel_bias[0] += (int32_t) accel_temp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
      accel_bias[1] += (int32_t) accel_temp[1];
      accel_bias[2] += (int32_t) accel_temp[2];
      gyro_bias[0]  += (int32_t) gyro_temp[0];
      gyro_bias[1]  += (int32_t) gyro_temp[1];
      gyro_bias[2]  += (int32_t) gyro_temp[2];

    }
    accel_bias[0] /= (int32_t) packet_count; // Normalize sums to get average count biases
    accel_bias[1] /= (int32_t) packet_count;
    accel_bias[2] /= (int32_t) packet_count;
    gyro_bias[0]  /= (int32_t) packet_count;
    gyro_bias[1]  /= (int32_t) packet_count;
    gyro_bias[2]  /= (int32_t) packet_count;

    if(accel_bias[2] > 0L) {
      accel_bias[2] -= (int32_t) accelsensitivity;
    }  // Remove gravity from the z-axis accelerometer bias calculation
    else {
      accel_bias[2] += (int32_t) accelsensitivity;
    }

    // Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
    data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
    data[1] = (-gyro_bias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
    data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
    data[3] = (-gyro_bias[1]/4)       & 0xFF;
    data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
    data[5] = (-gyro_bias[2]/4)       & 0xFF;

    // Push gyro biases to hardware registers
    writeByte(MPU9250_ADDRESS, XG_OFFSET_H, data[0]);
    writeByte(MPU9250_ADDRESS, XG_OFFSET_L, data[1]);
    writeByte(MPU9250_ADDRESS, YG_OFFSET_H, data[2]);
    writeByte(MPU9250_ADDRESS, YG_OFFSET_L, data[3]);
    writeByte(MPU9250_ADDRESS, ZG_OFFSET_H, data[4]);
    writeByte(MPU9250_ADDRESS, ZG_OFFSET_L, data[5]);

    // Output scaled gyro biases for display in the main program
    dest1[0] = (float) gyro_bias[0]/(float) gyrosensitivity;
    dest1[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
    dest1[2] = (float) gyro_bias[2]/(float) gyrosensitivity;

    // Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
    // factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
    // non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
    // compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
    // the accelerometer biases calculated above must be divided by 8.

    int32_t accel_bias_reg[3] = {
      0, 0, 0    }; // A place to hold the factory accelerometer trim biases
    readBytes(MPU9250_ADDRESS, XA_OFFSET_H, 2, &data[0]); // Read factory accelerometer trim values
    accel_bias_reg[0] = (int32_t) (((int16_t)data[0] << 8) | data[1]);
    readBytes(MPU9250_ADDRESS, YA_OFFSET_H, 2, &data[0]);
    accel_bias_reg[1] = (int32_t) (((int16_t)data[0] << 8) | data[1]);
    readBytes(MPU9250_ADDRESS, ZA_OFFSET_H, 2, &data[0]);
    accel_bias_reg[2] = (int32_t) (((int16_t)data[0] << 8) | data[1]);

    uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
    uint8_t mask_bit[3] = {
      0, 0, 0    }; // Define array to hold mask bit for each accelerometer bias axis

    for(ii = 0; ii < 3; ii++) {
      if((accel_bias_reg[ii] & mask)) mask_bit[ii] = 0x01; // If temperature compensation bit is set, record that fact in mask_bit
    }

    // Construct total accelerometer bias, including calculated average accelerometer bias from above
    accel_bias_reg[0] -= (accel_bias[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
    accel_bias_reg[1] -= (accel_bias[1]/8);
    accel_bias_reg[2] -= (accel_bias[2]/8);

    data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
    data[1] = (accel_bias_reg[0])      & 0xFF;
    data[1] = data[1] | mask_bit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
    data[3] = (accel_bias_reg[1])      & 0xFF;
    data[3] = data[3] | mask_bit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
    data[5] = (accel_bias_reg[2])      & 0xFF;
    data[5] = data[5] | mask_bit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers

    // Apparently this is not working for the acceleration biases in the MPU-9250
    // Are we handling the temperature correction bit properly?
    // Push accelerometer biases to hardware registers
    writeByte(MPU9250_ADDRESS, XA_OFFSET_H, data[0]);
    writeByte(MPU9250_ADDRESS, XA_OFFSET_L, data[1]);
    writeByte(MPU9250_ADDRESS, YA_OFFSET_H, data[2]);
    writeByte(MPU9250_ADDRESS, YA_OFFSET_L, data[3]);
    writeByte(MPU9250_ADDRESS, ZA_OFFSET_H, data[4]);
    writeByte(MPU9250_ADDRESS, ZA_OFFSET_L, data[5]);

    // Output scaled accelerometer biases for display in the main program
    dest2[0] = (float)accel_bias[0]/(float)accelsensitivity;
    dest2[1] = (float)accel_bias[1]/(float)accelsensitivity;
    dest2[2] = (float)accel_bias[2]/(float)accelsensitivity;
}
void getMres() {
  switch (Mscale)
  {
    // Possible magnetometer scales (and their register bit settings) are:
    // 14 bit resolution (0) and 16 bit resolution (1)
  case MFS_14BITS:
    mRes = 10.0 * 4219.0/ 8190.0; // Proper scale to return milliGauss
    break;
  case MFS_16BITS:
    mRes = 10.0 * 4219.0/ 32760.0; // Proper scale to return milliGauss
    break;
  }
}

void getGres() {
  switch (Gscale)
  {
    // Possible gyro scales (and their register bit settings) are:
    // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11).
    // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
  case GFS_250DPS:
    gRes = 250.0/32768.0;
    break;
  case GFS_500DPS:
    gRes = 500.0/32768.0;
    break;
  case GFS_1000DPS:
    gRes = 1000.0/32768.0;
    break;
  case GFS_2000DPS:
    gRes = 2000.0/32768.0;
    break;
  }
}
void getAres(void) {
  switch (Ascale)
  {
    // Possible accelerometer scales (and their register bit settings) are:
    // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
    // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
  case AFS_2G:
    aRes = 2.0/32768.0;
    break;
  case AFS_4G:
    aRes = 4.0/32768.0;
    break;
  case AFS_8G:
    aRes = 8.0/32768.0;
    break;
  case AFS_16G:
    aRes = 16.0/32768.0;
    break;
  }
}

void initMPU9250(void){
    // wake up device
     writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors
     wait(1000); // Wait for all registers to reset

     // get stable time source
     writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x01);  // Auto select clock source to be PLL gyroscope reference if ready else
     wait(1000);

     // Configure Gyro and Thermometer
     // Disable FSYNC and set thermometer and gyro bandwidth to 41 and 42 Hz, respectively;
     // minimum delay time for this setting is 5.9 ms, which means sensor fusion update rates cannot
     // be higher than 1 / 0.0059 = 170 Hz
     // DLPF_CFG = bits 2:0 = 011; this limits the sample rate to 1000 Hz for both
     // With the MPU9250, it is possible to get gyro sample rates of 32 kHz (!), 8 kHz, or 1 kHz
     writeByte(MPU9250_ADDRESS, CONFIG, 0x03);

     // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
     writeByte(MPU9250_ADDRESS, SMPLRT_DIV, 0x04);  // Use a 200 Hz rate; a rate consistent with the filter update rate
     // determined inset in CONFIG above

     // Set gyroscope full scale range
     // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
     uint8 c = readByte(MPU9250_ADDRESS, GYRO_CONFIG);
     //  writeRegister(GYRO_CONFIG, c & ~0xE0); // Clear self-test bits [7:5]
     // writeByte(MPU9250_ADDRESS, GYRO_CONFIG, c & ~0x02); // Clear Fchoice bits [1:0]
     // writeByte(MPU9250_ADDRESS, GYRO_CONFIG, c & ~0x18); // Clear AFS bits [4:3]
     // writeByte(MPU9250_ADDRESS, GYRO_CONFIG, c | Gscale << 3); // Set full scale range for the gyro
     // Clear Fchoice bit[1] and AFS bits[4:3]
     writeByte(MPU9250_ADDRESS, GYRO_CONFIG, (c & ~(0x02 | 0x18)) | Gscale << 3); // Set full scale range for the gyro
     // writeRegister(GYRO_CONFIG, c | 0x00); // Set Fchoice for the gyro to 11 by writing its inverse to bits 1:0 of GYRO_CONFIG

     // Set accelerometer full-scale range configuration
     c = readByte(MPU9250_ADDRESS, ACCEL_CONFIG);
     //  writeRegister(ACCEL_CONFIG, c & ~0xE0); // Clear self-test bits [7:5]
     // writeByte(MPU9250_ADDRESS, ACCEL_CONFIG, c & ~0x18); // Clear AFS bits [4:3]
     // writeByte(MPU9250_ADDRESS, ACCEL_CONFIG, c | Ascale << 3); // Set full scale range for the accelerometer
     writeByte(MPU9250_ADDRESS, ACCEL_CONFIG, (c & ~0x18) | Ascale << 3); // Set full scale range for the accelerometer

     // Set accelerometer sample rate configuration
     // It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
     // accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
     c = readByte(MPU9250_ADDRESS, ACCEL_CONFIG2);
     // writeByte(MPU9250_ADDRESS, ACCEL_CONFIG2, c & ~0x0F); // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])
     // writeByte(MPU9250_ADDRESS, ACCEL_CONFIG2, c | 0x03); // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz
     writeByte(MPU9250_ADDRESS, ACCEL_CONFIG2, (c & ~0x0F) | 0x03); // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz

     // The accelerometer, gyro, and thermometer are set to 1 kHz sample rates,
     // but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting

     // Configure Interrupts and Bypass Enable
     // Set interrupt pin active high, push-pull, hold interrupt pin level HIGH until interrupt cleared,
     // clear on read of INT_STATUS, and enable I2C_BYPASS_EN so additional chips
     // can join the I2C bus and all can be controlled by the Arduino as master
     writeByte(MPU9250_ADDRESS, INT_PIN_CFG, 0x22);
     writeByte(MPU9250_ADDRESS, INT_ENABLE, 0x01);  // Enable data ready (bit 0) interrupt
     wait(1000);
}
void initAK8963(float * destination)
{
  // First extract the factory calibration for each magnetometer axis
  uint8_t rawData[3];  // x/y/z gyro calibration data stored here
  writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer
  wait(1000);
  writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x0F); // Enter Fuse ROM access mode
  wait(1000);
  readBytes(AK8963_ADDRESS, AK8963_ASAX, 3, &rawData[0]);  // Read the x-, y-, and z-axis calibration values
  destination[0] =  ((float)(rawData[0] - 128))/256.0 + 1.0;   // Return x-axis sensitivity adjustment values, etc.
  destination[1] =  ((float)(rawData[1] - 128))/256.0 + 1.0;
  destination[2] =  ((float)(rawData[2] - 128))/256.0 + 1.0;
  writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer
  wait(100);
  // Configure the magnetometer for continuous read and highest resolution
  // set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
  // and enable continuous mode data acquisition Mmode (bits [3:0]), 0010 for 8 Hz and 0110 for 100 Hz sample rates
  writeByte(AK8963_ADDRESS, AK8963_CNTL, Mscale << 4 | Mmode); // Set magnetometer data resolution and sample ODR
  wait(1000);
}

void get_offset_value(void){//get 1000 times average offset value
    int cnt;
    for (cnt = 0; cnt < 1000; cnt++)
    {
        readGyroData(g_xyz);
        gyro_x_cal += g_xyz[0];
        gyro_y_cal += g_xyz[1];
        gyro_z_cal += g_xyz[2];
        wait(100);
    }

    gyro_x_cal /= 1000; //Divide the gyro_x_cal variable by 2000 to get the avarage offset
    gyro_y_cal /= 1000; //Divide the gyro_y_cal variable by 2000 to get the avarage offset
    gyro_z_cal /= 1000;


}
void get_YPR(void){

    readAccelData(a_xyz);
    readGyroData(g_xyz);
   //Subtract the offset values from the raw gyro values
    g_xyz[0] -= gyro_x_cal;
    g_xyz[1] -= gyro_y_cal;
    g_xyz[2] -= gyro_z_cal;

    //Gyro angle calculations . Note 0.0000611 = 1 / (250Hz x 65.5) =0.0000611
    angle_pitch += g_xyz[0] * 0.0000611;                                   //Calculate the traveled pitch angle and add this to the angle_pitch variable
    angle_roll += g_xyz[1] * 0.0000611;                                    //Calculate the traveled roll angle and add this to the angle_roll variable
    //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians     0.0000002664
    angle_pitch += angle_roll * sin( ((float)g_xyz[2]) * 0.000001066);               //If the IMU has yawed transfer the roll angle to the pitch angel
    angle_roll -= angle_pitch * sin( ((float)g_xyz[2]) * 0.000001066);               //If the IMU has yawed transfer the pitch angle to the roll angel

    //Accelerometer angle calculations
    acc_total_vector = sqrt( pow(a_xyz[0],2)+pow(a_xyz[1],2)+pow(a_xyz[2],2) );  //Calculate the total accelerometer vector
    //57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
    angle_pitch_acc = asin( (float)a_xyz[1]/acc_total_vector) * 57.296;       //Calculate the pitch angle
    angle_roll_acc = asin( (float)a_xyz[0]/acc_total_vector) * -57.296;       //Calculate the roll angle

    roll = angle_roll_acc  * D2R;
    pitch = angle_pitch_acc * D2R;
    yaw += g_xyz[2] * t * D2R;


}
void readAccelData(int16_t* destination)
{
    uint8_t rawData[6];  // x/y/z accel register data stored here
    readBytes(MPU9250_ADDRESS, ACCEL_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers into data array
    destination[0] = ((int16_t)rawData[0] << 8) | rawData[1] ;  // Turn the MSB and LSB into a signed 16-bit value
    destination[1] = ((int16_t)rawData[2] << 8) | rawData[3] ;
    destination[2] = ((int16_t)rawData[4] << 8) | rawData[5] ;
}


void readGyroData(int16_t * destination)
{
     uint8_t rawData[6];  // x/y/z gyro register data stored here
     readBytes(MPU9250_ADDRESS, GYRO_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
     destination[0] = ((int16_t)rawData[0] << 8) | rawData[1] ;  // Turn the MSB and LSB into a signed 16-bit value
     destination[1] = ((int16_t)rawData[2] << 8) | rawData[3] ;
     destination[2] = ((int16_t)rawData[4] << 8) | rawData[5] ;
}

void readMagData(int16_t * destination)
{
  uint8_t rawData[7];  // x/y/z gyro register data, ST2 register stored here, must read ST2 at end of data acquisition
  if(readByte(AK8963_ADDRESS, AK8963_ST1) & 0x01) { // wait for magnetometer data ready bit to be set
    readBytes(AK8963_ADDRESS, AK8963_XOUT_L, 7, &rawData[0]);  // Read the six raw data and ST2 registers sequentially into data array
    uint8_t c = rawData[6]; // End data read by reading ST2 register
    if(!(c & 0x08)) { // Check if magnetic sensor overflow set, if not then report data
      destination[0] = ((int16_t)rawData[1] << 8) | rawData[0] ;  // Turn the MSB and LSB into a signed 16-bit value
      destination[1] = ((int16_t)rawData[3] << 8) | rawData[2] ;  // Data stored as little Endian
      destination[2] = ((int16_t)rawData[5] << 8) | rawData[4] ;
    }
  }
}

#endif /* INCLUDE_MPU9250_H_ */
