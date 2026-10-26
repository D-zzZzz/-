#include "zf_common_headfile.h"

#define MOTOR1_FORWARD_LEVEL  1
#define MOTOR2_FORWARD_LEVEL  1
#define FINAL_AUTO_STOP_DISTANCE_CM  100.0f



void motor_init(void)
{
    pwm_init(MOTOR1_PWM, 17000, 0);
    gpio_init(MOTOR1_DIR, GPO, MOTOR1_FORWARD_LEVEL, GPO_PUSH_PULL);

    pwm_init(MOTOR2_PWM, 17000, 0);
    gpio_init(MOTOR2_DIR, GPO, MOTOR2_FORWARD_LEVEL, GPO_PUSH_PULL);

    pwm_set_duty(MOTOR1_PWM, 0);
    pwm_set_duty(MOTOR2_PWM, 0);
}
void negative_pressure_init(void)
{
    pwm_init(NEGATIVE_PRESSURE_PWM, NEGATIVE_PRESSURE_PWM_FREQ, 0);
}



void negative_pressure_set_us(uint16 pulse_us)
{
    uint32 duty;

    if (pulse_us < NEGATIVE_PRESSURE_MIN_US)
    {
        pulse_us = NEGATIVE_PRESSURE_MIN_US;
    }
    else if (pulse_us > NEGATIVE_PRESSURE_MAX_US)
    {
        pulse_us = NEGATIVE_PRESSURE_MAX_US;
    }

    duty = (uint32)pulse_us * NEGATIVE_PRESSURE_PWM_FREQ / 100;
    pwm_set_duty(NEGATIVE_PRESSURE_PWM, duty);
}
void negative_pressure_stop(void)
{
    pwm_set_duty(NEGATIVE_PRESSURE_PWM, 0);
}

static int32 limit_pwm_abs(int32 duty)
{
    if(duty > PWM_MAX ) duty = PWM_MAX ;
    if(duty < -PWM_MAX ) duty = -PWM_MAX ;
    return duty;
}

void motor_left(int32 duty_m1)
{
    duty_m1 = limit_pwm_abs(duty_m1);

    if(duty_m1 >= 0)
    {
        gpio_set_level(MOTOR1_DIR, !DIR);
        pwm_set_duty(MOTOR1_PWM, duty_m1);
    }
    else
    {
        gpio_set_level(MOTOR1_DIR, DIR);
        pwm_set_duty(MOTOR1_PWM, -duty_m1);
    }
}

void motor_right(int32 duty_m2)
{
    duty_m2 = limit_pwm_abs(duty_m2);

    if(duty_m2 >= 0)
    {
        gpio_set_level(MOTOR2_DIR, DIR);
        pwm_set_duty(MOTOR2_PWM, duty_m2);
    }
    else
    {
        gpio_set_level(MOTOR2_DIR, !DIR);
        pwm_set_duty(MOTOR2_PWM, -duty_m2);
    }
}

void motor_control(int MotorR_PWM, int MotorL_PWM)
{
    if (Is_Last_Junction_Recognized())
    {
        Motor_Speed_Reset();
        motor_right(0);
        motor_left(0);
        return;
    }
/*
    if (stopline_flag && (Length - Stop_Length) >= FINAL_AUTO_STOP_DISTANCE_CM)
    {
        Motor_Speed_Reset();
        motor_right(0);
        motor_left(0);
        return;
    }*/

    motor_right(MotorR_PWM);
    motor_left(MotorL_PWM);
}

