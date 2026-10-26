/*
 * encoder.c
 *
 *  Created on: 2026-04-11
 *      Author: dada
 */

#include "zf_common_headfile.h"

int16 encoder_data_quaddec1 = 0;
int16 encoder_data_quaddec2 = 0;

void encoder_init(void)
{
    encoder_dir_init(TIM4_ENCODER, TIM4_ENCODER_CH1_P02_8, TIM4_ENCODER_CH2_P00_9);   // left: A=P02.8 DIR=P00.9
    encoder_dir_init(TIM6_ENCODER, TIM6_ENCODER_CH1_P20_3, TIM6_ENCODER_CH2_P20_0);   // right: A=P20.3 DIR=P20.0
}
