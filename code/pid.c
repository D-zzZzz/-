/*
 * pid.c
 *
 *  Created on: 2026年4月11日
 *      Author: dada
 */

#include "zf_common_headfile.h"

// ==================== 全局变量定义（需外部访问的不加static） ====================
// 1. 转向环 (外环) 变量
int Error = 0;          // 当前图像偏差
int Last_Error = 0;     // 上次图像偏差

// 2. 速度环 (内环) 期望变量
int motorL_Set = 0;     // 左轮期望速度 (给内环的目标值)
int motorR_Set = 0;     // 右轮期望速度 (给内环的目标值)

int high_lost;

int target_speed_l = 0;
int target_speed_r = 0;
// 3. 速度环 (内环) 实际执行变量
int MotorL_PWM = 0;     // 真正给左轮电机的 PWM
int MotorR_PWM = 0;     // 真正给右轮电机的 PWM
//前馈加上pi增量控制
float MotorL_PI_Out = 0;
float MotorR_PI_Out = 0;

volatile int Actual_Speed_L = 0;
volatile int Actual_Speed_R = 0;
static int Actual_Speed_L_Filter = 0;
static int Actual_Speed_R_Filter = 0;

#define STARTUP_RAMP_TICKS 200  // 150  5ms = 1s
static uint16 startup_ramp_tick = 0;

static uint8_t start_diff_lock_cnt = 0;
// ==================== 文件内部静态变量（加static避免命名污染） ====================
int Actual_Z = 0;       // 陀螺仪Z轴角速度（仅内部使用）
int speed_out = 0;      // 融合计算后的最终转向差速
int speed_base = 0;     // 基础直道速度
static int Error_Speed_L = 0, Last_Error_Speed_L = 0; // 左轮速度偏差（仅内部使用）
static int Error_Speed_R = 0, Last_Error_Speed_R = 0; // 右轮速度偏差（仅内部使用）
float K_ff_L = 5.40f;
float K_ff_R = 4.60f;
//float K_ff_L = 5;
//float K_ff_R = 4.55f;

// ==================== 参数配置区 ====================
// 【转向环参数】(逐飞非线性PID)
float Kp_turn = 0.85f; //0.75
float Kp2_turn = 0.033f; //0.028
float Kd_turn = 0.75f; //0.75
float gyro_turn = -0.022f;
// 【速度环参数】(增量式PI)
float KpR_speed = 2.55;
float KiR_speed = 0.042f;
float KpL_speed = 2.42f;
float KiL_speed = 0.042f;
extern float Length;

static int get_weighted_mid_line (int end_y)
{
    static const uint8_t mid_weight_list[IMAGE_HEIGHT] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 6, 6, 9, 9, 9, 9, 9, 15, 15, 15, 15, 16, 15, 15, 15, 15, 15, 10, 10, 10, 10, 10, 10,
            15, 14, 13, 12, 11, 10, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 6, 6, 6, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1};
    static int last_mid_line_value = 94;
    uint32_t weighted_midline_sum = 0;
    uint32_t weight_sum = 0;

    if (end_y > Deal_Top)
    {
        end_y = Deal_Top;
    }
    if (end_y >= IMAGE_HEIGHT)
    {
        end_y = IMAGE_HEIGHT - 1;
    }
    if (end_y < start_center_y)
    {
        return INVALID_POS;
    }

    for (int y = start_center_y; y <= end_y; y++)
    {
        if (mid_line[y] == INVALID_POS || mid_line[y] >= XM)
        {
            continue;
        }

        weighted_midline_sum += (uint32_t) mid_line[y] * mid_weight_list[y];
        weight_sum += mid_weight_list[y];
    }

    if (weight_sum == 0)
    {
        return INVALID_POS;
    }

    int mid_line_value = (int) (weighted_midline_sum / weight_sum);
    mid_line_value = (last_mid_line_value * 2 + mid_line_value * 8) / 10;
    last_mid_line_value = mid_line_value;

    return mid_line_value;
}

void Circuit_Control (void)
{
    // 1. 单点动态前瞻：获取干净的有效图像视距
    int look_ahead = Stop_line;
    /*

     if (IF == curve)
     {
     int curve_length = Stop_line - start_center_y;

     look_ahead = start_center_y + curve_length * 4/ 5;

     if (look_ahead < Deal_Bottom + 5)
     {
     look_ahead = Deal_Bottom + 5;
     }


     if (look_ahead < Deal_Bottom )
     {
     look_ahead = Deal_Bottom + 5;
     }


     }
     else
     {
     if (look_ahead > Deal_Bottom + 5)
     {
     look_ahead -= 5;    // 直线保持远一点
     }

     }
     // 【新增】数组越界边界检查（防止内存越界跑飞）
     if (look_ahead < 0)
     look_ahead = 0;
     if (look_ahead >= IMAGE_HEIGHT)
     look_ahead = IMAGE_HEIGHT - 1;*/

    int weighted_mid = INVALID_POS;

    if (start_point_ok)
    {
        weighted_mid = get_weighted_mid_line(look_ahead);
    }


    else if (!start_point_ok)
    {




            // 其他情况才继承并衰减上一帧
            Error = Last_Error * 9 / 10;

    }

    else if (weighted_mid == INVALID_POS)
    {
        uint8_t ward = Get_Max_White_Column_X();
        uint8_t in_left_turn = (debug_road_state == 0)
                ||  // ROAD_LEFT_TURN
                (A == 21)
                ||               // 第5拐点抢断进入帧
                (A == 24)
                ||               // 左拐进入帧
                (A == 40)
                ||               // 普通左弯
                (A == 41) || (A == 42) || (A == 43) || (A == 45) || (A == 50) || (A == 51) || (A == 52) || (A == 53)
                || (A == 55);

        if (debug_turn_cmd == 9 && !in_left_turn)
        {
            if (ward > XM / 2 + 2)
                Error = 30;
            else if (ward < XM / 2 - 2)
                Error = -30;
            else
            {
                Error = 0;
                Last_Error = 0;
            }

        }
        else if (debug_turn_cmd == 8 && !in_left_turn)
        {
            if (ward > XM / 2 + 2)
                Error = 30;
            else if (ward < XM / 2 - 2)
                Error = -30;
            else
            {
                Error = 0;
                Last_Error = 0;
            }

        }
        else if (debug_turn_cmd == 6 && !in_left_turn)
        {
            if (ward > XM / 2 + 2)
            {
                Error = 30;
                Last_Error = 30;
            }
            else if (ward < XM / 2 - 2)
            {
                Error = -30;
                Last_Error = -30;
            }
            else
            {
                Error = 0;
                Last_Error = 0;
            }

        }
        else
        {
            Error = 0;
            Last_Error = 0;
        }

    }

    else
    {
        if (IF == curve)
        {
            look_ahead -= 2;

            if (look_ahead < 0)
            {
                look_ahead = 0;
            }

            if (mid_line[look_ahead] == INVALID_POS || mid_line[look_ahead] >= XM)
            {
                Error = 0;
                Last_Error = 0;
            }
            else
            {
                Error = mid_line[look_ahead] - 94;
            }
        }
        else
        {
            Error = weighted_mid - 94;
        }
    }
    Actual_Z = Get_Clean_Gyro_Z(); // 请替换为你底层的陀螺仪读取函数

    int abs_error = (Error > 0) ? Error : -Error;

    if (start_diff_lock_cnt < 5)
    {
        start_diff_lock_cnt++;
        speed_out = 30;
        Last_Error = Error;   // 吃掉启动第一帧 D 项冲击
    }
    else
    {
        speed_out = (int) (Kp_turn * Error + Kp2_turn * Error * abs_error + Kd_turn * (Error - Last_Error)
                + gyro_turn * Actual_Z);

        if (speed_out > TURN_LIMIT)
            speed_out = TURN_LIMIT;
        if (speed_out < -TURN_LIMIT)
            speed_out = -TURN_LIMIT;

        // 仅在弯道中限制差速：断点在72以下时保持原输出，越远则逐渐缩小
        /* if (IF == curve)
         {
         const int NEAR_Y = 92;
         const int FAR_Y = 95;
         const int MIN_PERCENT = 80;
         int scale_percent = 100;

         if (hightest >= FAR_Y)
         {
         scale_percent = MIN_PERCENT;
         }
         else if (hightest > NEAR_Y)
         {
         scale_percent = 100 - (hightest - NEAR_Y) * (100 - MIN_PERCENT) / (FAR_Y - NEAR_Y);
         }

         speed_out = speed_out * scale_percent / 100;
         int high_lost;
         if (r_lost_num && l_lost_num)
         {
         high_lost =
         r_center[r_lost_num - 1] > l_center[l_lost_num - 1] ?
         r_center[r_lost_num - 1] : l_center[l_lost_num - 1];
         }
         else if (l_lost_num)
         {
         high_lost = l_center[l_lost_num - 1];
         }
         else
         {
         high_lost = r_center[r_lost_num - 1];
         }

         if (high_lost > 95&&t_lost_num==0)
         speed_out = speed_out * 60 / 100;
         }*/

        high_lost = -1;

        if (IF == curve)
        {

            if (r_lost_num && l_lost_num)
            {
                high_lost =
                        r_center[r_lost_num - 1] > l_center[l_lost_num - 1] ?
                                r_center[r_lost_num - 1] : l_center[l_lost_num - 1];
            }
            else if (l_lost_num)
            {
                high_lost = l_center[l_lost_num - 1];
            }
            else
            {
                high_lost = r_center[r_lost_num - 1];
            }
            const int NEAR_Y = 75;
            const int FAR_Y = 90;
            const int MIN_PERCENT = 70;
            int scale_percent = 100;

            if (high_lost >= FAR_Y)
            {
                scale_percent = MIN_PERCENT;
            }
            else if (high_lost > NEAR_Y)
            {
                scale_percent = 100 - (high_lost - NEAR_Y) * (100 - MIN_PERCENT) / (FAR_Y - NEAR_Y);
            }
            speed_out = speed_out * scale_percent / 100;
            if (high_lost > 97)
                speed_out = speed_out * 50 / 100;
            if (high_lost < 48 && high_lost)
                speed_out = speed_out * 150 / 100;
        }

        Last_Error = Error;
    }

    // 5. 线性刹车法：车头歪得越厉害，基础速度降得越多
    // 【修复】使用pid.h中定义的BASE_SPEED和DECEL_FACTOR，删除致命的*号
    int abs_Error = (Error > 0) ? Error : -Error;
    // 定义一个死区（比如允许 10 个像素的容错）
    // 在这个范围内，认为是“微小扰动”或“元器件直线”，绝对不减速！
    if (abs_Error < 10)
    {
        speed_base = BASE_SPEED;
    }
    else
    {
        // 超过死区后，才开始减速。减去死区值，保证减速曲线平滑过渡
        int active_Error = abs_Error - 10;

        // 线性：
        // speed_base = BASE_SPEED - (DECEL_FACTOR * active_Error);

        speed_base = BASE_SPEED - (DECEL_FACTOR * active_Error * active_Error);
    }

    // 兜底防死火
    if (speed_base < 240)
    {
        speed_base = 240;
    }

    // 6. 差速混合：算出左右轮此时此刻的“期望速度”
    motorL_Set = speed_base + speed_out;
    motorR_Set = speed_base - speed_out;
    if (motorL_Set > SPEED_MAX)
        motorL_Set = SPEED_MAX;
    if (motorL_Set < -SPEED_MAX)
        motorL_Set = -SPEED_MAX;
    if (motorR_Set > SPEED_MAX)
        motorR_Set = SPEED_MAX;
    if (motorR_Set < -SPEED_MAX)
        motorR_Set = -SPEED_MAX;
}

// ==========================================================
// 函数 2：底层肌肉执行器 (必须放在高频定时器里，建议 5ms)
// ==========================================================
void Motor_Speed_PID_Loop (void)
{

    // 1. 【核心正确】读编码器+立刻清零（增量式速度计算）
    int encL_raw = encoder_get_count(TIM4_ENCODER);
    encoder_clear_count(TIM4_ENCODER);
    int encR_raw = encoder_get_count(TIM6_ENCODER);
    encoder_clear_count(TIM6_ENCODER);
    int speed_l_raw = encL_raw;
    int speed_r_raw = -encR_raw;

    float delta_tick = ((float) speed_l_raw + (float) speed_r_raw) * 0.5f;

    if (delta_tick > 0)
    {
        Length += delta_tick * LENGTH_PER_TICK;
    }

    Actual_Speed_L_Filter += (speed_l_raw - Actual_Speed_L_Filter) / 3;
    Actual_Speed_R_Filter += (speed_r_raw - Actual_Speed_R_Filter) / 3;

    Actual_Speed_L = Actual_Speed_L_Filter;
    Actual_Speed_R = Actual_Speed_R_Filter;

    if ((motorL_Set != 0 || motorR_Set != 0) && startup_ramp_tick < STARTUP_RAMP_TICKS)
    {
        startup_ramp_tick++;
    }

    // 左右轮目标同步缓升，保留原有差速比例
    target_speed_l = motorL_Set * startup_ramp_tick / STARTUP_RAMP_TICKS;
    target_speed_r = motorR_Set * startup_ramp_tick / STARTUP_RAMP_TICKS;

    // 2. 计算速度偏差
    Error_Speed_L = target_speed_l - (int) Actual_Speed_L;
    Error_Speed_R = target_speed_r - (int) Actual_Speed_R;
    if (abs(Error_Speed_L) <= 5)
        Error_Speed_L = 0;
    if (abs(Error_Speed_R) <= 5)
        Error_Speed_R = 0;

    // 3. 【核心】增量式PI计算（只算PWM变化量，防积分饱和）
    MotorL_PI_Out += (KpL_speed * (Error_Speed_L - Last_Error_Speed_L) + KiL_speed * Error_Speed_L);
    MotorR_PI_Out += (KpR_speed * (Error_Speed_R - Last_Error_Speed_R) + KiR_speed * Error_Speed_R);
    Last_Error_Speed_L = Error_Speed_L;
    Last_Error_Speed_R = Error_Speed_R;

    int FF_L = (int) (K_ff_L * target_speed_l);
    int FF_R = (int) (K_ff_R * target_speed_r);

    // 先限制内部 PI 累加量
    if (MotorL_PI_Out > PI_MAX)
        MotorL_PI_Out = PI_MAX;
    if (MotorL_PI_Out < -PI_MAX)
        MotorL_PI_Out = -PI_MAX;
    if (MotorR_PI_Out > PI_MAX)
        MotorR_PI_Out = PI_MAX;
    if (MotorR_PI_Out < -PI_MAX)
        MotorR_PI_Out = -PI_MAX;

    MotorL_PWM = FF_L + MotorL_PI_Out;
    MotorR_PWM = FF_R + MotorR_PI_Out;

    // 4. 硬件限幅（匹配pid.h的SPEED_MAX，简化类型转换）
    if (MotorL_PWM > PWM_MAX)
        MotorL_PWM = PWM_MAX;
    if (MotorL_PWM < -PWM_MAX)
        MotorL_PWM = -PWM_MAX;
    if (MotorR_PWM > PWM_MAX)
        MotorR_PWM = PWM_MAX;
    if (MotorR_PWM < -PWM_MAX)
        MotorR_PWM = -PWM_MAX;

    // 5. 驱动电机（逐飞库默认顺序：右轮PWM，左轮PWM！取消注释即可使用）
    motor_control(MotorR_PWM, MotorL_PWM);
}

void Motor_Speed_Reset (void)
{
    startup_ramp_tick = 0;

    Actual_Speed_L_Filter = 0;
    Actual_Speed_R_Filter = 0;

    Actual_Speed_L = 0;
    Actual_Speed_R = 0;

    Error_Speed_L = 0;
    Error_Speed_R = 0;
    Last_Error_Speed_L = 0;
    Last_Error_Speed_R = 0;

    MotorL_PI_Out = 0;
    MotorR_PI_Out = 0;

    MotorL_PWM = 0;
    MotorR_PWM = 0;

    encoder_clear_count(TIM4_ENCODER);
    encoder_clear_count(TIM6_ENCODER);
}
