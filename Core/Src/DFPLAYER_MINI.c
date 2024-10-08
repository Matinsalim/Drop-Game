/*
 * DFPLAYER_MINI.c
 *
 *  Created on: May 16, 2020
 *      Author: controllerstech
 */


#include "stm32f0xx_hal.h"
#include "stdio.h"

extern UART_HandleTypeDef huart2;
#define DF_UART &huart2

#define Source      0x02  // TF CARD


/*************************************** NO CHANGES AFTER THIS *************************************************/

int ispause =0;
int isplaying=1;


# define Start_Byte 0x7E
# define End_Byte   0xEF
# define Version    0xFF
# define Cmd_Len    0x06
# define Feedback   0x00    //If need for Feedback: 0x01,  No Feedback: 0

void Send_cmd (uint8_t cmd, uint8_t Parameter1, uint8_t Parameter2)
{
	uint16_t Checksum = Version + Cmd_Len + cmd + Feedback + Parameter1 + Parameter2;
	Checksum = 0-Checksum;

	uint8_t CmdSequence[10] = { Start_Byte, Version, Cmd_Len, cmd, Feedback, Parameter1, Parameter2, (Checksum>>8)&0x00ff, (Checksum&0x00ff), End_Byte};

	HAL_UART_Transmit(DF_UART, CmdSequence, 10, HAL_MAX_DELAY);
}

void DF_PlayFromStart(void)
{
  Send_cmd(0x03,0x00,0x01);
  HAL_Delay(200);
}
void DF_Choose (uint8_t par)
{
	Send_cmd(0x03, 0,par);
	HAL_Delay(200);
}
void DF_Init (uint8_t volume)
{
	Send_cmd(0x3F, 0x00, Source);
	HAL_Delay(200);
	Send_cmd(0x06, 0x00, volume);
	HAL_Delay(500);
}

void DF_Next (void)
{
	Send_cmd(0x01, 0x00, 0x00);
	HAL_Delay(200);
}

void DF_Pause (void)
{
	Send_cmd(0x0E, 0, 0);
	HAL_Delay(200);
}

void DF_Previous (void)
{
	Send_cmd(0x02, 0, 0);
	HAL_Delay(200);
}

void DF_Playback (void)
{
	Send_cmd(0x0D, 0, 0);
	HAL_Delay(200);
}
















