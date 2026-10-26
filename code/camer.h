#include "zf_common_headfile.h"
#ifndef CORE_CAMER_H
#define CORE_CAMER_H

void draw_all_line(void);

void debug_camera_init(void);
uint8 debug_camera_prepare(uint16 frame_id);
uint8 debug_camera_has_frame(void);
uint8 debug_camera_send(void);
void debug_camera_drop(void);

#endif