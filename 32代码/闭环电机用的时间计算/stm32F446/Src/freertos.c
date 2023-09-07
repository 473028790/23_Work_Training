/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor_control.h"
#include "tim.h"
#include "screen.h"
#include "vision.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId visionHandle;
osThreadId screenHandle;
osThreadId motor_controlHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void vision_tesk(void const * argument);
void screen_tesk(void const * argument);
void motor_control_tesk(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of vision */
  osThreadDef(vision, vision_tesk, osPriorityNormal, 0, 128);
  visionHandle = osThreadCreate(osThread(vision), NULL);

  /* definition and creation of screen */
  osThreadDef(screen, screen_tesk, osPriorityNormal, 0, 128);
  screenHandle = osThreadCreate(osThread(screen), NULL);

  /* definition and creation of motor_control */
  osThreadDef(motor_control, motor_control_tesk, osPriorityNormal, 0, 128);
  motor_controlHandle = osThreadCreate(osThread(motor_control), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(2);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_vision_tesk */
/**
* @brief Function implementing the vision thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vision_tesk */
void vision_tesk(void const * argument)
{
  /* USER CODE BEGIN vision_tesk */
  /* Infinite loop */
  for(;;)
  {
    vision_uart_deal();
    osDelay(2);
  }
  /* USER CODE END vision_tesk */
}

/* USER CODE BEGIN Header_screen_tesk */
/**
* @brief Function implementing the screen thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_screen_tesk */
void screen_tesk(void const * argument)
{
  /* USER CODE BEGIN screen_tesk */
  /* Infinite loop */
  for(;;)
  {
    screen_display_task();
    osDelay(2);
  }
  /* USER CODE END screen_tesk */
}

/* USER CODE BEGIN Header_motor_control_tesk */
uint8_t push_rod_motor_flage=0,Down_motor_flag=0,up_motor_tim_number=0,up_motor_sign_2=0;
uint16_t up_motor_sign_1=0,up_motor_wait_tim=500;
int cnt1,CNT2;//this useed
int cjr226=0,cue151=0;
/**
* @brief Function implementing the motor_control thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_motor_control_tesk */
void motor_control_tesk(void const * argument)
{
  /* USER CODE BEGIN motor_control_tesk */
  /* Infinite loop */
  for(;;)
  {
    if(up_motor_tim_number==1)
    {
      up_motor_sign_1++;
      if(up_motor_sign_1==up_motor_wait_tim)
      {
				cjr226++;
        up_motor_sign_2=1;
        up_motor_sign_1=0;
        up_motor_tim_number=0;
      }
    }
    if(up_motor_sign_2==1)
    {
			cue151++;
      UP_motor_move();
    }
    if(Down_motor_flag==1)
    {
      //cnt1++;
      if(Down_Motor.motor_act-Down_Motor.motor_target==2) up_motor_wait_tim=800;
      else
      {
        up_motor_wait_tim=500;
      }
      rotateToPosition(Down_Motor.motor_act,Down_Motor.motor_target);
      up_motor_tim_number=1;
      Down_motor_flag=0;
    }

    if(push_rod_motor_flage==1)
    {
      cnt1++;
      push_rod_motor_in();
      if(cnt1%2700==0) push_rod_motor_flage=3;
      //HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);
    }
    else if(push_rod_motor_flage==2)
    {
      CNT2++;
      push_rod_motor_push();
      if(CNT2%2700==0) push_rod_motor_flage=3;
      //HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
    }
    else if(push_rod_motor_flage==3)
    {
      push_rod_motor_stop();
      //HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
    }
    /*
    if(cnt1==200)
    {
      ZDT_X57_Bypass_Position_LV_Control(1,1,100, 900, 0, 0);
      cnt1=0;
    }
    
    if(cnt3==1)
    {
      Tim_3_ch2_motor.Pulse++;
      Tim_3_ch2_motor.Actual_angle =Tim_3_ch2_motor.Pulse* 360.0 /3200.0;
      if(Tim_3_ch2_motor.direction==0) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);
      else if(Tim_3_ch2_motor.direction==1) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
      if(Tim_3_ch2_motor.Actual_angle <= Tim_3_ch2_motor.Target_angle)
      {
        __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 500);
      }
      else
      {
        __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 0);
        Tim_3_ch2_motor.Target_angle=0;
        Tim_3_ch2_motor.Actual_angle=0;
        Tim_3_ch2_motor.Pulse=0;
        cnt3=0;
      }
    }
    */
    osDelay(2);
  }
  /* USER CODE END motor_control_tesk */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
