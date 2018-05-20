/*
********************************************************************************
* File Name          : main.h
* Description        : Header file for main program body 
********************************************************************************
*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "system_util.h"
#include "stm32l1xx_hal.h"
#include "stm32l152c_discovery.h"
#include "gpio.h"
#include "spi.h"
#include "lora.h"
#include "lcd.h"

/* Defines -------------------------------------------------------------------*/
#define DISPLAY_DELAY              800  // ms
#define BUTTON_HELD_LONG           500  // ms
#define MAX_EXPECTED_NUM_PACKAGES  1000	
#define LED_GREEN                  LED3
#define LED_BLUE                   LED4

/* Unions --------------------------------------------------------------------*/
union two_byte_union
{
	uint8_t buffer[2];
	uint16_t num;
};

/* Function declarations -----------------------------------------------------*/
void wait_for_user_button(void);

#endif // __MAIN_H	