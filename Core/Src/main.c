/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include <ask_rt433.h>
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DFPLAYER_MINI.h"
#include "stdbool.h"
#include "FLASH_PAGE.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef WS_LEDS
#include "WS2811_12.h"
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define H 12
#define E 16
#define F 17

#define NONE 97
#define EFFECT_1 99
#define EFFECT_2 100



/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
typedef enum{
	idle,
	learning,
	learned
}learning_state_t;
learning_state_t ask_learning_state = idle;
typedef enum{
	waiting_For_Start,
	button_Clicked,
	playing_Game,
	difficulty_setting,
	turn_setting,
}state_Of_Game_t;
state_Of_Game_t game_State = waiting_For_Start;

uint8_t randomNumber[10]={0,1,2,3,4,5,6,8,11,12};
uint16_t data = 0x0000;
uint16_t data_before_droped = 0xFFFF;
uint32_t seed = 0;
uint8_t state_Of_Segment = 0;
uint8_t timer_For_Ask_Lern = 0;
uint8_t remote_pressed = false;
uint8_t turn = 0;
ask_t rf433;
uint32_t ask_code_in_flash;
uint8_t code[3];
uint8_t sticks_Of_Dropped = 0;
uint8_t number_Of_Stick [16] = {9,3,4,5,6,7,8,0,2,0,0,0,1,0,0,0};
uint8_t button_Blinking = 0;
uint8_t a_hold_timer_cnt = 0;
uint8_t b_hold_timer_cnt = 0;
uint8_t c_hold_timer_cnt = 0;
uint32_t magnet_state = 0;


uint8_t coin = 0;
uint8_t x = 0;

// Settings
uint32_t difficulty;		//E, H, 0, 4, 8
uint32_t turn_num;		// 1, 2, 3, F

uint32_t remote_duration_hold = 0 ,remote_start_hold = 0 , remote_is_hold = 0 ,a_hold = 0, b_hold = 0 ,c_hold = 0;



void ShiftOut(uint16_t data);
void DF_Choose(uint8_t);
void segment_Update(int num);
void timer_Update();
void reset_Magnets();
//=================================================================================================================================================================================
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN)
{
	if(!HAL_GPIO_ReadPin(Ext_IO3_GPIO_Port, Ext_IO3_Pin))
		if(coin == 1)
		{
			if (game_State == waiting_For_Start)
			{
				game_State = button_Clicked;
				if(turn==0)
					turn = turn_num;
			}
		}
}
//=================================================================================================================================================================================
void reset_Shift_Register()
{
	HAL_GPIO_WritePin(MR_GPIO_Port, MR_Pin, 0);
	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 1);
	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 0);
}
//=================================================================================================================================================================================
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	reset_Shift_Register();
	HAL_GPIO_WritePin(MR_GPIO_Port, MR_Pin, 1);
	if(x)
	ShiftOut(data);
	else
	ShiftOut(data_before_droped);

	x =! x;

}
//=================================================================================================================================================================================
void segment_Update(int num)
{
	if(num==0)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,1);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,1);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,1);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,1);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,1);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,1);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,0);
	}
	else if(num==1)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,0);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,1);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,1);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,0);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,0);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,0);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,0);
	}
	else if(num==2)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,1);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,1);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,0);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,1);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,1);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,0);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,1);
	}
	else if(num==3)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,1);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,1);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,1);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,1);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,0);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,0);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,1);
	}
	else if(num==4)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,0);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,1);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,1);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,0);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,0);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,1);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,1);
	}
	else if(num==5)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,1);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,0);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,1);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,1);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,0);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,1);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,1);
	}
	else if(num==6)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,1);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,0);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,1);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,1);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,1);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,1);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,1);
	}
	else if(num==7)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,1);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,1);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,1);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,0);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,0);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,0);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,0);
	}
	else if(num==8)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,1);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,1);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,1);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,1);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,1);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,1);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,1);
	}
	else if(num==9)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,1);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,1);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,1);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,1);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,0);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,1);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,1);
	}
	else if(num==H)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,0);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,1);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,1);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,0);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,1);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,1);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,1);
	}
	else if(num==E)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,1);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,0);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,0);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,1);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,1);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,1);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,1);
	}
	else if(num==F)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,1);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,0);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,0);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,0);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,1);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,1);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,1);
	}
	//	else if(num==PAUSE)
	//	{
	//		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,1);
	//		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,1);
	//		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,0);
	//		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,0);
	//		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,1);
	//		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,1);
	//		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,1);
	//	}
	else if(num==NONE)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,0);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,0);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,0);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,0);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,0);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,0);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,0);
	}
	else if(num==EFFECT_1)
	{
		if(state_Of_Segment==0)
		{
			HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,0);
			HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,1);
		}
		else if(state_Of_Segment==1)
		{
			HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,0);
			HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,1);
		}
		else if(state_Of_Segment==2)
		{
			HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,0);
			HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,1);
		}
		else if(state_Of_Segment==3)
		{
			HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,0);
			HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,1);
		}
		else if(state_Of_Segment==4)
		{
			HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,0);
			HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,1);
		}

		else if(state_Of_Segment==5)
		{
			HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,0);
			HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,1);
		}

		else if(state_Of_Segment>=6)
			state_Of_Segment=0;
	}
	else if(num==EFFECT_2)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,1);
		HAL_Delay(500);
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,0);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,1);
		HAL_Delay(500);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,0);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,1);
		HAL_Delay(500);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,0);
	}
}
//=================================================================================================================================================================================
void shuffle(uint8_t *array, uint8_t size)
{
	for (int i = size - 1; i > 0; i--) {
		int j = rand() % (i + 1);

		int temp = array[i];
		array[i] = array[j];
		array[j] = temp;
	}
}
//=================================================================================================================================================================================
void ShiftOut(uint16_t data)
{
	for (int i = 0; i < 16; i++)
	{
		HAL_GPIO_WritePin(SER_GPIO_Port, SER_Pin, (data & (1 << (15 - i))) ? 1 : 0);
		HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, 1);
		HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, 0);
	}
	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 1);
	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 0);
}
//=================================================================================================================================================================================
void button_Click()
{
	DF_Choose(1);
	data=0xFFFF;
	for(int i = 3; i>=0; i--)
	{
		segment_Update(i);
		HAL_Delay(1000);
	}
	segment_Update(NONE);
	game_State=playing_Game;
}
//=================================================================================================================================================================================
void start_Game()
{
	if(sticks_Of_Dropped<10)
	{

		data_before_droped = data_before_droped  & (~(1 << randomNumber[sticks_Of_Dropped]));
		HAL_Delay(250);

		if(sticks_Of_Dropped==0)
		{
			if(difficulty < 16)
				DF_Choose(2); // Plays Music
			else
			{
				DF_Choose(3); // Plays Music
				HAL_Delay(1600);
			}
		}

		if(difficulty<10)
		{
			segment_Update(number_Of_Stick[randomNumber[sticks_Of_Dropped]]);			// "9 - ..." is bug of simkeshi

#ifdef WS_LEDS
			Set_LED(9 - number_Of_Stick[randomNumber[sticks_Of_Dropped]], 64, 128, 255);
			WS2812_Send();
#endif
			HAL_Delay(difficulty * 100);
#ifdef WS_LEDS
			Set_LED(9 - number_Of_Stick[randomNumber[sticks_Of_Dropped]], 0, 0, 0);
			WS2812_Send();
#endif
		}
		else if(difficulty == 16)
		{
			data = data  & (~(1 << randomNumber[sticks_Of_Dropped]));

			sticks_Of_Dropped++;
			data_before_droped = data_before_droped  & (~(1 << randomNumber[sticks_Of_Dropped]));
			HAL_Delay(250);
		}

		data = data & (~(1 << randomNumber[sticks_Of_Dropped]));
		HAL_Delay(1250 + (800 - (difficulty*100)));
		if(difficulty == 16)
			HAL_Delay(1100);
		else if(difficulty == 12)
			HAL_Delay(400);

		sticks_Of_Dropped++;
	}
	else if(sticks_Of_Dropped==10)
	{
		if(turn_num != F)
			turn--;
		sticks_Of_Dropped=0;
		if (turn==0)
			coin=0;
		segment_Update(NONE);
		DF_Pause();
		reset_Magnets();

		game_State=waiting_For_Start;
	}

}
//=================================================================================================================================================================================
void difficultySettings()
{
	if(HAL_GPIO_ReadPin(Ext_IO3_GPIO_Port, Ext_IO3_Pin) == 0)
	{
		HAL_Delay(50);
		while(HAL_GPIO_ReadPin(Ext_IO3_GPIO_Port, Ext_IO3_Pin) == 0)
			HAL_Delay(50);
		difficulty += 4;
		if(difficulty >= 17)
			difficulty = 0;
	}
}
//=================================================================================================================================================================================
void turnSettings()
{
	if(HAL_GPIO_ReadPin(Ext_IO3_GPIO_Port, Ext_IO3_Pin) == 0)
	{
		HAL_Delay(50);
		while(HAL_GPIO_ReadPin(Ext_IO3_GPIO_Port, Ext_IO3_Pin) == 0)
			HAL_Delay(50);
		turn_num += 1;
		if(turn_num == 4)
			turn_num = F;
		else if(turn_num >= 5)
			turn_num = 1;
	}
}
//=================================================================================================================================================================================
void check_a_hold_key()
{

	if(a_hold_timer_cnt>=30)
	{
		if(game_State==difficulty_setting)
		{
			game_State =waiting_For_Start;
			Flash_Write_Data(0x08009000, &difficulty, 1);
		}
		else
			game_State=difficulty_setting;

		a_hold_timer_cnt=0;
		segment_Update(NONE);
	}
}
//=================================================================================================================================================================================
void check_b_hold_key()

{
	if(b_hold_timer_cnt>=30)
	{
		if(game_State==turn_setting)
		{
			game_State =waiting_For_Start;
			Flash_Write_Data(0x0800a000, &turn_num, 1);
			coin = 0;
			turn = 0;
			reset_Magnets();
		}
		else
			game_State=turn_setting;

		b_hold_timer_cnt=0;
		segment_Update(NONE);
	}
}
//=================================================================================================================================================================================
void check_c_hold_key()
{
	if(c_hold_timer_cnt>=30)
	{
		segment_Update(NONE);
		segment_Update(EFFECT_2);
		if(magnet_state)
		{
			data_before_droped = 0x0000;
			data = 0x0000;
			magnet_state = 0;
		}
		else
		{
			data_before_droped = 0xFFFF;
			data = 0xFFFF;
			magnet_state = 1;
		}
		Flash_Write_Data(0x0800b000, &magnet_state, 1);
		c_hold_timer_cnt=0;
		segment_Update(NONE);
	}
}
//=================================================================================================================================================================================
void blinking()
{
	for(int i=0;i<=5;i++)
	{
		HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, 1);
		HAL_Delay(75);
		HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, 0);
		HAL_Delay(75);
	}
}
//=================================================================================================================================================================================
void reset_Magnets()
{
	if(magnet_state)
	{
		data = 0xFFFF;
		data_before_droped = 0xFFFF;
	}
	else
	{
		data = 0x0000;
		data_before_droped = 0x0000;
	}
}
//=================================================================================================================================================================================
void check_And_Learn_Ask()
{

	ask_loop(&rf433);
	if (ask_available(&rf433))
	{
		ask_read(&rf433, code, NULL, NULL);
		if((ask_code_in_flash & 0x0000FFFF) == (code[0] | (code[1] << 8)))
		{
			HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, 1);
			if((code[2] & 0x0F) == 0x01)	// A
				coin = 1;
			if(ask_isHold(&rf433))
			{
				HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, 1);
				remote_is_hold = 1;
				remote_start_hold = HAL_GetTick();
				if((code[2] & 0x0F) == 0x01)	// A
					a_hold = 1;
				else if((code[2] & 0x0F) == 0x02)	// B
					b_hold = 1;
				else if((code[2] & 0x0F) == 0x04)	// C
					c_hold = 1;
			}
		}

	}
	else
		HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, 0);
//=================================================================================================================================================================================

	if(ask_learning_state == idle)	// Learn Procedure ...
	{
		if(!HAL_GPIO_ReadPin(Ext_BTN_GPIO_Port, Ext_BTN_Pin))
		{
			timer_For_Ask_Lern=0;
			ask_learning_state = learning;
		}
	}
	else if(ask_learning_state == learning)
	{
		if(timer_For_Ask_Lern==35)// wait for 5 seconds
		{
			// ask code is valid, so save it ...
			ask_code_in_flash = code[0] | (code[1] << 8) | (code[2] << 16);
			blinking();
			Flash_Write_Data(0x08008000, &ask_code_in_flash, 1);
			ask_learning_state = learned;
		}

		if(HAL_GPIO_ReadPin(Ext_BTN_GPIO_Port, Ext_BTN_Pin))	// BTN is released, learning procedure failed
			ask_learning_state = idle;

	}
	else // learned
	{
		// wait for BTN to release
		if(HAL_GPIO_ReadPin(Ext_BTN_GPIO_Port, Ext_BTN_Pin))
			ask_learning_state = idle;
	}
}
//=================================================================================================================================================================================
void timer_Update()

{

	if(remote_is_hold == 1)
		remote_duration_hold = HAL_GetTick() - remote_start_hold;

	if(htim14.Instance->CNT>150)
	{
		htim14.Instance->CNT=0;
		state_Of_Segment++;
		timer_For_Ask_Lern++;
		button_Blinking=!button_Blinking;


		if(a_hold)
		{
			if(remote_duration_hold > 500)
			{
				a_hold = 0;
				remote_is_hold = 0;			// 500 ticks passed, but a_duration_hold didn't update, so a is not held anymore. so ...
				a_hold_timer_cnt = 0;
			}
			else
				a_hold_timer_cnt++;
		}
		else if(b_hold)
		{
			if(remote_duration_hold > 500)
			{
				b_hold = 0;
				remote_is_hold = 0;
				b_hold_timer_cnt = 0;
			}
			else
				b_hold_timer_cnt++;
		}
		else if(c_hold)
		{
			if(remote_duration_hold > 500)
			{
				c_hold = 0;
				remote_is_hold = 0;
				c_hold_timer_cnt = 0;
			}
			else
				c_hold_timer_cnt++;
		}


	}
}
//=================================================================================================================================================================================
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART2_UART_Init();
	MX_TIM3_Init();
	MX_TIM15_Init();
	MX_TIM6_Init();
	MX_TIM14_Init();
	/* USER CODE BEGIN 2 */

	//	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start_IT(&htim15);
	HAL_TIM_Base_Start(&htim14);
	//	seed = __HAL_TIM_GET_COUNTER(&htim3);
	//	srand(seed);
	reset_Shift_Register();
	ask_init(&rf433,ASK_IN_SIG_GPIO_Port,ASK_IN_SIG_Pin);
	Flash_Read_Data(0x08008000, &ask_code_in_flash, 1);	// Read ASK code in Flash
	Flash_Read_Data(0x08009000, &difficulty, 1);
	Flash_Read_Data(0x0800a000, &turn_num, 1);
	Flash_Read_Data(0x0800b000, &magnet_state, 1);


	//	memset(LED_Data, 0, sizeof(LED_Data));

#ifdef WS_LEDS
	HAL_Delay(500);
	for(int i=0; i < 5; i++)
	{
		Set_LED(5 - i, 255, 255, 255);
		Set_LED(i + 5, 255, 255, 255);
		WS2812_Send();
		HAL_Delay(200);
		Set_LED(5 - i, 0, 0, 0);
		Set_LED(i + 5, 0, 0, 0);
		WS2812_Send();
	}

	HAL_Delay(500);

	for(int j = 0; j < 3; j++)
	{
		for(int i = 0; i < 10; i++)
			Set_LED(i, 255, 255, 255);
		WS2812_Send();
		HAL_Delay(150);
		for(int i = 0; i < 10; i++)
			Set_LED(i, 0, 0, 0);
		WS2812_Send();
		HAL_Delay(150);
	}
#else
	HAL_Delay(3000);
#endif

	//  HAL_Delay(3000);
	DF_Init(30);
	reset_Magnets();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		// Receive the ask code
		check_And_Learn_Ask();

		// EXT_IO4 --> Automation Device Signal
		if(HAL_GPIO_ReadPin(Coin_Reader_GPIO_Port, Coin_Reader_Pin) == 0)
			coin = 1;

		// F mode (Free)
		if(turn_num == F)
			coin = 1;

		if(game_State==waiting_For_Start)//check coin & ask & start button
		{
			segment_Update(EFFECT_1);
			timer_Update();
			shuffle(randomNumber, 10);
			if(coin)
			{
				if(button_Blinking)
				{
					data = 0xFFFF;
					data_before_droped = 0xFFFF;

				}
				else
				{
					data = 0xFBFF;
					data_before_droped = 0xFBFF;

				}
			}
			check_a_hold_key();
			check_b_hold_key();
			check_c_hold_key();


		}
		else if(game_State==button_Clicked)
			button_Click();
		else if(game_State==playing_Game)
			start_Game();
		else if(game_State==difficulty_setting)
		{
			segment_Update(difficulty);
			difficultySettings();
			timer_Update();
			check_a_hold_key();
		}
		else if(game_State==turn_setting)
		{
			segment_Update(turn_num);
			turnSettings();
			timer_Update();
			check_b_hold_key();
		}

		//			setting();
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
