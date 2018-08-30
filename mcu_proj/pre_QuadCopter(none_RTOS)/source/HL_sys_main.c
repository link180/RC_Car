#include "MPU9250.h"
#include "MPU6050.h"
#include "Matrix.h"
#include "QuadCopter.h"
#include "MS5611.h"



max1 torque;  // 모터 토크
max1 v_dot; // 기체좌표계 가속도
max1 eta_ref; // 각가속도
max1 p_2dot; // 가속도

int i;

float real_elev,real_rudd,real_ail,throttle;  // 조종기 전후진, 좌우회전, 제자리 회전, 스로틀
float quad_start; // 쿼드콥터 켜기



int main(void)
{

    char txt_buf[256] = { 0 };
    unsigned int buf_len;

    sciInit();

    disp_set("SCI Configuration Success!!\n\r\0");

    gioInit();

    disp_set("GIO Init Success!!\n\r\0");

    i2cInit();
    wait(10000000);
    disp_set("I2C Init Success!!\n\r\0");

    etpwmInit();

    sprintf(txt_buf, "ETPWM Configuration Success!!\n\r");
    buf_len = strlen(txt_buf);
    sciDisplayText(UART, (uint8 *) txt_buf, buf_len);

    etpwmStartTBCLK();
    wait(10000);



    uint8 c = readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250); // MPU9250 인식이 되어 제대로 값이 읽어져 오면 0x71이 읽힘.
    sprintf(txt_buf, "I AM = %x\n\r\0", c);
    buf_len = strlen(txt_buf);
    sciDisplayText(sciREG1, (uint8 *) txt_buf, buf_len);



    if (c == 0x71)
    {
        Init_MS5611(MS5611_STANDARD);

        disp_set("Init MS5611 Success!!\n\r\0");

        calibrateMPU9250(gyroBias, accelBias); // gyro, acc 평균값 셋팅을 위한 함수
        disp_set("MPU9250 calibration Success!!!!!!\n\r\0");

        initMPU9250(); // MPU9250 초기화
        disp_set("MPU9250 Init Success!!!!!!\n\r\0");

        initAK8963(magCalibration); // 지자기 초기화
        disp_set("MPU9250 AK8963 Init Success!!!!!!\n\r\0");

        get_offset_value(); // gyro 값을 1000번을 읽어 평균값을 구하는 함수
        disp_set("gyro_offset_setting Success!!\n\r\0");

        initMotorPWM();
        disp_set("Init MOTOR_PWM Success!!\n\r\0");

        ecapInit();
        _enable_interrupt_();

        ecapStartCounter(ecapREG1);
        ecapStartCounter(ecapREG2);
        ecapStartCounter(ecapREG3);
        ecapStartCounter(ecapREG4);
        ecapEnableCapture(ecapREG1);
        ecapEnableCapture(ecapREG2);
        ecapEnableCapture(ecapREG3);
        ecapEnableCapture(ecapREG4);
        disp_set("Init Controller Success!!\n\r\0");

        initYPR();
        get_Iv();
        get_inv_T();

        /*고도 제어 할 때 신경써야 할 부분임. 속도 값을 알아야 함. MPU9250 가속도값을 적분해서 쓰면 될 것 같긴함.*/
        p_2dot.x = 0;
        p_2dot.y = 0;
        p_2dot.z = 0;

        disp_set("Init YPR Success!!\n\r\0");
//        gioSetDirection(hetPORT1, 0xFFFFFFFF);  // 모든게 완료 되면 보드에 전원.
        wait(1000000);
    }
    else
    {
        disp_set("MPU9250 doesn`t work!!!");
        while (1)
            ;
    }
//    disp_set("quad start press: 0\t quad stop press : 1!!\n\r\0");
    for (;;)   // loop()
    {
        /*mpu6050 밑에 자이로와 각도는 확실히 됨.*/
//         get_mpu6050_RPY();
//         get_mpu6050_gyro();


        h_cal_controller(real_elev, real_rudd, real_ail,quad_start); // 조종기 히스테리시스 고려.

        if (stop_n_start == 1) // stop_n_start == 1
        {

            if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01) // 인터럽트가 활성화 확인
            {

                readAccelData(accelCount);  // Read the x/y/z adc values
                getAres();  // scale을 계산해주는 함수

                ax = (float) accelCount[0] * aRes; // - accelBias[0];  // get actual g value, this depends on scale being set
                ay = (float) accelCount[1] * aRes; // - accelBias[1];
                az = (float) accelCount[2] * aRes; // - accelBias[2];

                readGyroData(gyroCount);  // Read the x/y/z adc values
                getGres();

                // Calculate the gyro value into actual degrees per second
                gx = (float) gyroCount[0] * gRes; // get actual gyro value, this depends on scale being set
                gy = (float) gyroCount[1] * gRes;
                gz = (float) gyroCount[2] * gRes;

                /*roll(x)와 pitch(y) yaw(z) 의 각도*/
                get_YPR();


//                sprintf(txt_buf, "roll = %d \t pitch = %d \t yaw = %d \n\r\0",
//                        (int) angle_pitch_acc, (int) angle_roll_acc, (int)(yaw * R2D));
//
//                buf_len = strlen(txt_buf);
//                sciDisplayText(sciREG1, (uint8 *) txt_buf, buf_len);
//                wait(3000000);

                /*sensor angle & gyro & gyro_dot for mpu9250*/
                roll_angle_in = roll;
                roll_rate_in = gx;
                pitch_angle_in = pitch;
                pitch_rate_in = gy;
                yaw_angle_in = yaw;
                yaw_rate_in = gz;
                eta_ref.x = roll_output;
                eta_ref.y = pitch_output;
                eta_ref.z = yaw_output;


                /*QuadCopter Math & PWM for mpu9250*/
                calcYPRtoDualPID();
                get_C(roll, pitch, yaw, &C);


//            sprintf(txt_buf, "C1 = %f \t C2 = %f \t C3= %f \n\r  C4 = %f \t C5 = %f \t C6= %f \n\r  C7 = %f \t C8 = %f \t C9= %f \n\r\n\r",
//                    C.x[0],C.x[1],C.x[2],C.y[0],C.y[1],C.y[2],C.z[0],C.z[1],C.z[2]);
//            buf_len = strlen(txt_buf);
//            sciDisplayText(UART, (uint8 *) txt_buf, buf_len);
                get_t_R(roll, pitch, yaw, &t_R);
//                        sprintf(txt_buf, "t1 = %f \t t2 = %f \t t3= %f \n\r  t4 = %f \t t5 = %f \t t6= %f \n\r  t7 = %f \t t8 = %f \t t9= %f \n\r\n\r",
//                                t_R.x[0], t_R.x[1], t_R.x[2],
//                                t_R.y[0], t_R.y[1], t_R.y[2],
//                                t_R.z[0], t_R.z[1], t_R.z[2]);
//                        buf_len = strlen(txt_buf);
//                        sciDisplayText(UART, (uint8 *) txt_buf, buf_len);

                torque = torque_conv(Iv, C, eta_ref);
//                          sprintf(txt_buf, "t1 = %f \t t2 = %f \t t3 = %f \n\r", torque.x, torque.y, torque.z);
//                          buf_len = strlen(txt_buf);
//                          sciDisplayText(UART, (uint8 *) txt_buf, buf_len);
                v_dot = force_conv(t_R, p_2dot);

                get_motor_thrust(torque, v_dot, inv_T, U, F);

//            for(i=0; i<16;i++){
//                if (i % 4 == 0)
//                {
//                    sprintf(txt_buf, "\n\r");
//                    buf_len = strlen(txt_buf);
//                    sciDisplayText(UART, (uint8 *) txt_buf, buf_len);
//                }
//                sprintf(txt_buf, "inv[%d] = %f \t", i, inv_T[i]);
//                buf_len = strlen(txt_buf);
//                sciDisplayText(UART, (uint8 *) txt_buf, buf_len);
//            }

//
//             sprintf(txt_buf, "F0 = %f \t F1 = %f \t F2 = %f \t F3 = %f\n\r", F[0], F[1], F[2], F[3]);
//             buf_len = strlen(txt_buf);
//             sciDisplayText(UART, (uint8 *) txt_buf, buf_len);

                calcMotorPWM();



//                sprintf(txt_buf,
//                        "pitch_target_angle = %f\t roll_target_angle = %f\t yaw_target_angle = %f \n\r",
//                        pitch_target_angle * R2D, roll_target_angle * R2D,
//                        yaw_target_angle * R2D);
//                buf_len = strlen(txt_buf);
//                sciDisplayText(UART, (uint8 *) txt_buf, buf_len);
            }
        }

        if (stop_n_start == -1)
        {
            pwmSet();
            disp_set("Quad is stopped!!\n\r\0");
        }

        if (stop_n_start == 0)
        {
            temp = readTemperature(true);
            pressure = readPressure(true);
            altitude = getAltitude(pressure, 101110); // 서울의 해수면 높이

            sprintf(txt_buf,
                    "altitude = %lf \t pressure = %ld \t temperature = %lf \n\r\0",
                    altitude, pressure, temp);

            buf_len = strlen(txt_buf);
            sciDisplayText(sciREG1, (uint8 *) txt_buf, buf_len);
        }

    }

}

void ecapNotification(ecapBASE_t *ecap, uint16 flags)
{
    uint32 cap[10];
//    char txt_buf[256] = { 0 };
//    unsigned int buf_len;

    cap[0] = ecapGetCAP1(ecapREG1);
    cap[1] = ecapGetCAP2(ecapREG1);
    cap[2] = ecapGetCAP1(ecapREG2);
    cap[3] = ecapGetCAP2(ecapREG2);
    cap[4] = ecapGetCAP1(ecapREG3);
    cap[5] = ecapGetCAP2(ecapREG3);
    cap[6] = ecapGetCAP1(ecapREG4);
    cap[7] = ecapGetCAP2(ecapREG4);
    cap[8] = ecapGetCAP1(ecapREG5);
    cap[9] = ecapGetCAP2(ecapREG5);

    real_ail = (cap[3] - cap[2]) * 1000 / VCLK3_FREQ * 0.00018 -268; // ecap2  channel 1
    real_elev = (cap[1] - cap[0]) * 1000 / VCLK3_FREQ * 0.00018 -273;  // ecap1  channel 2
    throttle = (cap[5] - cap[4]) * 1000 / VCLK3_FREQ / 1000; // ecap3   channel 3
    real_rudd = (cap[7] - cap[6]) * 1000 / VCLK3_FREQ * 0.00018 -274.5; //ecap4     channel 4
    quad_start = (cap[9] - cap[8]) * 1000 / VCLK3_FREQ / 1000;


//    sprintf(txt_buf, "elev = %f ms\t rudd = %f ms\t ail = %f ms\t thro = %f ms \t start = %f ms\n\r", real_elev,real_rudd,real_ail,throttle,quad_start);
//    buf_len = strlen(txt_buf);
//    sciDisplayText(UART, (uint8 *) txt_buf, buf_len);
}
