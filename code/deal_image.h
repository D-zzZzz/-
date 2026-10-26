/*
 * deal_image.h
 *
 *  Created on: 2026锟斤拷3锟斤拷19锟斤拷
 *
 */

#ifndef CODE_DEAL_IMAGE_H_
#define CODE_DEAL_IMAGE_H_

#include "zf_common_headfile.h"
#include "tu.h"
/*
预锟斤拷前瞻10-30cm锟斤拷锟斤拷锟揭革拷10cm锟斤拷锟斤拷野
*/
extern uint16 white_sum;


#define XM 188      //锟斤拷锟斤拷图锟斤拷 锟斤拷锟斤拷X
#define YM 120        //锟斤拷锟斤拷图锟斤拷 锟斤拷锟斤拷Y
#define XX (188 -1)             //锟斤拷锟斤拷图锟斤拷 锟铰憋拷X
#define YY (120 -1)             //锟斤拷锟斤拷图锟斤拷 锟铰憋拷Y

//锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟叫黑匡拷
#define Deal_Left   12             //锟斤拷锟斤拷锟斤拷预锟斤拷锟斤拷 锟斤拷锟斤拷锟斤拷卤锟�
#define Deal_Right  (XX-12)          //锟斤拷锟斤拷锟斤拷预锟斤拷锟斤拷 锟斤拷锟揭诧拷锟铰憋拷
#define Deal_Bottom 5             //锟斤拷锟斤拷锟斤拷预锟斤拷锟斤拷 锟斤拷锥锟斤拷卤锟�
#define Deal_Top    (YY-12)          //锟斤拷锟斤拷锟斤拷预锟斤拷锟斤拷 锟筋顶锟斤拷锟铰憋拷
//锟斤拷录锟斤拷失Lost_line锟侥边斤拷位锟斤拷
#define Lost_Left   (Deal_Left - 2)
#define Lost_Right  (Deal_Right +2)
#define Lost_Bottom (Deal_Bottom -2)
#define Lost_Top    (Deal_Top + 2)


#define IMG_W  MT9V03X_W      //原始图锟斤拷 锟斤拷锟斤拷锟斤拷
#define IMG_H  MT9V03X_H      //原始图锟斤拷 锟斤拷锟斤拷锟斤拷
#define _IMG_W (IMG_W-1)    //原始图锟斤拷 锟铰憋拷锟斤拷
#define _IMG_H (IMG_H-1)    //原始图锟斤拷 锟铰憋拷锟斤拷



#define COL IMG_W    //原始图锟斤拷 锟斤拷锟斤拷锟斤拷
#define ROW IMG_H    //原始图锟斤拷 锟斤拷锟斤拷锟斤拷
#define _COL_IMG_W //原始图锟斤拷 锟铰憋拷锟斤拷
#define _ROW _IMG_H //原始图锟斤拷 锟铰憋拷锟斤拷

#define INVALID_POS 0xFF  // 锟斤拷锟斤拷一锟斤拷锟斤拷效值锟斤拷锟�
extern uint8_t mid_line[YM];

#define IMG_SIZE (IMG_W * IMG_H)    //图锟斤拷叽锟�
#define MAP_SIZE (YM * XM)  // 锟斤拷图锟竭达拷

//图锟斤拷锟斤拷锟斤拷锟斤拷应锟斤拷锟�
#define Black 0  //锟斤拷色
#define White 255  //锟斤拷色
#define Control_line 1 //锟斤拷色
#define Left_line 2  //锟斤拷色
#define Right_line 3  //锟斤拷色
#define Lost_line 4  //锟斤拷色
#define Judge_line 5  //锟斤拷色
#define Make_up 6    //锟斤拷色
#define LandR 7      //锟斤拷色
#define USE_num YM*4   //锟斤拷锟斤拷锟揭碉拷锟斤拷锟斤拷锟斤拷员锟斤拷锟斤拷



enum ImgFlag
{
    straightlineS,
    straightlineL,
    curve,

};

typedef enum Elements_Mode
{
    NO,
    YES,
}Elements_Mode;


typedef enum Elements_Lock
{
    Empty_,
    Annulus_,
    Disappear_,
    Stopline_,
}Elements_Lock;


typedef enum Annulus_Mode
{
    Differential,
    Tracing,
}Annulus_Mode;

extern volatile Elements_Mode elements_Mode;
extern volatile Elements_Lock elements_Lock[10];
extern volatile Annulus_Mode Annulus_mode;

extern int elements_index;

extern enum ImgFlag IF;
extern enum ImgFlag IF_L;

extern uint8_t search_break_reason;


extern float turn_yaw_deg;
extern uint8 stop_num;
extern volatile uint8 stopline_flag;
extern uint8 Annulus_flag;
extern int Annulus_Curvature[5];
extern int Annulus_num;
extern int Curvature;

extern uint8_t nowThreshold;
extern int minGrayscale;
extern int maxGrayscale;
extern uint8_t minThreshold;
extern int minGray;
extern int maxGray;
extern uint16 EXP_TIME;
extern int UD_OFFSET_DEF;
extern uint8_t maxThreshold;
extern uint8 img_threshold_group[3];//锟斤拷锟斤拷锟斤拷值锟斤拷锟斤拷   锟斤拷锟斤拷锟斤拷
extern uint8 close_Threshold;
extern uint8 far_Threshold;
extern uint8 mid_Threshold;

extern uint8_t Make_up_num[YM];





extern uint8 mt9v03x_image[IMG_H][IMG_W];
extern uint8 imgOSTU[YM][XM];
extern uint8 imgTran[IMG_H][IMG_W];
extern uint8 white_num_row[YM];
extern uint8 white_num_col[XM];
extern uint8 mid_line[YM];
extern uint8 start_center;
extern uint8 start_center_x;
extern uint8 start_center_y;
extern uint8 end_center_x;
extern uint8 end_center_y;
extern uint8 start_point_l;
extern uint8 start_point_r;
extern int points_l[(uint16)USE_num][2];//锟斤拷锟斤拷
extern int points_r[(uint16)USE_num][2];//锟斤拷锟斤拷
extern float dir_r[(uint16)USE_num];//锟斤拷锟斤拷锟芥储锟揭憋拷锟斤拷锟斤拷锟斤拷锟斤拷
extern float dir_l[(uint16)USE_num];//锟斤拷锟斤拷锟芥储锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟�
extern uint32_t r_data_statics;//统锟斤拷锟揭憋拷
extern uint32_t l_data_statics;//统锟斤拷锟揭憋拷
extern uint8 hightest;//锟斤拷叩锟�
extern uint16 leftmost[2];
extern uint16 rightmost[2];
extern uint16 topmost[2];
extern uint8 dealimg_finish_flag;
extern uint8 disappear_flag;
extern int deal_runing;
extern int dealimg_time;
extern int disappear_num;
extern int Disappear_angle_L[3];
extern int Disappear_angle_R[3];
extern float Start_angle;
extern int disappear_total;
extern float Disappear_Length;
extern float Disappear_Zero;
extern float Disappear_Yaw;
extern int Disappear_Dir_L[3][3];
extern int Disappear_Dir_R[3][3];
typedef enum RUN_Dir
{
    Left,
    Right,
} RUN_Dir;
extern volatile RUN_Dir run_dir;

extern uint8 l_lost_tip;
extern uint8 r_lost_tip;
extern uint8 t_lost_tip;
extern uint8 b_lost_tip;
extern uint8 b_lost_num;
extern uint8 t_lost_num;
extern uint8 l_lost_num;
extern uint8 r_lost_num;
extern uint8 Stop_line;
extern uint8 dis_Solidline;

extern uint16 b_center[3];
extern uint16 l_center[3];
extern uint16 r_center[3];
extern uint16 t_center[3];
extern float Out_Annulus_Length;
extern float Stopline_Position;
extern volatile float Stop_Length;
extern const float white_width[YM];
//extern const float k1[YM];
//extern const float k2[YM];
extern float start_time;
extern float end_time;
extern float deal_time;


extern float Length;
extern  float Last_Valid_Junction_Length;

extern uint8_t top_straight_error_fuse;

extern volatile uint8_t debug_road_state;
extern volatile uint8_t debug_turn_cmd;
extern uint16 EXP_TIME;
void standard(void);
uint8 getOSTUThreshold(void);
void Get_imgOSTU(void);
uint8 get_start_point(void);
uint8 search_l_r(uint8 start_l_x,uint8 start_l_y,uint8 start_r_x,uint8 start_r_y );
void Get_lost_tip(uint8 length);
void Get_start_center(void);
uint8 Get_top_straightline(void);
uint8 Left_curve_line(void);
uint8 Right_curve_line(void);
uint8 Get_Annulus();
uint8 Go_Annulus();
uint8 Deal_crossroads();
uint8 Get_stopline(void);


//uint8 Dashedline_Makeup();
uint8 Disappear_detection();
uint8 Deal_disappear();
void Get_curve_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int8_t n);
void Draw_Heikuang(void);
uint8_t Headle_disconnected_lu(void);
uint8_t Deal_Turn_State ( uint8_t *stop_line);
uint8_t Deal_Normal_Curve_State(uint8_t *stop_line);
uint8_t Continue_Normal_Curve_No_Start(uint8_t *stop_line);
uint8_t Has_Forward_Road_After_T (void);
uint8_t Handle_dianzu(void);
uint8_t Has_Disconnected_Forward_Road(void);
uint8_t Find_Top_Forward_Point(void);
uint8_t Draw_Top_Forward_Line(void);
uint8_t lvbo_diode_xia(void);
uint8_t lvbo_diode_shang(void);
uint8_t lvbo_diode_row_white(void);
void Turn_Yaw_Task(void);
uint8_t Draw_To_Top_Center_Line(void);
static uint16 Count_White_On_Line(int x0, int y0, int x1, int y1);
uint8_t top_white_too_much(void);
uint8_t Is_Seventh_Straight_Lock(void);
uint8_t Get_Max_White_Column_X(void);
uint8_t Is_Last_Junction_Recognized(void);


#endif /* CODE_DEAL_IMAGE_H_ */
