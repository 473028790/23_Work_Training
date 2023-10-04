#include "vision.h"
extern uint8_t vision_usart_flag,Down_motor_flag;
extern uint8_t Vision_data[Vision_FRAME_LENGTH];
extern int compress_motor_start;
uint8_t Rotation_direction,rotation_distance;
void vision_uart_deal(void)
{
    if(vision_usart_flag==1)
    {
        if(Vision_data[0]==0x80 && Vision_data[2]==0x7E)
        {
            vision_garbage.last_Type=vision_garbage.Type;
            vision_garbage.Type=Vision_data[1];
            if(vision_garbage.Type==1) waste_number.kitchen_number++;
            if(vision_garbage.Type==2) waste_number.recovery_number++;
            if(vision_garbage.Type==3) waste_number.harmful_number++;
            if(vision_garbage.Type==4) waste_number.other_number++;
            
            vision_garbage.Serial++;
            if((vision_garbage.Serial)%10==1 && vision_garbage.Serial!=1)
            {
                vision_garbage.last_Type=1;
            }
        }
        if (vision_garbage.Type >= 1 && vision_garbage.Type <= 4) {
            Down_Motor.motor_target=vision_garbage.Type;
            Down_Motor.motor_act=vision_garbage.last_Type;
            Down_motor_flag=1;
        }

        //rotateToPosition(Down_Motor.motor_act,Down_Motor.motor_target);
        vision_usart_flag=0;
    }
}
static void vision_SendData(uint8_t data1)
{	
    uint8_t tail  =0x80;
    uint8_t End =0x7E;
    static uint8_t screen_data;

    screen_data=data1;


    HAL_UART_Transmit(&huart2,(uint8_t *)&tail,1,0x10);
    HAL_UART_Transmit(&huart2,(uint8_t *)&screen_data,sizeof(screen_data),0x10);
    HAL_UART_Transmit(&huart2,(uint8_t *)&End,1,0x10);
};
int cnt62=0;
void vision_send_1(void)
{
	cnt62++;

	uint8_t tail  =0x81;
	uint8_t End =0x7C;
	static uint8_t vision_stop=1;


	HAL_UART_Transmit(&huart2,(uint8_t *)&tail,1,0x10);
	HAL_UART_Transmit(&huart2,(uint8_t *)&vision_stop,sizeof(vision_stop),0x10);
	HAL_UART_Transmit(&huart2,(uint8_t *)&End,1,0x10);
	
}
int cnt=0;
void vision_send_2(void)
{
	cnt++;
	uint8_t tail  =0x81;
	uint8_t End =0x7C;
	static uint8_t vision_stop=2;


	HAL_UART_Transmit(&huart2,(uint8_t *)&tail,1,0x10);
	HAL_UART_Transmit(&huart2,(uint8_t *)&vision_stop,sizeof(vision_stop),0x10);
	HAL_UART_Transmit(&huart2,(uint8_t *)&End,1,0x10);
}