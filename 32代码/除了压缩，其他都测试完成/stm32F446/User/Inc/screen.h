#ifndef __SCREEN_H
#define __SCREEN_H

#include "usart.h"
#include "motor_control.h"

#define Show_garbage_categories 01
#define Display_garbage_quantity 02
#define Display_classification_status 03


void screen_return_movie(void);
void screen_display_task(void);
void screen_Identify_garbage(uint8_t garbage_Serial,uint8_t garbage_type,uint8_t garbage_state);
void screen_Number_garbage(uint8_t garbage_type,uint8_t garbage_number,uint8_t garbage_overload);
#endif

