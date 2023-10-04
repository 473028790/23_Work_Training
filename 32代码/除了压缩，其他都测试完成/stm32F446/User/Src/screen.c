#include "screen.h"
uint8_t screen_usart_flag;
extern uint8_t garbage_Serial;
extern uint8_t Vision_data[Vision_FRAME_LENGTH];
uint8_t garbage_overload;
static void screen_SendData(uint8_t data1,uint8_t data2,uint8_t data3,uint8_t data4);

/**
 * @brief: The following garbage count command DOWN
 * @param {uint8_t} garbage_type
 * @param {uint8_t} garbage_number
 * @param {uint8_t} garbage_overload
 * @return {*}
 */
void screen_Number_garbage(uint8_t garbage_type,uint8_t garbage_number,uint8_t garbage_overload)
{
    screen_SendData(01,garbage_type,garbage_number,garbage_overload);
}
/**
 * @brief: The command to identify the garbage location above UP
 * @param {uint8_t} garbage_Serial
 * @param {uint8_t} garbage_type
 * @param {uint8_t} garbage_state
 * @return {*}
 */
void screen_Identify_garbage(uint8_t garbage_Serial,uint8_t garbage_type,uint8_t garbage_state)
{
    screen_SendData(02,garbage_Serial,garbage_type,garbage_state);
}
/**
 * @brief: Command to return video
 * @return {*}
 */
void screen_return_movie(void)
{
    screen_SendData(03,0,0,0);
}

/**
 * @brief: Serial port 3 sending
 * @param {uint8_t} data1
 * @param {uint8_t} data2
 * @param {uint8_t} data3
 * @param {uint8_t} data4
 * @return {*}
 */
static void screen_SendData(uint8_t data1,uint8_t data2,uint8_t data3,uint8_t data4)
{	
    uint8_t tail  =0x80;
    uint8_t End =0x7E;
    static uint8_t screen_data[4];

    screen_data[0]=data1;
    screen_data[1]=data2;
    screen_data[2]=data3;
    screen_data[3]=data4;

    HAL_UART_Transmit(&huart6,(uint8_t *)&tail,1,0x10);
    HAL_UART_Transmit(&huart6,(uint8_t *)&screen_data,sizeof(screen_data),0x10);
    HAL_UART_Transmit(&huart6,(uint8_t *)&End,1,0x10);
};

void screen_display_task(void)
{
    if(screen_usart_flag==1)
    {
        screen_Identify_garbage(garbage_Serial,Show_garbage_categories,Vision_data[1]);
        screen_Identify_garbage(garbage_Serial,Display_garbage_quantity,1);
        screen_Identify_garbage(garbage_Serial,Display_classification_status,1);
        if(vision_garbage.Type==1) screen_Number_garbage(vision_garbage.Type,waste_number.kitchen_number,garbage_overload);
        if(vision_garbage.Type==2) screen_Number_garbage(vision_garbage.Type,waste_number.recovery_number,garbage_overload);
        if(vision_garbage.Type==3) screen_Number_garbage(vision_garbage.Type,waste_number.harmful_number,garbage_overload);
        if(vision_garbage.Type==4) screen_Number_garbage(vision_garbage.Type,waste_number.other_number,garbage_overload);
        screen_usart_flag=0;
    }
}

