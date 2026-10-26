/*
 * pid.h
 *
 *  Created on: 2026  4  11
 *      Author: dada
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_

#include "zf_common_headfile.h"

// ====================    ٿ  Ʋ        Ժ  ģ   λƥ 䣡   ====================
#define BASE_SPEED     420    //    ؼ       ֱ   ٶȣ   λ        /5ms           ȡ    ƥ 䣩
#define TURN_LIMIT     233      //    ת   ٶȲ  ֹת  ̫ ͣ
#define SPEED_MAX      1400
#define PWM_MAX          8000
#define DECEL_FACTOR   0      //    ؼ       ϵ    ƫ  Խ   ٶȽ   Խ
#define IMAGE_HEIGHT   120     // ͼ  ߶ȣ            ͷʵ ʷֱ  ʵ
#define PI_MAX 900



#define LENGTH_PER_TICK 0.002f
// ==================== ȫ ֱ          ٶȻ /ת   ã  ====================
extern int high_lost;
extern int target_speed_l;
extern int target_speed_r;
extern int motorL_Set;      //          ٶȣ       /5ms  ת 򻷸
extern int motorR_Set;      //          ٶȣ       /5ms  ת 򻷸
extern int MotorL_PWM;      //         PWM
extern int MotorR_PWM;      //         PWM
extern int Error;            //  Ӿ ƫ ת   ã
extern int Last_Error;       //  ϴ  Ӿ ƫ
extern int Actual_Z;
extern volatile int Actual_Speed_L;
extern volatile int Actual_Speed_R;
extern  float K_ff_L;
extern float K_ff_R;
extern int speed_out;
extern int speed_base;

// ====================          ====================
void Circuit_Control(void);
void Motor_Speed_PID_Loop(void);
void Motor_Speed_Reset(void);
#endif /* CODE_PID_H_ */
