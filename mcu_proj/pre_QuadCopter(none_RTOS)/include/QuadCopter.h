/*
 * QuadCopter.h
 *
 *  Created on: 2018. 8. 13.
 *      Author: HyunwooPark
 */

#ifndef INCLUDE_QUADCOPTER_H_
#define INCLUDE_QUADCOPTER_H_

#include "MPU9250.h"
#include "Matrix.h"

#include <stdio.h>
#include <time.h>

#define THROTTLE_MAX    1750
#define THROTTLE_MIN    1080

#define QUAD_MASS  2.2
#define L  0.225 // length of arm
#define KT  0.0011275   // Thrust Coef
#define KD  0.0000983489586    // Moment Coef
#define GRAVITY  9.81
#define M_R   (KT / KD)  // 힘과 모멘트의 상관관계 계수


/*제어기 관련 변수*/
max1 m_3x1_eta; // inertia frame에서의 각도
max1 m_3x1_eta_ref; //inertia frame에서의 각가속도
max3 C, R;  // rotation matrix, Translation matrix
max3 Iv;
max3 t_R;


/* matrix inverse T*/
float m[16];
float inv_T[16];


/*입력 U, 출력 F*/
float U[4] = {0};
float F[4] = {0};

/*모터별 속도 관련 변수*/
float motorA_PWM, motorB_PWM, motorC_PWM, motorD_PWM;//각 모터별 RPM
float w_A , w_B  , w_C  , w_D ;//각 모터별 각속도
float VoltA, VoltB, VoltC, VoltD;
//float throttle = 1250.0; // PWM

/*Uart */
uint32 rx_data =0;
uint32 tmp =0;
uint32 value =0;


/*쿼드로터 각도 관련 변수*/
extern float roll;
extern float pitch;
extern float yaw;

extern float init_pitch;
extern float init_roll;
extern float init_yaw;
//시간관련 값//
float dt = 0.0025;
unsigned long t_now;
unsigned long t_prev;

/*조종기 관련 변수*/
extern float real_elev,real_rudd,real_ail,throttle;  // 조종기 전후진, 좌우회전, 제자리 회전, 스로틀
static int stop_n_start = -1;

//이중루프PID에서 제어할 변수들(Roll, Pitch, Yaw)//
float roll_target_angle = 0.0;
float roll_angle_in;
float roll_rate_in;
float roll_stabilize_kp = 3.5;  // angle kp
float roll_stabilize_ki = 0;  // angle ki
float roll_rate_kp = 0.004;  // angle rate kp setting
float roll_rate_ki = 0.0035;  // angle rate ki setting
float roll_rate_kd = 0.0006;  // angle rate kd setting
float roll_prev_rate_error = 0;
float roll_stabilize_iterm;
float roll_rate_iterm;
float roll_output;

float pitch_target_angle = 0.0;
float pitch_angle_in;
float pitch_rate_in;
float pitch_stabilize_kp = 3.5;
float pitch_stabilize_ki = 0;
float pitch_rate_kp = 0.004;
float pitch_rate_ki = 0.0035;
float pitch_rate_kd = 0.0006;
float pitch_prev_rate_error = 0;
float pitch_stabilize_iterm = 0;
float pitch_rate_iterm = 0;
float pitch_output = 0;

float yaw_target_angle = 0.0;
float yaw_angle_in;
float yaw_rate_in;
float yaw_stabilize_kp = 2.5;
float yaw_stabilize_ki = 0;
float yaw_rate_kp = 0.033;
float yaw_rate_ki = 0.0035;
float yaw_rate_kd = 0;
float yaw_prev_rate_error = 0;
float yaw_stabilize_iterm = 0;
float yaw_rate_iterm = 0;
float yaw_output = 0;

void initDT(void);
void calcDT(void);

void initYPR(void);
void dualPID(float target_angle, float angle_in, float rate_in,
             float stabilize_kp, float stabilize_ki, float rate_kp,
             float rate_ki, float rate_kd, float *prev_rate_error, float *stabilize_iterm, float *rate_iterm,
             float *output);
void initMotorPWM(void);
void get_C(float phi, float theta, float psi, max3 *C);
void initMotorPWM(void);
void get_C(float phi, float theta, float psi, max3 *C);
void get_Iv(void);
void h_cal_controller(float elev, float rudd, float ail, float sns);


/*초기 시간 값*/
void initDT(void){
  t_prev = clock(); //초기 t_prev값은 근사값//
}
/*구동 시간 계산*/
void calcDT(void){
    char txt_buf[256] = {0};
    unsigned int buf_len;

  t_now = clock();
  dt = (double)(t_now - t_prev)/180000000000.0 ;
//  t_prev = t_now;

  sprintf(txt_buf, "\t %lf \n\r\0",
          dt);

  buf_len = strlen(txt_buf);
  sciDisplayText(sciREG1, (uint8 *) txt_buf, buf_len);
}

void initYPR(void)
{
    /*for mpu9250*/
    get_YPR();

    init_roll = roll;
    init_pitch = pitch;
    init_yaw = yaw;
    roll_target_angle = init_roll;
    pitch_target_angle = init_pitch;
    yaw_target_angle = init_yaw;

    /*for mpu6050*/
//    get_mpu6050_RPY();
//    roll_target_angle = real_roll;
//    pitch_target_angle = real_pitch;
//    yaw_target_angle = real_yaw;
}

/*이중 PID 쿼드 제어에 효과적. angle의 오차를 목표로 두고 제어하므로 더 안정적이다.*/
void dualPID(float target_angle, float angle_in, float rate_in,
             float stabilize_kp, float stabilize_ki, float rate_kp,
             float rate_ki, float rate_kd, float *prev_rate_error, float *stabilize_iterm, float *rate_iterm,
             float *output){

    float angle_error;
    float desired_rate;
    float rate_error;

    float stabilize_pterm, rate_pterm, rate_dterm;


    //이중루프PID알고리즘//
    angle_error = target_angle - angle_in;

    stabilize_pterm = stabilize_kp * angle_error;
    (*stabilize_iterm) += stabilize_ki * angle_error * dt; //안정화 적분항//

    desired_rate = stabilize_pterm;

    rate_error = desired_rate - rate_in;

    rate_pterm = rate_kp * rate_error; //각속도 비례항//
    (*rate_iterm) += rate_ki * rate_error * dt; //각속도 적분항//
    rate_dterm = rate_kd * (rate_error - *prev_rate_error) / dt;
    (*prev_rate_error) = rate_error;
    (*output) = rate_pterm + (*rate_iterm) + (*stabilize_iterm) + rate_dterm; //최종 출력 : 각속도 비례항 + 각속도 적분항 + 안정화 적분항//

}



/* 각 motor RPM 초기화*/
void initMotorPWM(void){
    w_A = 0.0; w_B = 0.0; w_C = 0.0; w_D = 0.0;
    motorA_PWM = 1000.0;
    motorB_PWM = 1000.0;
    motorC_PWM = 1000.0;
    motorD_PWM = 1000.0;

}


void get_C(float phi, float theta, float psi, max3 *C){ // roll, pitch, yaw

    C->x[0] = 1;
    C->x[1] = 0;
    C->x[2] = -sin(theta);

    C->y[0] = 0;
    C->y[1] = cos(phi);
    C->y[2] = sin(phi) * cos(theta);

    C->z[0] = 0;
    C->z[1] = -sin(phi);
    C->z[2] = cos(phi) * cos(theta);
}

void get_Iv(void){ // quadrotor inertia moments
    Iv.x[0] = 0.0325325;
    Iv.x[1] = 0;
    Iv.x[2] = 0;

    Iv.y[0] = 0;
    Iv.y[1] = 0.0325325;
    Iv.y[2] = 0;

    Iv.z[0] = 0;
    Iv.z[1] = 0;
    Iv.z[2] = 0.057845;
}

max1 torque_conv(max3 Iv, max3 C, max1 eta_ref){
    max1 torque;
    max3 temp;

    max3_3_by_3_mul(Iv, C, &temp);
    max1_3by3_x_3by1(temp, eta_ref, &torque);

    return torque;
}

void get_t_R(float phi, float theta, float psi, max3 *t_R){ // roll, pitch, yaw --  get transpose R

    t_R->x[0] = cos(psi) * cos(theta);
    t_R->x[1] = sin(psi) * cos(theta);
    t_R->x[2] = -sin(theta);

    t_R->y[0] = cos(psi)*sin(theta)*sin(phi) - sin(psi)*cos(phi);
    t_R->y[1] = sin(psi)*sin(theta)*sin(phi) + cos(psi) * cos(phi);
    t_R->y[2] = cos(theta) * sin(phi);

    t_R->z[0] = cos(psi)*sin(theta)*cos(phi) + sin(psi) * sin(phi);
    t_R->z[1] = sin(psi)*sin(theta)*cos(phi) - cos(psi)*sin(phi);
    t_R->z[2] = cos(theta)*cos(phi);

}

max1 force_conv(max3 t_R, max1 p_2dot){
    max1 v_dot;
    max1 u;
    u.x = 0;
    u.y = 0;
    u.z = p_2dot.z + GRAVITY; // 고도센서로 높이 제어할 때 건드릴 예정.

    max1_3by3_x_3by1(t_R, u, &v_dot);
    return v_dot;
}


void get_inv_T(void){
    /*1행*/
    m[0] = 1 / QUAD_MASS;
    m[1] = 1 / QUAD_MASS;
    m[2] = 1 / QUAD_MASS;
    m[3] = 1 / QUAD_MASS;
    /*2행*/
    m[4] = 0;
    m[5] = -L;
    m[6] = 0;
    m[7] = L;
    /*3행*/
    m[8] = -L;
    m[9] = 0;
    m[10] = L;
    m[11] = 0;
    /*4행*/
    m[12] = M_R;
    m[13] = -M_R;
    m[14] = M_R;
    m[15] = -M_R;

    invertColumnMajor(m, inv_T);
}

void get_motor_thrust(max1 torque, max1 v_dot,float inverse_T[16], float U[4], float F[4]){

    float min =0;
    float max =20;

    U[0] = v_dot.z;
    U[1] = torque.x;
    U[2] = torque.y;
    U[3] = torque.z;

    m_4x4_4x1(inverse_T, U, F);

    if(F[0] < min) F[0] = min;
    if(F[1] < min) F[1] = min;
    if(F[2] < min) F[2] = min;
    if(F[3] < min) F[3] = min;

    if(F[0] > max) F[0] = max;
    if(F[1] > max) F[1] = max;
    if(F[2] > max) F[2] = max;
    if(F[3] > max) F[3] = max;

}
void calcMotorPWM(void){
    char txt_buf[256] = { 0 };
    unsigned int buf_len;

    w_A = sqrt(F[0] / KT);
    w_B = sqrt(F[1] / KT);
    w_C = sqrt(F[2] / KT);
    w_D = sqrt(F[3] / KT);

  VoltA = (w_A * 60) / (2 * M_PI * 320);  // w -> rpm
  VoltB = (w_B * 60) / (2 * M_PI * 320);
  VoltC = (w_C * 60) / (2 * M_PI * 320);
  VoltD = (w_D * 60) / (2 * M_PI * 320);

  motorA_PWM = throttle + VoltA * 45.045 - real_elev + real_rudd - real_ail ; // 1000 + (VoltA / 22.2V)(duty)  * 1000 -> PWM
  motorB_PWM = throttle + VoltB * 45.045 + real_elev + real_rudd + real_ail ;
  motorC_PWM = throttle + VoltC * 45.045 + real_elev - real_rudd - real_ail ;
  motorD_PWM = throttle + VoltD * 45.045 - real_elev - real_rudd + real_ail ;

  //PWM값은 1000~2000이므로 각 경계값마다의 보정작업, 내 모터는 토크가 워낙 쎄기 때문에 최대 1500으로 일단 고정.//
  if(motorA_PWM < THROTTLE_MIN){
      motorA_PWM = THROTTLE_MIN;
  } if(motorA_PWM > THROTTLE_MAX){
      motorA_PWM = THROTTLE_MAX;
  } if(motorB_PWM < THROTTLE_MIN){
      motorB_PWM = THROTTLE_MIN;
  } if(motorB_PWM > THROTTLE_MAX){
      motorB_PWM = THROTTLE_MAX;
  } if(motorC_PWM < THROTTLE_MIN){
      motorC_PWM = THROTTLE_MIN;
  } if(motorC_PWM > THROTTLE_MAX){
      motorC_PWM = THROTTLE_MAX;
  } if(motorD_PWM < THROTTLE_MIN){
      motorD_PWM = THROTTLE_MIN;
  } if(motorD_PWM > THROTTLE_MAX){
      motorD_PWM = THROTTLE_MAX;
  }
    sprintf(txt_buf, "pwmA = %f \t pwmB = %f \t pwmC = %f \t pwmD= %f \n\r\0",
            motorA_PWM, motorB_PWM, motorC_PWM, motorD_PWM);

    buf_len = strlen(txt_buf);
    sciDisplayText(sciREG1, (uint8 *) txt_buf, buf_len);

    etpwmSetCmpA(etpwmREG1, motorA_PWM);    // front ccw
    etpwmSetCmpA(etpwmREG2, motorB_PWM);  // rear cw
    etpwmSetCmpA(etpwmREG3, motorC_PWM);    // rear ccw
    etpwmSetCmpA(etpwmREG4, motorD_PWM);  // front cw

}

void calcYPRtoDualPID(void){

//    char txt_buf[256] = { 0 };
//    unsigned int buf_len;

    dualPID(roll_target_angle,
              roll_angle_in,
              roll_rate_in,
              roll_stabilize_kp,
              roll_stabilize_ki,
              roll_rate_kp,
              roll_rate_ki,
              roll_rate_kd,
              &roll_prev_rate_error,
              &roll_stabilize_iterm,
              &roll_rate_iterm,
              &roll_output
      );

      dualPID(pitch_target_angle,
              pitch_angle_in,
              pitch_rate_in,
              pitch_stabilize_kp,
              pitch_stabilize_ki,
              pitch_rate_kp,
              pitch_rate_ki,
              pitch_rate_kd,
              &pitch_prev_rate_error,
              &pitch_stabilize_iterm,
              &pitch_rate_iterm,
              &pitch_output
      );


      dualPID(yaw_target_angle,
              yaw_angle_in,
              yaw_rate_in,
              yaw_stabilize_kp,
              yaw_stabilize_ki,
              yaw_rate_kp,
              yaw_rate_ki,
              yaw_rate_kd,
              &yaw_prev_rate_error,
              &yaw_stabilize_iterm,
              &yaw_rate_iterm,
              &yaw_output
      );

//      sprintf(txt_buf, "r_t = %f \t p_t = %f \t y_t = %f \n\r\0",
//              roll_target_angle * R2D, pitch_target_angle * R2D, yaw_target_angle * R2D);
//
//      buf_len = strlen(txt_buf);
//      sciDisplayText(sciREG1, (uint8 *) txt_buf, buf_len);
}

void pwmSet(){
    value = 1000;
    etpwmSetCmpA(etpwmREG1, value);
    etpwmSetCmpA(etpwmREG2, value);
    etpwmSetCmpA(etpwmREG3, value);
    etpwmSetCmpA(etpwmREG4, value);
    wait(10000);
}

void itoa(int num, char *c){  // integer to ascii
    int radix = 10;
    int count = 0, degree = 1;
    int i;
    while(true){
        if(num/degree > 0 ) count++;
        else break;
        degree *= radix;
    }

    degree/=radix;

    for( i=0;i<count;i++){
        *(c+i) = (num/degree) + '0';
        num -=(num/degree) * degree;
        degree /=radix;
    }

    *(c+count) = '\0';
}

void h_cal_controller(float elev, float rudd, float ail, float sns){

    /*pitch 제어*/
    if (elev < 1 && elev > -1)// 히스테리시스를 고려함.
    {
        real_elev = 0.0;
    }

    /*roll 제어*/
    if(rudd < 1 && rudd > -1){
        real_rudd = 0.0;
    }


    /* yaw 제어 */
    if (ail < 1 && ail > -1)  // yaw는 회전하면 그 회전한 각도를 타겟으로 각도가 바뀌어야 함.
    {
        real_ail = 0.0;
    }

    /*start and stop*/
     if (sns < 1000.0){ //start
         stop_n_start = 1;
     }
     else if(sns > 1900.0){ // stop
         stop_n_start = -1;
     }else{ // 중립
         stop_n_start = 0;

     }

}

#endif
