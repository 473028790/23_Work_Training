#include "overload.h"
#include "main.h"
#include "motor_control.h"
#include "screen.h"

extern Down_Motor_t Down_Motor;
uint8_t voerload_state[5];
uint8_t rubbish_overload[4];
int cnt2661=0;
int voerload_state_1=0,voerload_state_2=0,voerload_state_3=0,voerload_state_4=0;
static void voerload_get(void)
{
    if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_1)==1)
    {
        voerload_state_1++;
        if(voerload_state_1>100)
        {
            voerload_state_1=0;
            voerload_state[1]=1;
        }
    }
    if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0)==1)
    {
        voerload_state_2++;
        if(voerload_state_2>100)
        {
            voerload_state_2=0;
            voerload_state[2]=1;
        }
    }
    if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_4)==1)
    {
        voerload_state_3++;
        if(voerload_state_3>100)
        {
            voerload_state_3=0;
            voerload_state[3]=1;
        }
    }
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5)==1)
    {
        voerload_state_4++;
        if(voerload_state_4>100)
        {
            voerload_state_4=0;
            voerload_state[4]=1;
        }
    }
}
uint8_t rubbish_flag=0;
static void voerload_change(void)
{
    //uint8_t rubbish_flag=0;
    for(uint8_t i=1;i<5;i++)
    {
        if(voerload_state[i]==1)
        {
            cnt2661++;
            rubbish_flag=(i+Down_Motor.motor_target+4)%5;
            voerload_state[i]=0;;
            //rubbish_flag=i;
        }  
    }

}
int cnt6=0,cnt7=0,cnt8=0,cnt9=0;
static void voerload_send(void)
{
    uint8_t overload_rubbish_number=0;
    if(rubbish_flag==1)
    {
        overload_rubbish_number=waste_number.kitchen_number;
        screen_Number_garbage(rubbish_flag,overload_rubbish_number,1);
        cnt6++;
        rubbish_flag=0;
    }
    if(rubbish_flag==2)
    {
        overload_rubbish_number=waste_number.recovery_number;
        screen_Number_garbage(rubbish_flag,overload_rubbish_number,1);
        cnt7++;
        rubbish_flag=0;
    }
    if(rubbish_flag==3)
    {
        overload_rubbish_number=waste_number.harmful_number;
        screen_Number_garbage(rubbish_flag,overload_rubbish_number,1);
        cnt8++;
        rubbish_flag=0;
    }
    if(rubbish_flag==4)
    {
        overload_rubbish_number=waste_number.other_number;
        screen_Number_garbage(rubbish_flag,overload_rubbish_number,1);
        cnt9++;
        rubbish_flag=0;
    }
}
void overload_task(void)
{
    voerload_get();
    voerload_change();
    voerload_send();
}