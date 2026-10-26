/*
 * motor.h
 *
 *  Created on: 2026��4��10��
 *      Author: dada
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_


#include "zf_common_headfile.h"


#define MOTOR1_PWM   ATOM2_CH4_P11_9    //����1���PWM����
#define MOTOR1_DIR   P11_10   //������1DIR
#define MOTOR2_PWM   ATOM3_CH6_P11_11    //����2���PWM����
#define MOTOR2_DIR   P11_12   //������2DIR
#define DIR        0   //�����ת

#define NEGATIVE_PRESSURE_PWM        ATOM0_CH0_P21_2
#define NEGATIVE_PRESSURE_PWM_FREQ   100
#define NEGATIVE_PRESSURE_MIN_US     1000
#define NEGATIVE_PRESSURE_MAX_US     2000

void motor_init(void);
void motor_left(int32 duty_m1);
void motor_right(int32 duty_m2);
void motor_control(int MotorR_PWM, int MotorL_PWM);
void negative_pressure_init(void);
void negative_pressure_set_us(uint16 pulse_us);
void negative_pressure_set_percent(uint8 percent);
void negative_pressure_stop(void);

#endif /* CODE_MOTOR_H_ */


