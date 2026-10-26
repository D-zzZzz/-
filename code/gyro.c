#include "zf_common_headfile.h"


int Gyro_Z_Offset = 0; // Z����ƫֵ

void Gyro_Offset_Init(void)
{
    long sum = 0;
    // ѭ���ɼ� 100 �Σ���ƽ��ֵ��Ϊ��ƫ
    for(int i = 0; i < 500; i++)
    {
        imu660rc_get_gyro(); // ������ɿ�ײ�Ķ�ȡ����
        sum += imu660rc_gyro_z;
        system_delay_ms(2);  // ��ʱ��һ��оƬ
    }
    Gyro_Z_Offset = sum / 500;
}
int Get_Clean_Gyro_Z(void)
{
    imu660rc_get_gyro(); // ˢ����ɿ����ȫ�ֱ���

    // ��ȥ��ƫ���õ����Դ����Ľ��ٶȣ�
    // (ע�������ţ�����תʱ���ֵ����������������Ǹ���������ǰ��Ӹ����ŷ�תһ��)
    return (imu660rc_gyro_z - Gyro_Z_Offset);
}
