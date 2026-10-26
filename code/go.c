#include "zf_common_headfile.h"
/******************************************************************************
 * 函数名称     : go
 * 描述        : 小车go！！�?
 * 进入参数     : voidֵ
 * 返回参数     : void
 ******************************************************************************/
int A = 99;
uint8_t start_point_ok = 0;
void go (void)
{
    start_point_ok = 0;
    nowThreshold = getOSTUThreshold();

    Get_imgOSTU();
    Draw_Heikuang();

    Stop_line = 0; //动态前瞻初始化
    top_straight_error_fuse = 0;
    A = 0;

    IF_L = IF;
    IF = straightlineS;
    memset(mid_line, INVALID_POS, sizeof(mid_line));
    if (disappear_flag == 0 && get_start_point()) //如果找到起点
    {
        start_point_ok = 1;
        search_l_r(start_point_l, start_center_y, start_point_r, start_center_y); //八邻域搜索边�?
        Get_lost_tip(2); //获取边缘丢失特征

        if (Stop_line == 0)  //常规巡线及所有节点处�?
        {

            Get_start_center(); //获取中线起点

            //   uint8_t near_left_lost = (l_lost_num > 0) || (leftmost[0] <= Deal_Left + 30);
            // uint8_t near_right_lost = (r_lost_num > 0) || (rightmost[0] >= Deal_Right - 30);
            // ================= 疯狂电路：核心节点识�?=================
            // 特征1：标准的 T 或者Y�?
            // 特征2：三极管_型分叉（顶部有，左右有一个出路）
            if (Deal_Turn_State(&Stop_line))
            {

                if (Stop_line == 0)
                {
                    Stop_line = Get_top_straightline();
                    IF = IF_L;

                }
            }

            /* else if (b_lost_num > 0 && l_lost_num &&
             !t_lost_num && !r_lost_num)
             {

             Stop_line = Has_Disconnected_Forward_Road();
             A=3;

             // 没发现前方断路，或者搭线失败，才按普通左弯处理
             if (Stop_line == 0)
             {
             Stop_line = Left_curve_line();
             A=4;
             }
             }
             else if (b_lost_num > 0 && r_lost_num &&
             !t_lost_num && !l_lost_num)
             {
             Stop_line = Has_Disconnected_Forward_Road();
             A=6;

             // 对应你图片里的情况：有前方道路就不会直接右拐
             if (Stop_line == 0)
             {
             Stop_line = Right_curve_line();
             A=5;
             }
             }*/
            else if (Deal_Normal_Curve_State(&Stop_line))
            {

            }
            else if (b_lost_num > 0 && t_lost_num)
            {
                A = 7;

                Stop_line = Handle_dianzu();

                if (Stop_line == 0)
                {
                    Stop_line = Get_top_straightline();
                    A = 8;
                }

            }
            // ==========================================================

            // 遇到开关和电容等短断路，直接搭�?
            else if (t_lost_num == 0 && hightest > Deal_Bottom + 30 && hightest < Deal_Top - 10
                    && b_center[0] > Deal_Left + 30 && b_center[0] < Deal_Right - 30)
            {
                A = 9;
                Stop_line = Has_Disconnected_Forward_Road();

                if (Stop_line == 0)
                {
                    A = 10;
                    IF = IF_L;
                }
            }
            else
            {
                A = 98;

                Stop_line = 0;
                IF = IF_L;
            }
        }
        if (Stop_line == 0)
        {
            for (int y = Deal_Top; y >= start_center_y; y--)
            {
                if (mid_line[y] != INVALID_POS && mid_line[y] < XM)
                {
                    Stop_line = y;
                    break;
                }
            }
        }


    }
    else
    {

        A = 100;

        if (Continue_Normal_Curve_No_Start(&Stop_line))
        {
            start_point_ok = 1;
        }

    }
    
    // disappear_flag = Deal_disappear();
    if (elements_index >= 9 || elements_Lock[elements_index] == Empty_)
    {
        elements_index = 0;
    }

}
