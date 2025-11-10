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
#include "dma.h"
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
#include "WS2811_12.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define SIZE 10

#define NONE 120
#define PAUSE 98
#define EFFECT 99
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
	win_state,
	loose_state,
}state_Of_Game_t;
state_Of_Game_t game_State = waiting_For_Start;

uint8_t randomNumber[10]={0,1,2,3,4,5,6,8,11,12};
uint16_t data = 0x0000;
uint32_t seed = 0;
uint8_t x = 0;
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
uint8_t a_hold_timer_cnt = 0, a_not_hold_timer_cnt = 0;
uint8_t b_hold_timer_cnt = 0, b_not_hold_timer_cnt = 0;
uint8_t coin = 0;

// Settings
uint32_t difficulty;		// H, 0, 2, 4, 6, 8
uint32_t turn_num;		// 1, 2, 3


uint8_t segment_num = 0;
uint8_t game_time_s = 0;
uint8_t seg_state = 0;
uint16_t game_timer_ms = 0;
uint32_t hall_sensor_ints = 0;
uint8_t progress = 0;

void ShiftOut(uint16_t data);
void DF_Choose(uint8_t);
void segment_Update(int num);
void timer_Update();

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN)
{
	if(GPIO_PIN == ASK_IN_SIG_Pin)
	{
//		if(!HAL_GPIO_ReadPin(Ext_IO3_GPIO_Port, Ext_IO3_Pin))
//			if(coin == 1)
//			{
//				if (game_State == waiting_For_Start)
//				{
//					game_State = button_Clicked;
//					if(turn==0)
//						turn = turn_num;
//				}
//			}
	}
	else if(GPIO_PIN == GPIO_PIN_2)	// Hall effect sensor
	{
		if(game_State == playing_Game)
		{
			hall_sensor_ints++;
		}
	}
}

void reset_Shift_Register()
{
	HAL_GPIO_WritePin(MR_GPIO_Port, MR_Pin, 0);
	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 1);
	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 0);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
//	reset_Shift_Register();
//	HAL_GPIO_WritePin(MR_GPIO_Port, MR_Pin, 1);
//	ShiftOut(data);
	if(htim->Instance == TIM14)	// every 1ms
	{
		// Segment Codes ...
		if(segment_num != NONE)
		{
			int d0 = segment_num % 10;
			int d1 = segment_num / 10;
			seg_state++;
			if(seg_state == 7)	// 5ms
			{
				HAL_GPIO_WritePin(SA1_GPIO_Port, SA1_Pin, 1);
				HAL_GPIO_WritePin(SA2_GPIO_Port, SA2_Pin, 0);
				segment_Update(d1);
			}
			else if(seg_state == 14)	// 10ms
			{
				HAL_GPIO_WritePin(SA1_GPIO_Port, SA1_Pin, 0);
				HAL_GPIO_WritePin(SA2_GPIO_Port, SA2_Pin, 1);
				segment_Update(d0);
				seg_state = 0;
			}
		}
		else
		{
			HAL_GPIO_WritePin(SA1_GPIO_Port, SA1_Pin, 0);
			HAL_GPIO_WritePin(SA2_GPIO_Port, SA2_Pin, 0);
		}

		// game codes
		if(game_State == playing_Game)
		{
			game_timer_ms++;
			if(game_timer_ms == 1000)
			{
				game_timer_ms = 0;
				game_time_s--;
			}
		}
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
	else if(num==10)
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,0);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,1);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,1);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,0);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,1);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,1);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,1);
	}
	else if(num==PAUSE)
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
	else if(num==EFFECT)
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


//void display_segment()
//{
//	int d0 = segment_num % 10;
//	int d1 = segment_num / 10;
//
//	HAL_GPIO_WritePin(SA1_GPIO_Port, SA1_Pin, 1);
//	HAL_GPIO_WritePin(SA2_GPIO_Port, SA2_Pin, 0);
//	segment_Update(d1);
//	HAL_Delay(5);
//	HAL_GPIO_WritePin(SA1_GPIO_Port, SA1_Pin, 0);
//	HAL_GPIO_WritePin(SA2_GPIO_Port, SA2_Pin, 1);
//	segment_Update(d0);
//	HAL_Delay(5);
//}

void display_nothing()
{
	HAL_GPIO_WritePin(SA1_GPIO_Port, SA1_Pin, 0);
	HAL_GPIO_WritePin(SA2_GPIO_Port, SA2_Pin, 0);
	segment_Update(NONE);
}


void button_Click()
{
	DF_Choose(1);
	for(int i = 3; i >= 0; i--)
	{
		segment_num = i;
		HAL_Delay(700);
		segment_num = NONE;
		HAL_Delay(200);
	}
	game_State=playing_Game;
	DF_Choose(2);
	htim14.Instance->CNT = 0;
	game_time_s = 30;
}

void start_Game()
{

	segment_num = game_time_s;

	// write codes with "hall_sensor_ints"
//	if(hall_sensor_ints < 30000)
//		progress = hall_sensor_ints / 1000;
//	else if(hall_sensor_ints < 100000)
//		progress = hall_sensor_ints / 2000;
//	else if(hall_sensor_ints < 150000)
//		progress = hall_sensor_ints / 1875;

//	if(hall_sensor_ints < 30)
//		progress = hall_sensor_ints / 1;
//	else if(hall_sensor_ints < 100)
//		progress = hall_sensor_ints / 2;
//	else if(hall_sensor_ints < 150)
//		progress = hall_sensor_ints / 1;

	if(hall_sensor_ints == 10)
	{
		DF_Choose(3);
		hall_sensor_ints++;
	}
	else if(hall_sensor_ints == 20)
	{
		DF_Choose(3);
		hall_sensor_ints++;
	}
	else if(hall_sensor_ints == 30)
	{
		DF_Choose(5);
		hall_sensor_ints++;

		data = 0xFFF;
		reset_Shift_Register();
		HAL_GPIO_WritePin(MR_GPIO_Port, MR_Pin, 1);
		ShiftOut(data);
		HAL_Delay(500);
		data = 0x000;
		reset_Shift_Register();
		HAL_GPIO_WritePin(MR_GPIO_Port, MR_Pin, 1);
		ShiftOut(data);
	}


//	Set_LED(END_OF_LEDs_NUM - (progress * END_OF_LEDs_NUM / 100), 255, 255, 255);
//	WS2812_Send();


}




// ASK usage
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
//	bool isNew = false;

	ask_loop(&rf433);
	if (ask_available(&rf433))
	{
		ask_read(&rf433, code, NULL, NULL);

		// Check if is this ask code the main remote ?!
		if((ask_code_in_flash & 0x0000FFFF) == (code[0] | (code[1] << 8)))
		{
			// A check hold and release
			if((code[2] & 0x0F) == 0x01)	// A
			{
				HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, 1);
				if(game_State == waiting_For_Start)
					game_State = button_Clicked;
				HAL_Delay(5);
			}
		}
	}
	else
		HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, 0);


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

void timer_Update()
{
//	if(htim14.Instance->CNT>150)
//	{
//		htim14.Instance->CNT=0;
//		state_Of_Segment++;
//		timer_For_Ask_Lern++;
//		button_Blinking=!button_Blinking;
//	}
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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_TIM15_Init();
  MX_TIM6_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */

//	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start_IT(&htim15);
	HAL_TIM_Base_Start_IT(&htim14);
//	seed = __HAL_TIM_GET_COUNTER(&htim3);
//	srand(seed);
	reset_Shift_Register();
	ask_init(&rf433,ASK_IN_SIG_GPIO_Port,ASK_IN_SIG_Pin);
	Flash_Read_Data(0x08008000, &ask_code_in_flash, 1);	// Read ASK code in Flash
	Flash_Read_Data(0x08009000, &difficulty, 1);
	Flash_Read_Data(0x0800a000, &turn_num, 1);

//	memset(LED_Data, 0, sizeof(LED_Data));

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


	//  HAL_Delay(3000);
		DF_Init(30);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{

		// Receive the ask code
		check_And_Learn_Ask();

		if(game_State==waiting_For_Start)//check coin & ask & start button
		{
			timer_Update();
		}
		else if(game_State==button_Clicked)
			button_Click();
		else if(game_State==playing_Game)
		{
			start_Game();
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
