#ifndef __MOTOR_CONTROL_H
#define __MOTOR_CONTROL_H
#include "stdint.h"
#include "usart.h"
#include <stdlib.h> 
#include "tim.h"

typedef  struct
{
    uint16_t Pulse;
    uint16_t Actual_angle;
    uint16_t Target_angle;
    uint16_t Start_Pulse;
    uint16_t direction;
}Motor_open_t;
extern Motor_open_t Garbage_inlet_motor;

typedef  struct
{
    uint8_t motor_target;
    uint8_t motor_act;
    uint8_t motor_static;
}Down_Motor_t;
extern Down_Motor_t Down_Motor;

typedef  struct
{
    uint8_t kitchen_number;
    uint8_t recovery_number;
    uint8_t harmful_number;
    uint8_t other_number;
}waste_number_t;
extern waste_number_t waste_number;

void ZDT_X57_Traj_Position_Control(uint8_t addr, uint8_t dir, uint16_t acc, uint16_t dec, float velocity, float position, uint8_t raf, uint8_t multi);
void ZDT_X57_Reading_motor_position_error(uint8_t addr);
void UP_motor_move(void);
void rotateToPosition(int currentPos, int targetPos);

void push_rod_motor_in(void);
void push_rod_motor_push(void);
void push_rod_motor_stop(void);
void compress_motor(void);
#endif

