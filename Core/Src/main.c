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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DFPLAYER_MINI.h"
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define SIZE 10


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t state = 0;
uint8_t waiting_For_Start = 0;
uint8_t button_Clicked = 1;
uint8_t playing_Game = 2;
uint8_t randomNumber[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
uint16_t data = 0;
uint32_t seed = 0;




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
	else
	{
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin,0);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin,0);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin,0);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin,0);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,0);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin,0);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin,0);
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


void button_Click()
{
	DF_Choose(2);
	for(int i = 3; i>=0; i--)
	{
		segment_Update(i);
		HAL_Delay(1000);
	}
	segment_Update(10);
	state=playing_Game;
}

//void set_shiftregister_bit(uint8_t num)
//{
//	// reset all bits
//
//	// set one bit to start shift
//	HAL_GPIO_WritePin(SER_GPIO_Port, SER_Pin, 1);
//	HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin,1);
//	HAL_Delay(10);
//	HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin,0);
//	HAL_Delay(10);
//	HAL_GPIO_WritePin(SER_GPIO_Port, SER_Pin, 1);
//
//
//	// shift num times
//	for(int i = 0; i < num; i++)
//	{
//		HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin,1);
//		HAL_Delay(10);
//		HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin,0);
//		HAL_Delay(10);
//	}
//
//	// latch output
//
//}


void start_Game()
{
	DF_Choose(1);
	shuffle(randomNumber, 16);

	HAL_GPIO_WritePin(MR_GPIO_Port, MR_Pin, 0);
	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 1);
	HAL_Delay(1);
	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 0);
	HAL_Delay(1);
	HAL_GPIO_WritePin(MR_GPIO_Port, MR_Pin, 1);
	HAL_Delay(1);

	for(int i = 0; i<16; i++)
	{

		data = data | (0x01 << randomNumber[i]);
		for(int j=0;j<16;j++)
		{
			HAL_GPIO_WritePin(SER_GPIO_Port, SER_Pin, (data >> j) & 1);
			HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, 1);
			HAL_Delay(1);
			HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, 0);
			HAL_Delay(1);
		}
		segment_Update(randomNumber[i]);
		HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 1);
		HAL_Delay(1);
		HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 0);
		HAL_Delay(1);


		//		set_shiftregister_bit(randomNumber[i]);
		HAL_Delay(1000);
	}

	HAL_GPIO_WritePin(MR_GPIO_Port, MR_Pin, 0);
	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 1);
	HAL_Delay(1);
	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 0);
	HAL_Delay(1);
	segment_Update(10);
	DF_Pause();
	data=0;
	state=waiting_For_Start;
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
	/* USER CODE BEGIN 2 */
	DF_Init(30);
	HAL_TIM_Base_Start(&htim3);

	seed = __HAL_TIM_GET_COUNTER(&htim3);
	srand(seed);
	HAL_GPIO_WritePin(MR_GPIO_Port, MR_Pin, 0);
	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 1);
	HAL_Delay(10);
	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, 0);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		if(state==waiting_For_Start)//check coin & ask & start button
		{
			if(!HAL_GPIO_ReadPin(Ext_IO1_GPIO_Port, Ext_IO1_Pin))
				state = button_Clicked;
			shuffle(randomNumber, 16);
		}
		else if(state==button_Clicked)
			button_Click();

		else if(state==playing_Game)
			start_Game();
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

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void)
{

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 0;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 48000-1;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 9600;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, SER_Pin|LATCH_Pin|CLK_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(MR_GPIO_Port, MR_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, MCU_LED_Pin|G_Pin|F_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, E_Pin|D_Pin|C_Pin|B_Pin
			|A_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : SER_Pin LATCH_Pin CLK_Pin */
	GPIO_InitStruct.Pin = SER_Pin|LATCH_Pin|CLK_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : MR_Pin */
	GPIO_InitStruct.Pin = MR_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(MR_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : Ext_BTN_Pin */
	GPIO_InitStruct.Pin = Ext_BTN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(Ext_BTN_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : ASK_IN_SIG_Pin */
	GPIO_InitStruct.Pin = ASK_IN_SIG_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ASK_IN_SIG_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : Ext_IO4_Pin Ext_IO3_Pin Ext_IO2_Pin Ext_IO1_Pin
                           Coin_Reader_Pin */
	GPIO_InitStruct.Pin = Ext_IO4_Pin|Ext_IO3_Pin|Ext_IO2_Pin|Ext_IO1_Pin
			|Coin_Reader_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : MCU_LED_Pin G_Pin F_Pin */
	GPIO_InitStruct.Pin = MCU_LED_Pin|G_Pin|F_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : E_Pin D_Pin C_Pin B_Pin
                           A_Pin */
	GPIO_InitStruct.Pin = E_Pin|D_Pin|C_Pin|B_Pin
			|A_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
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
