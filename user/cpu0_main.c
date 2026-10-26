#include "zf_common_headfile.h"
#pragma section all "cpu0_dsram"

#define LCD_W   320
#define LCD_H   204   // 188:120 等比例放大到接近满宽，剩下空间显示参 ?
#define IMAGE_TX_ENABLE       1
#define IMAGE_TX_FRAME_DIV    1
#define PARAM_TX_ENABLE       1
#define IMAGE_FAKE_LOAD_ENABLE 0
#define IMAGE_FAKE_LOAD_US    22000U  // 50 FPS = 20 ms/frame, reserve about 4 ms
/*IMAGE_FAKE_LOAD_US    理论帧率
10000U  100 FPS
20000U  50 FPS
22000U  45.5 FPS
23000U  43.5 FPS
25000U  40 FPS
30000U  33.3 FPS*/

char wireless_buf[64];
static int zhen = 0;
#define OSCILLOSCOPE_CHANNEL_NUM 11

seekfree_assistant_oscilloscope_struct oscilloscope_config;
float oscilloscope_buffer[OSCILLOSCOPE_CHANNEL_NUM];
extern volatile uint8 oscilloscope_send_flag;
extern seekfree_assistant_transfer_callback_function seekfree_assistant_transfer_callback;

/*
 * 无图传模式下仍走完整的图像/边线打包路径，并读取相同的数据。
 * 最后一次图像数据回调中补足到 IMAGE_FAKE_LOAD_US，使占用时间接近
 * 30 MHz WiFi-SPI 发送一帧 188x120 图像时的耗时。
 */
static volatile uint32 image_fake_checksum = 0;
static uint32 image_fake_start_us = 0;
volatile uint32 image_tx_load_us = 0;

static uint32 image_fake_transfer (const uint8 *buffer, uint32 length)
{
    uint32 checksum = 0;

    for (uint32 i = 0; i < length; i++)
    {
        checksum += buffer[i];
    }
    image_fake_checksum ^= checksum;

    /* 图像数据是本帧最后一个发送块，在这里等待可保持发送槽一直占用。 */
    if (length == (uint32) MT9V03X_W * MT9V03X_H)
    {
        uint32 elapsed_us = system_getval_us() - image_fake_start_us;
        if (elapsed_us < IMAGE_FAKE_LOAD_US)
        {
            system_delay_us(IMAGE_FAKE_LOAD_US - elapsed_us);
        }
    }

    return 0;
}

static void debug_camera_fake_load (void)
{
    seekfree_assistant_transfer_callback_function saved_callback = seekfree_assistant_transfer_callback;

    image_fake_start_us = system_getval_us();
    seekfree_assistant_transfer_callback = image_fake_transfer;
    debug_camera_send();
    seekfree_assistant_transfer_callback = saved_callback;
    image_tx_load_us = system_getval_us() - image_fake_start_us;
}

static void debug_camera_real_send_timed (void)
{
    uint32 start_us = system_getval_us();
    debug_camera_send();
    image_tx_load_us = system_getval_us() - start_us;
}

static uint16 debug_frame_id = 0;
volatile uint8 image_frame_ready = 0;
volatile uint8 image_cpu1_busy = 0;
volatile uint8 image_cpu0_using = 0;
volatile uint16 image_processed_frame_id = 0;

int core0_main (void)
{
    clock_init();
    debug_init();
    // 此处编写用户代码 例如外设初始化代码等
    ips200_set_dir(IPS200_CROSSWISE_180); // 需要先横屏 不然显示不下
    ips200_init(IPS200_TYPE_SPI);
    imu660rc_init(IMU660RC_QUARTERNION_DISABLE);
    while(1)
    {
        if(mt9v03x_init())
            gpio_toggle_level(P20_8);
        else
            break;
        system_delay_ms(500);
    }
  negative_pressure_init();
   negative_pressure_set_us(1850);//1500
   system_delay_ms(3000);
   Gyro_Offset_Init();
    motor_init();
    encoder_init();
#if IMAGE_TX_ENABLE || PARAM_TX_ENABLE
#define WIFI_SSID_TEST      "abca"
#define WIFI_PASSWORD_TEST  "1234567891"

    while (wifi_spi_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST))
    {
        ips200_show_string(0, 40, "wifi fail");
        system_delay_ms(100);
    }

    ips200_show_string(0, 40, "wifi ok");

#if (WIFI_SPI_AUTO_CONNECT == 0)
    while (wifi_spi_socket_connect("TCP",
    WIFI_SPI_TARGET_IP, "8086", "8087"))
    {
        system_delay_ms(100);
    }
#endif

    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);
    wireless_uart_init();
 //   seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIRELESS_UART);
    // seekfree_assistant_camera_config(
      // &image_tx_camera,
       //SEEKFREE_ASSISTANT_CAMERA_TYPE_GRAY,
       //XM,
       //YM,
       //image_tx_buffer);


#endif

#if IMAGE_TX_ENABLE || IMAGE_FAKE_LOAD_ENABLE
    debug_camera_init();
#endif

#if PARAM_TX_ENABLE
    seekfree_assistant_oscilloscope_config(
            &oscilloscope_config,
            OSCILLOSCOPE_CHANNEL_NUM,
            oscilloscope_buffer);
#endif

    pit_ms_init(CCU60_CH0, 5);
    Motor_Speed_Reset();


    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();         // 等待所有核心初始化完毕

    // ================== 4. 后台主循 ?(大状态机) ==================
    while (TRUE)
    {
        debug_frame_id = image_processed_frame_id;

#if PARAM_TX_ENABLE
        if (oscilloscope_send_flag)
        {
            oscilloscope_send_flag = 0;
            oscilloscope_buffer[0] = (float)debug_turn_cmd;
            oscilloscope_buffer[1] = (float)A;
            oscilloscope_buffer[2] = (float)speed_out;
         /* oscilloscope_buffer[3] = (float)turn_yaw_deg;
            oscilloscope_buffer[4] = (float)white_sum;
            oscilloscope_buffer[5] = (float)speed_out;
            oscilloscope_buffer[6] = (float)l_lost_tip;*/
                       oscilloscope_buffer[3] = (float)target_speed_l;
                        oscilloscope_buffer[4] = (float) target_speed_r;
                        oscilloscope_buffer[5] = (float)Actual_Speed_L;
                        oscilloscope_buffer[6] = (float)Actual_Speed_R;
            oscilloscope_buffer[7] = (float)Length;
            oscilloscope_buffer[8] = (float)turn_yaw_deg;
            oscilloscope_buffer[9] = (float)Error;
            oscilloscope_buffer[10] = (float)debug_frame_id;
            seekfree_assistant_oscilloscope_send(&oscilloscope_config);
        }
#endif

#if IMAGE_TX_ENABLE || IMAGE_FAKE_LOAD_ENABLE
        if (debug_camera_has_frame())
        {
            static uint8 image_tx_frame_count = 0;
            if (++image_tx_frame_count >= IMAGE_TX_FRAME_DIV)
            {
                image_tx_frame_count = 0;
#if IMAGE_TX_ENABLE
                debug_camera_real_send_timed();
#else
                debug_camera_fake_load();
#endif
            }
            else
            {
                debug_camera_drop();
            }
        }
#endif

        static uint16 last_lcd_frame = 0xFFFF;
        if (last_lcd_frame != debug_frame_id)
        {
            last_lcd_frame = debug_frame_id;
            ips200_show_int(0, 210, Error, 5);
            ips200_show_int(40, 210, A, 5);
            ips200_show_int(70, 210, debug_turn_cmd, 5);
        }

        image_cpu0_using = 0;
    }
}
#pragma section all restore


