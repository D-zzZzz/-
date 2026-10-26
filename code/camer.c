#include "zf_common_headfile.h"

#define LCD_IMG_W   188
#define LCD_IMG_H   120

#define DEBUG_CAMERA_SLOT_NUM      1
#define DEBUG_CAMERA_SLOT_INVALID  0xFF

typedef struct
{
    uint8 image[MT9V03X_H][MT9V03X_W];
    seekfree_assistant_camera_boundary_uint8_struct left[USE_num];
    seekfree_assistant_camera_boundary_uint8_struct right[USE_num];
    seekfree_assistant_camera_boundary_uint8_struct mid[YM];
    uint16 left_count;
    uint16 right_count;
    uint16 mid_count;
    uint16 frame_id;
} debug_camera_frame_t;

static debug_camera_frame_t debug_camera_frames[DEBUG_CAMERA_SLOT_NUM];
static seekfree_assistant_camera_struct debug_camera_obj;
static seekfree_assistant_camera_boundary_struct debug_left_info;
static seekfree_assistant_camera_boundary_struct debug_right_info;
static seekfree_assistant_camera_boundary_struct debug_mid_info;

static volatile uint8 debug_ready = 0;
static volatile uint8 debug_ready_slot = 0;
static volatile uint8 debug_sending_slot = DEBUG_CAMERA_SLOT_INVALID;
static uint8 debug_write_slot = 0;

static inline int lcd_x(int x)
{
    return ((XM - 1) - x) * (LCD_IMG_W - 1) / (XM - 1);
}

static inline int lcd_y(int y)
{
    return y * (LCD_IMG_H - 1) / (YM - 1);
}

static void draw_cyan_block(int x, int y)
{
    int cx = lcd_x(x);
    int cy = lcd_y(y);

    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            int px = cx + dx;
            int py = cy + dy;
            if (px >= 0 && px < LCD_IMG_W && py >= 0 && py < LCD_IMG_H)
            {
                ips200_draw_point(px, py, RGB565_CYAN);
            }
        }
    }
}

static uint8 debug_camera_find_write_slot(void)
{
    for (uint8 i = 0; i < DEBUG_CAMERA_SLOT_NUM; i++)
    {
        uint8 slot = (uint8)((debug_write_slot + i) % DEBUG_CAMERA_SLOT_NUM);

        if (debug_sending_slot == slot)
        {
            continue;
        }

        if (debug_ready && debug_ready_slot == slot)
        {
            continue;
        }

        debug_write_slot = (uint8)((slot + 1) % DEBUG_CAMERA_SLOT_NUM);
        return slot;
    }

    return DEBUG_CAMERA_SLOT_INVALID;
}

static void debug_draw_frame_id(debug_camera_frame_t *frame, uint16 frame_id)
{
    static const uint8 digit_font[10][5] =
    {
        {7, 5, 5, 5, 7}, {2, 6, 2, 2, 7},
        {7, 1, 7, 4, 7}, {7, 1, 7, 1, 7},
        {5, 5, 7, 1, 1}, {7, 4, 7, 1, 7},
        {7, 4, 7, 5, 7}, {7, 1, 1, 1, 1},
        {7, 5, 7, 5, 7}, {7, 5, 7, 1, 7}
    };
    uint8 digits[3];

    frame_id %= 1000;
    digits[0] = frame_id / 100;
    digits[1] = frame_id / 10 % 10;
    digits[2] = frame_id % 10;

    for (int y = 1; y < 13; y++)
    {
        for (int x = 1; x < 25; x++)
        {
            frame->image[y][x] = 0;
        }
    }

    for (int n = 0; n < 3; n++)
    {
        for (int row = 0; row < 5; row++)
        {
            for (int col = 0; col < 3; col++)
            {
                if (digit_font[digits[n]][row] & (1 << (2 - col)))
                {
                    int px = 2 + n * 8 + col * 2;
                    int py = 2 + row * 2;
                    frame->image[py][px] = 255;
                    frame->image[py][px + 1] = 255;
                    frame->image[py + 1][px] = 255;
                    frame->image[py + 1][px + 1] = 255;
                }
            }
        }
    }
}

void debug_camera_init(void)
{
    debug_ready = 0;
    debug_ready_slot = 0;
    debug_sending_slot = DEBUG_CAMERA_SLOT_INVALID;
    debug_write_slot = 0;

    seekfree_assistant_camera_config(
            &debug_camera_obj,
            SEEKFREE_ASSISTANT_CAMERA_TYPE_MT9V03X,
            MT9V03X_W,
            MT9V03X_H,
            debug_camera_frames[0].image[0]);
}

uint8 debug_camera_prepare(uint16 frame_id)
{
    uint8 slot = debug_camera_find_write_slot();
    if (slot == DEBUG_CAMERA_SLOT_INVALID)
    {
        return 0;
    }

    debug_camera_frame_t *frame = &debug_camera_frames[slot];
    uint16 left_count = 0;
    uint16 right_count = 0;
    uint16 mid_count = 0;

    for (uint16 y = 0; y < YM; y++)
    {
        for (uint16 x = 0; x < XM; x++)
        {
            if (imgOSTU[y][x] == White ||
                imgOSTU[y][x] == Left_line ||
                imgOSTU[y][x] == Right_line ||
                imgOSTU[y][x] == Control_line)
            {
                frame->image[YM - 1 - y][x] = 255;
            }
            else
            {
                frame->image[YM - 1 - y][x] = 0;
            }
        }
    }
    /*memcpy(&frame->image[0][0],
             &mt9v03x_image[0][0],
             sizeof(frame->image));*/





    debug_draw_frame_id(frame, frame_id);

    uint32 count = l_data_statics < USE_num ? l_data_statics : USE_num;
    for (uint32 i = 0; i < count; i++)
    {
        if (points_l[i][0] >= 0 && points_l[i][0] < XM &&
            points_l[i][1] >= 0 && points_l[i][1] < YM)
        {
            frame->left[left_count].x = (uint8)points_l[i][0];
            frame->left[left_count].y = (uint8)(YM - 1 - points_l[i][1]);
            left_count++;
        }
    }

    count = r_data_statics < USE_num ? r_data_statics : USE_num;
    for (uint32 i = 0; i < count; i++)
    {
        if (points_r[i][0] >= 0 && points_r[i][0] < XM &&
            points_r[i][1] >= 0 && points_r[i][1] < YM)
        {
            frame->right[right_count].x = (uint8)points_r[i][0];
            frame->right[right_count].y = (uint8)(YM - 1 - points_r[i][1]);
            right_count++;
        }
    }

    for (uint16 y = 0; y < YM; y++)
    {
        if (mid_line[y] != INVALID_POS && mid_line[y] < XM)
        {
            frame->mid[mid_count].x = (uint8)mid_line[y];
            frame->mid[mid_count].y = (uint8)(YM - 1 - y);
            mid_count++;
        }
    }

    frame->left_count = left_count;
    frame->right_count = right_count;
    frame->mid_count = mid_count;
    frame->frame_id = frame_id;

    debug_ready_slot = slot;
    debug_ready = 1;
    return 1;
}

uint8 debug_camera_has_frame(void)
{
    return debug_ready;
}

void debug_camera_drop(void)
{
    debug_ready = 0;
}

uint8 debug_camera_send(void)
{
    if (!debug_ready)
    {
        return 0;
    }

    uint8 slot = debug_ready_slot;
    debug_camera_frame_t *frame = &debug_camera_frames[slot];

    debug_sending_slot = slot;
    if (debug_ready_slot == slot)
    {
        debug_ready = 0;
    }

    if (frame->left_count)
    {
        seekfree_assistant_camera_boundary_config(
                &debug_left_info,
                SEEKFREE_ASSISTANT_DATA_TYPE_UINT8,
                RGB565_RED,
                frame->left_count,
                frame->left);
        seekfree_assistant_camera_boundary_send(&debug_left_info);
    }

    if (frame->right_count)
    {
        seekfree_assistant_camera_boundary_config(
                &debug_right_info,
                SEEKFREE_ASSISTANT_DATA_TYPE_UINT8,
                RGB565_BLUE,
                frame->right_count,
                frame->right);
        seekfree_assistant_camera_boundary_send(&debug_right_info);
    }

    if (frame->mid_count)
    {
        seekfree_assistant_camera_boundary_config(
                &debug_mid_info,
                SEEKFREE_ASSISTANT_DATA_TYPE_UINT8,
                RGB565_PURPLE,
                frame->mid_count,
                frame->mid);
        seekfree_assistant_camera_boundary_send(&debug_mid_info);
    }

    seekfree_assistant_camera_config(
            &debug_camera_obj,
            SEEKFREE_ASSISTANT_CAMERA_TYPE_MT9V03X,
            MT9V03X_W,
            MT9V03X_H,
            frame->image[0]);
    seekfree_assistant_camera_send(&debug_camera_obj);

    debug_sending_slot = DEBUG_CAMERA_SLOT_INVALID;
    return 1;
}

void draw_all_line(void)
{
    for (int i = 1; i < l_data_statics; i++)
    {
        int x1 = lcd_x(points_l[i - 1][0]);
        int y1 = lcd_y(points_l[i - 1][1]);
        int x2 = lcd_x(points_l[i][0]);
        int y2 = lcd_y(points_l[i][1]);

        if (x1 >= 0 && x1 < LCD_IMG_W && y1 >= 0 && y1 < LCD_IMG_H &&
            x2 >= 0 && x2 < LCD_IMG_W && y2 >= 0 && y2 < LCD_IMG_H)
        {
            ips200_draw_line(x1, y1, x2, y2, RGB565_RED);
        }
    }

    for (int i = 1; i < r_data_statics; i++)
    {
        int x1 = lcd_x(points_r[i - 1][0]);
        int y1 = lcd_y(points_r[i - 1][1]);
        int x2 = lcd_x(points_r[i][0]);
        int y2 = lcd_y(points_r[i][1]);

        if (x1 >= 0 && x1 < LCD_IMG_W && y1 >= 0 && y1 < LCD_IMG_H &&
            x2 >= 0 && x2 < LCD_IMG_W && y2 >= 0 && y2 < LCD_IMG_H)
        {
            ips200_draw_line(x1, y1, x2, y2, RGB565_BLUE);
        }
    }

    for (int y = 0; y < YY - 1; y++)
    {
        if (mid_line[y] != INVALID_POS && mid_line[y + 1] != INVALID_POS)
        {
            int x1 = lcd_x(mid_line[y]);
            int y1 = lcd_y(y);
            int x2 = lcd_x(mid_line[y + 1]);
            int y2 = lcd_y(y + 1);

            if (x1 >= 0 && x1 < LCD_IMG_W && y1 >= 0 && y1 < LCD_IMG_H &&
                x2 >= 0 && x2 < LCD_IMG_W && y2 >= 0 && y2 < LCD_IMG_H)
            {
                ips200_draw_line(x1, y1, x2, y2, RGB565_PURPLE);
            }
        }
    }

    for (int y = Deal_Bottom; y <= Lost_Top; y++)
    {
        if (imgOSTU[y][Lost_Left] == Lost_line)
        {
            draw_cyan_block(Lost_Left, y);
        }

        if (imgOSTU[y][Lost_Right] == Lost_line)
        {
            draw_cyan_block(Lost_Right, y);
        }
    }

    for (int x = Lost_Left; x <= Lost_Right; x++)
    {
        if (imgOSTU[Lost_Top][x] == Lost_line)
        {
            draw_cyan_block(x, Lost_Top);
        }
    }
}
