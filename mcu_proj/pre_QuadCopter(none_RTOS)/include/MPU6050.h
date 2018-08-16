/*
 * MPU6050.h
 *
 *  Created on: 2018. 8. 13.
 *      Author: HyunwooPark
 */

#ifndef INCLUDE_MPU6050_H_
#define INCLUDE_MPU6050_H_

#include "MPU9250.h"

int acc_l_x, acc_l_y, acc_l_z;
int acc_h_x, acc_h_y, acc_h_z;

double real_acc_x, real_acc_y, real_acc_z;

int16_t l_gyro_x, l_gyro_y, l_gyro_z;
int16_t h_gyro_x, h_gyro_y, h_gyro_z;
double real_gyro_x, real_gyro_y, real_gyro_z;

int16_t l_roll, l_pitch, l_yaw;
int16_t h_roll, h_pitch, h_yaw;
double real_roll, real_pitch, real_yaw;

void get_mpu6050_RPY(void){
    char txt_buf[256] = {0};
    unsigned int buf_len;

     sciSendByte(sciREG3, 0x61); // using the serial port , disable I2C

        if (sciReceiveByte(sciREG3) == 0x53)
        {
            /*roll*/
            l_roll = sciReceiveByte(sciREG3);
            h_roll = sciReceiveByte(sciREG3);
            h_roll <<= 8;
            h_roll |= l_roll;
            real_roll = ((double) h_roll) / 32768.0 * 180;
            /*pitch*/
            l_pitch = sciReceiveByte(sciREG3);
            h_pitch = sciReceiveByte(sciREG3);
            h_pitch <<= 8;
            h_pitch |= l_pitch;
            real_pitch = ((double) h_pitch) / 32768.0 * 180;
            /*yaw*/
            l_yaw = sciReceiveByte(sciREG3);
            h_yaw = sciReceiveByte(sciREG3);
            h_yaw <<= 8;
            h_yaw |= l_yaw;
            real_yaw = ((double) h_yaw) / 32768.0 * 180;

//            sprintf(txt_buf, "roll = %lf \t pitch = %lf \t yaw = %lf \n\r\0",
//                    real_roll, real_pitch, real_yaw);
//
//            buf_len = strlen(txt_buf);
//            sciDisplayText(sciREG1, (uint8 *) txt_buf, buf_len);
        }

}
void get_mpu6050_gyro(void){
    char txt_buf[256] = {0};
    unsigned int buf_len;

     sciSendByte(sciREG3, 0x61); // using the serial port , disable I2C

        if(sciReceiveByte(sciREG3) == 0x52)
        {
            /*gyro_x*/
            l_gyro_x = sciReceiveByte(sciREG3);
            h_gyro_x = sciReceiveByte(sciREG3);
            h_gyro_x <<= 8;
            h_gyro_x |= l_gyro_x;
            real_gyro_x = ((double) h_gyro_x) / 32768.0 * 2000;
            /*gyro_y*/
            l_gyro_y = sciReceiveByte(sciREG3);
            h_gyro_y = sciReceiveByte(sciREG3);
            h_gyro_y <<= 8;
            h_gyro_y |= l_gyro_y;
            real_gyro_y = ((double) h_gyro_y) / 32768.0 * 2000;

            /*gyro_z*/
            l_gyro_z = sciReceiveByte(sciREG3);
            h_gyro_z = sciReceiveByte(sciREG3);
            h_gyro_z <<= 8;
            h_gyro_z |= l_gyro_z;
            real_gyro_z = ((double) h_gyro_z) / 32768.0 * 2000;

//            sprintf(txt_buf, "gyro_x = %lf \t gyro_y = %lf \t gyro_z = %lf \n\r\0",
//                    real_gyro_x, real_gyro_y, real_gyro_z);
//
//            buf_len = strlen(txt_buf);
//            sciDisplayText(sciREG1, (uint8 *) txt_buf, buf_len);
        }
}

#endif /* INCLUDE_MPU6050_H_ */
