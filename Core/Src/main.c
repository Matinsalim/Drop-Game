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
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DFPLAYER_MINI.h"
#include "stdbool.h"
#include "ask.h"
#include "FLASH_PAGE.h"
#include <stdio.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define SIZE 10

#define NONE 10
#define P 98
#define E 99
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
}state_Of_Game_t;
state_Of_Game_t game_State = waiting_For_Start;

uint8_t randomNumber[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
uint16_t data = 0xffff;
uint32_t seed = 0;
uint8_t x = 0;
int time_cnt = 0;
uint8_t remote_pressed = true;
ask_t rf433;
uint32_t ask_code_in_flash;
uint8_t code[3];



void ShiftOut(uint16_t data);
void DF_Choose(uint8_t);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN)
{
		if(!HAL_GPIO_ReadPin(Ext_IO3_GPIO_Port, Ext_IO3_Pin))
			game_State = button_Clicked;
}

void reset_Shift_Register()
{
	HAL_GPIO_WritePin(MR_GPIO_Port, MR_Pin, 0);
	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 1);
	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 0);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	reset_Shift_Register();
	HAL_GPIO_WritePin(MR_GPIO_Port, MR_Pin, 1);


	if(x)
	{
		ShiftOut(data);
		x=0;
	}
	else
	{
		reset_Shift_Register();
		x=1;
	}

}

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
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,1);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,1);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,1);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,1);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,0);
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
	else if(num==P)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,1);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,1);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,0);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,0);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,1);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,1);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,1);
	}
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
	else if(num==E)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,1);
		HAL_Delay(150);
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,0);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,1);
		HAL_Delay(150);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,0);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,1);
		HAL_Delay(150);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,0);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,1);
		HAL_Delay(150);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,0);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,1);
		HAL_Delay(150);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,0);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,1);
		HAL_Delay(150);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,0);
	}

}

void shuffle(uint8_t *array, uint8_t size)
{
	for (int i = size - 1; i > 0; i--) {
		int j = rand() % (i + 1);

		int temp = array[i];
		array[i] = array[j];
		array[j] = temp;
	}
}

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

void button_Click()
{
	DF_Choose(2);
	for(int i = 3; i>=0; i--)
	{
		segment_Update(i);
		HAL_Delay(1000);
	}
	segment_Update(NONE);
	game_State=playing_Game;
}

void start_Game()
{
	DF_Choose(1);

	for(int i = 0; i<16; i++)
	{

		data = data & (~(1 << randomNumber[i]));
		segment_Update(randomNumber[i]);
		HAL_Delay(1000);
	}

	segment_Update(NONE);
	DF_Pause();
	data=0xffff;
	game_State=waiting_For_Start;
}

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

void check_And_Learn_Ask()
{
	ask_loop(&rf433);
	if (ask_available(&rf433))
	{
		ask_read(&rf433, code, NULL, NULL);
		if(ask_code_in_flash == (code[0] | (code[1] << 8) | (code[2] << 16)))
		{
			HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, 1);
			remote_pressed = true;
			HAL_Delay(5);
		}
		else
		{
			remote_pressed = false;
			HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, 0);
		}

	}
	else
	{
		remote_pressed = false;
		HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, 0);
	}

	// Learn Procedure ...
	if(ask_learning_state == idle)
	{
		if(!HAL_GPIO_ReadPin(Ext_BTN_GPIO_Port, Ext_BTN_Pin))
		{
			HAL_TIM_Base_Start(&htim14);
			htim14.Instance->CNT=0;
			ask_learning_state = learning;
		}
	}
	else if(ask_learning_state == learning)
	{
		// wait for 5 seconds
		if(htim14.Instance->CNT>5000)
		{
			// ask code is valid, so save it ...
			ask_code_in_flash = code[0] | (code[1] << 8) | (code[2] << 16);
			blinking();
			Flash_Write_Data(0x08007000, &ask_code_in_flash, 1);
			HAL_TIM_Base_Stop(&htim14);
			ask_learning_state = learned;
		}

		if(HAL_GPIO_ReadPin(Ext_BTN_GPIO_Port, Ext_BTN_Pin))	// BTN is released, learning procedure failed
		{
			ask_learning_state = idle;
			HAL_TIM_Base_Stop(&htim14);
		}
	}
	else // learned
	{
		// wait for BTN to release
		if(HAL_GPIO_ReadPin(Ext_BTN_GPIO_Port, Ext_BTN_Pin))
			ask_learning_state = idle;
	}
}
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
	MX_USART2_UART_Init();
	MX_TIM3_Init();
	MX_TIM15_Init();
	MX_TIM6_Init();
	MX_TIM14_Init();
	/* USER CODE BEGIN 2 */
	DF_Init(30);
	HAL_TIM_Base_Start(&htim3);
//	HAL_TIM_Base_Start_IT(&htim15);//###########################################################################
	seed = __HAL_TIM_GET_COUNTER(&htim3);
	srand(seed);
	reset_Shift_Register();
	ask_init(&rf433,ASK_IN_SIG_GPIO_Port,ASK_IN_SIG_Pin);
	Flash_Read_Data(0x08007000, &ask_code_in_flash, 1);	// Read ASK code in Flash
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		// Receive the ask code
			check_And_Learn_Ask();

				if(game_State==waiting_For_Start)//check coin & ask & start button
				{
//					segment_Update(E);//###########################################################################
//					shuffle(randomNumber, 16);//###########################################################################
				}
				else if(game_State==button_Clicked)
					button_Click();

				else if(game_State==playing_Game)
					start_Game();
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
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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

#ifdef  USE_FULL_ASSERT
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
