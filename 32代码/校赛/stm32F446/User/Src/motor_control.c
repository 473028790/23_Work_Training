#include "motor_control.h"
static void motor_usart_SendCmd(uint8_t *cmd, uint8_t len);
extern int down_motor_send_flag;

Motor_open_t Garbage_inlet_motor;
Down_Motor_t Down_Motor;
waste_number_t waste_number;
extern uint8_t up_motor_sign_2;
uint8_t up_motor_move_flag;
/**
	*	@brief		直通位置限速模式
	*	@param		addr	：电机地址
	*	@param		dir			：方向										，0为CW，其余值为CCW
	*	@param		velocity：最大速度(RPM)					，范围0.0 - 4000.0RPM
	*	@param		position：位置(°)								，范围0.0°- (2^32 - 1)°
	*	@param		raf			：相位位置/绝对位置标志	，0为相对位置，其余值为绝对位置
	*	@param		multi		：多机同步标志						，0为不启用，其余值启用
	*	@retval		地址 + 功能码 + 命令状态 + 校验字节
	*/
uint8_t cmd[12]; 
void ZDT_X57_Bypass_Position_LV_Control(uint8_t addr, uint8_t dir, float velocity, float position, uint8_t raf, uint8_t multi)
{
	//uint8_t cmd[12] = {0}; 
    uint16_t vel = 0; 
    uint32_t pos = 0;
	// 将速度和位置放大10倍发送过去
	vel = (uint16_t)abs(velocity * 10.0f); pos = (uint32_t)abs(position * 10.0f);

	// 装载命令
	cmd[0]  =  addr;											// 地址
	cmd[1]  =  0xFB;											// 功能码
	cmd[2]  =  dir;												// 符号（方向）
	cmd[3]  =  (uint8_t)(vel >> 8);				// 最大速度(RPM)高8位字节
	cmd[4]  =  (uint8_t)(vel >> 0);				// 最大速度(RPM)低8位字节	
	cmd[5]  =  (uint8_t)(pos >> 24);			// 位置(bit24 - bit31)
	cmd[6]  =  (uint8_t)(pos >> 16);			// 位置(bit16 - bit23)
	cmd[7]  =  (uint8_t)(pos >> 8);				// 位置(bit8  - bit15)
	cmd[8]  =  (uint8_t)(pos >> 0);				// 位置(bit0  - bit7 )
	cmd[9]  =  raf;												// 相位位置/绝对位置标志
	cmd[10] =  multi;											// 多机同步运动标志
	cmd[11] =  0x6B;											// 校验字节
	
	// 发送命令
	motor_usart_SendCmd(cmd, 12);
}
uint8_t cmd2[16]; 
void ZDT_X57_Traj_Position_Control(uint8_t addr, uint8_t dir, uint16_t acc, uint16_t dec, float velocity, float position, uint8_t raf, uint8_t multi)
{
    uint16_t vel = 0; 
    uint32_t pos = 0;
	// 将速度和位置放大10倍发送过去
	vel = (uint16_t)abs(velocity * 10.0f); pos = (uint32_t)abs(position * 10.0f);

	// 装载命令
	cmd2[0]  =  addr;											// 地址
	cmd2[1]  =  0xFD;											// 功能码
	cmd2[2]  =  dir;												// 符号（方向）
	cmd2[3]  =  (uint8_t)(acc >> 8);				// 加速加速度(RPM/s)高8位字节
	cmd2[4]  =  (uint8_t)(acc >> 0);				// 加速加速度(RPM/s)低8位字节	
	cmd2[5]  =  (uint8_t)(dec >> 8);				// 减速加速度(RPM/s)高8位字节
	cmd2[6]  =  (uint8_t)(dec >> 0);				// 减速加速度(RPM/s)低8位字节	
	cmd2[7]  =  (uint8_t)(vel >> 8);				// 最大速度(RPM)高8位字节
	cmd2[8]  =  (uint8_t)(vel >> 0);				// 最大速度(RPM)低8位字节	
	cmd2[9]  =  (uint8_t)(pos >> 24);			// 位置(bit24 - bit31)
	cmd2[10] =  (uint8_t)(pos >> 16);			// 位置(bit16 - bit23)
	cmd2[11] =  (uint8_t)(pos >> 8);				// 位置(bit8  - bit15)
	cmd2[12] =  (uint8_t)(pos >> 0);				// 位置(bit0  - bit7 )
	cmd2[13] =  raf;												// 相位位置/绝对位置标志
	cmd2[14] =  multi;											// 多机同步运动标志
	cmd2[15] =  0x6B;											// 校验字节
	
	// 发送命令
	motor_usart_SendCmd(cmd2, 16);
}



uint8_t cmd1[3]; 
void ZDT_X57_Reading_motor_position_error(uint8_t addr)
{
	// 装载命令
	cmd1[0]  =  addr;											// 地址
	cmd1[1]  =  0x37;											// 功能码
	cmd1[2] =  0x6B;											// 校验字节
	// 发送命令
	HAL_UART_Transmit(&huart5,(uint8_t *)cmd1,3,0x10);
}

/**
 * @brief: Motor_Uart_send
 * @param {uint8_t} *cmd
 * @param {uint8_t} len
 * @return {*}
 */
static void motor_usart_SendCmd(uint8_t *cmd, uint8_t len)
{
	HAL_UART_Transmit(&huart5,(uint8_t *)cmd,16,0x10);
}
int clockwiseSteps,counterClockwiseSteps;
void rotateToPosition(int currentPos, int targetPos)
{
     clockwiseSteps = (targetPos - currentPos + 4) % 4; // 顺时针旋转的步数
     counterClockwiseSteps = (currentPos - targetPos + 4) % 4; // 逆时针旋转的步数

    if (clockwiseSteps <= counterClockwiseSteps) {
        // 顺时针旋转
		if(clockwiseSteps!=0)
		{
			//ZDT_X57_Bypass_Position_LV_Control(1,1,600.0f,900.0f*clockwiseSteps,0,0);
						ZDT_X57_Traj_Position_Control(1,1,2000,2000,3000,900.0f*clockwiseSteps,0,0);
		}
    } else {
        // 逆时针旋转
		if(clockwiseSteps!=0)
		{
			//ZDT_X57_Bypass_Position_LV_Control(1,0,600.0f,900.0f*counterClockwiseSteps,0,0);
			ZDT_X57_Traj_Position_Control(1,0,2000,2000,3000,900.0f*counterClockwiseSteps,0,0);
		}
    }
}
extern uint8_t vision_send_sign_1;
int cnt262=0,cnt26=0;
void UP_motor_move(void)
{
	if(up_motor_move_flag==1)
    {
		Garbage_inlet_motor.direction=1;
		Garbage_inlet_motor.Pulse++;
		Garbage_inlet_motor.Target_angle=330;
		if(Garbage_inlet_motor.direction==1) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);
		if(Garbage_inlet_motor.direction==2) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
		if(Garbage_inlet_motor.Pulse <= Garbage_inlet_motor.Target_angle)
		{
			vision_send_sign_1=1;
			cnt26++;
			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 20);
		}
		else
		{
			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 0);
			Garbage_inlet_motor.Target_angle=0;
			Garbage_inlet_motor.Actual_angle=0;
			Garbage_inlet_motor.Pulse=0;
			up_motor_move_flag=2;
		}
	}

	if(up_motor_move_flag==2)
    {
		Garbage_inlet_motor.direction=2;
		Garbage_inlet_motor.Pulse++;
		Garbage_inlet_motor.Target_angle=370;
		if(Garbage_inlet_motor.direction==1) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);
		if(Garbage_inlet_motor.direction==2) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
		if(Garbage_inlet_motor.Pulse <= Garbage_inlet_motor.Target_angle)
		{
			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 25);
		}
		else
		{
			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 0);
			Garbage_inlet_motor.Target_angle=0;
			Garbage_inlet_motor.Actual_angle=0;
			Garbage_inlet_motor.Pulse=0;
			up_motor_move_flag=0;
			up_motor_sign_2=0;
			vision_send_sign_1=2;
			cnt262++;
			//down_motor_send_flag=1;
		}
	}
}
void push_rod_motor_push(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);
}
void push_rod_motor_in(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
}
void push_rod_motor_stop(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);
}

