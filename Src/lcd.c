/**
	******************************************************************************
	* File Name          : LCD.c
	* Description        : This file provides code for the configuration
	*                      of the LCD instances.
	******************************************************************************
	*
	* COPYRIGHT(c) 2018 STMicroelectronics
	*
	* Redistribution and use in source and binary forms, with or without modification,
	* are permitted provided that the following conditions are met:
	*   1. Redistributions of source code must retain the above copyright notice,
	*      this list of conditions and the following disclaimer.
	*   2. Redistributions in binary form must reproduce the above copyright notice,
	*      this list of conditions and the following disclaimer in the documentation
	*      and/or other materials provided with the distribution.
	*   3. Neither the name of STMicroelectronics nor the names of its contributors
	*      may be used to endorse or promote products derived from this software
	*      without specific prior written permission.
	*
	* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
	* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	*
	******************************************************************************
	*/

/* Includes ------------------------------------------------------------------*/
#include "lcd.h"
	

LCD_HandleTypeDef hlcd;

/* helper function */
static int quick_pow10(int n)
{
    static int pow10[10] = {
        1, 10, 100, 1000, 10000, 
        100000, 1000000, 10000000, 100000000, 1000000000
    };

    return pow10[n];
}

/* wrapper function */
void lcd_display_str(uint8_t* ptr)
{
	BSP_LCD_GLASS_Clear();
	BSP_LCD_GLASS_DisplayString(ptr); 
}

/* wrapper function */
void lcd_display_int(int val)
{
	uint8_t string[6]; 
	sprintf((char*)string, "%d", val); 
	BSP_LCD_GLASS_Clear();  
	BSP_LCD_GLASS_DisplayString(string);
}

/* prints a float to the display with decimal point, a little hacky */
void lcd_display_float(float val)
{
	int i = 0;
	double fraction, integer;
	uint8_t frac_str[6], int_str[6];
	int integer_digits;
	
	/* Clear display */
	BSP_LCD_GLASS_Clear();  

	/* Split float into integer and fraction parts */
	fraction = modf(val, &integer);
	fraction = fraction < 0 ? -fraction : fraction; // take absolute value 

	/* Convert integer and fractional number parts into strings */
	sprintf((char*)int_str, "%g", integer);
	fraction *= quick_pow10(strlen((char*)int_str)); // shift decimal places 
	sprintf((char*)frac_str, "%d", ((int)fraction));
	integer_digits = strlen((char*)int_str);

	/* Print integer part, with a decimal point behind last digit */
	while(i < integer_digits - 1)
	{
		BSP_LCD_GLASS_WriteChar(&int_str[i], POINT_OFF, DOUBLEPOINT_OFF, i+1);	
		i++;
	}
	BSP_LCD_GLASS_WriteChar(&int_str[i], POINT_ON, DOUBLEPOINT_OFF, i+1);	
	i++;

	/* Print fraction part */
	while(i < 6)
	{
		BSP_LCD_GLASS_WriteChar(&frac_str[i-integer_digits], POINT_OFF, DOUBLEPOINT_OFF, i+1);	
		i++;
	}
}

/* prints a float with a percentage sign after */
void lcd_display_percentage(float val)
{
	BSP_LCD_GLASS_Clear();
	uint8_t ch1[2] = "°";
	uint8_t ch2 = '/';
	uint8_t ch3 = '%';

	/* Turn input value into a 3 digit string */
	int integer_val = (int)(val * 100);
	uint8_t str[3];
	sprintf((char*)str, "%d", integer_val);

	/* Print percentage value */
	int i;
	for(i = 0; i < strlen((char*)str); i++)
	{
		BSP_LCD_GLASS_WriteChar(&str[i], POINT_OFF, DOUBLEPOINT_OFF, i+1);	
	}
	i++;

	BSP_LCD_GLASS_WriteChar(&ch1[1], POINT_OFF, DOUBLEPOINT_OFF, i++);
	BSP_LCD_GLASS_WriteChar(&ch2, POINT_OFF, DOUBLEPOINT_OFF, i++);
	BSP_LCD_GLASS_WriteChar(&ch3, POINT_OFF, DOUBLEPOINT_OFF, i++);

}

/* wrapper function, waits before returning  */
void lcd_display_str_delayed(uint8_t* ptr, int delay_time)
{
	lcd_display_str(ptr);
	HAL_Delay(delay_time);
}

/* wrapper function, waits before returning  */
void lcd_display_int_delayed(int val, int delay_time)
{
	lcd_display_int(val);
	HAL_Delay(delay_time);
}

/* wrapper function, waits before returning  */
void lcd_display_float_delayed(float val, int delay_time)
{
	lcd_display_float(val);
	HAL_Delay(delay_time);
}

/* LCD init function */
void MX_LCD_Init(void)
{

	hlcd.Instance = LCD;
	hlcd.Init.Prescaler = LCD_PRESCALER_1;
	hlcd.Init.Divider = LCD_DIVIDER_16;
	hlcd.Init.Duty = LCD_DUTY_1_4;
	hlcd.Init.Bias = LCD_BIAS_1_4;
	hlcd.Init.VoltageSource = LCD_VOLTAGESOURCE_INTERNAL;
	hlcd.Init.Contrast = LCD_CONTRASTLEVEL_0;
	hlcd.Init.DeadTime = LCD_DEADTIME_0;
	hlcd.Init.PulseOnDuration = LCD_PULSEONDURATION_0;
	hlcd.Init.MuxSegment = LCD_MUXSEGMENT_DISABLE;
	hlcd.Init.BlinkMode = LCD_BLINKMODE_OFF;
	hlcd.Init.BlinkFrequency = LCD_BLINKFREQUENCY_DIV8;
	if (HAL_LCD_Init(&hlcd) != HAL_OK)
	{
		Error_Handler();
	}

}

void HAL_LCD_MspInit(LCD_HandleTypeDef* lcdHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	if(lcdHandle->Instance==LCD)
	{
	/* USER CODE BEGIN LCD_MspInit 0 */

	/* USER CODE END LCD_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_LCD_CLK_ENABLE();
	
		/**LCD GPIO Configuration    
		PC0     ------> LCD_SEG18
		PC1     ------> LCD_SEG19
		PC2     ------> LCD_SEG20
		PC3     ------> LCD_SEG21
		PA1     ------> LCD_SEG0
		PA2     ------> LCD_SEG1
		PA3     ------> LCD_SEG2
		PB10     ------> LCD_SEG10
		PB11     ------> LCD_SEG11
		PB12     ------> LCD_SEG12
		PB13     ------> LCD_SEG13
		PB14     ------> LCD_SEG14
		PB15     ------> LCD_SEG15
		PC6     ------> LCD_SEG24
		PC7     ------> LCD_SEG25
		PC8     ------> LCD_SEG26
		PC9     ------> LCD_SEG27
		PA8     ------> LCD_COM0
		PA9     ------> LCD_COM1
		PA10     ------> LCD_COM2
		PA15     ------> LCD_SEG17
		PC10     ------> LCD_SEG28
		PC11     ------> LCD_SEG29
		PC12     ------> LCD_SEG30
		PD2     ------> LCD_SEG31
		PB3     ------> LCD_SEG7
		PB4     ------> LCD_SEG8
		PB5     ------> LCD_SEG9
		PB8     ------> LCD_SEG16
		PB9     ------> LCD_COM3 
		*/
		GPIO_InitStruct.Pin = SEG14_Pin|SEG15_Pin|SEG16_Pin|SEG17_Pin 
													|SEG18_Pin|SEG19_Pin|SEG20_Pin|SEG21_Pin 
													|SEG22_Pin|SEG23_Pin|GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = SEG0_Pin|SEG1_Pin|SEG2_Pin|COM0_Pin 
													|COM1_Pin|COM2_Pin|SEG12_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = SEG6_Pin|SEG7_Pin|SEG8_Pin|SEG9_Pin 
													|SEG10_Pin|SEG11_Pin|SEG3_Pin|SEG4_Pin 
													|SEG5_Pin|SEG13_Pin|COM3_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* USER CODE BEGIN LCD_MspInit 1 */

	/* USER CODE END LCD_MspInit 1 */
	}
}

void HAL_LCD_MspDeInit(LCD_HandleTypeDef* lcdHandle)
{

	if(lcdHandle->Instance==LCD)
	{
	/* USER CODE BEGIN LCD_MspDeInit 0 */

	/* USER CODE END LCD_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_LCD_CLK_DISABLE();
	
		/**LCD GPIO Configuration    
		PC0     ------> LCD_SEG18
		PC1     ------> LCD_SEG19
		PC2     ------> LCD_SEG20
		PC3     ------> LCD_SEG21
		PA1     ------> LCD_SEG0
		PA2     ------> LCD_SEG1
		PA3     ------> LCD_SEG2
		PB10     ------> LCD_SEG10
		PB11     ------> LCD_SEG11
		PB12     ------> LCD_SEG12
		PB13     ------> LCD_SEG13
		PB14     ------> LCD_SEG14
		PB15     ------> LCD_SEG15
		PC6     ------> LCD_SEG24
		PC7     ------> LCD_SEG25
		PC8     ------> LCD_SEG26
		PC9     ------> LCD_SEG27
		PA8     ------> LCD_COM0
		PA9     ------> LCD_COM1
		PA10     ------> LCD_COM2
		PA15     ------> LCD_SEG17
		PC10     ------> LCD_SEG28
		PC11     ------> LCD_SEG29
		PC12     ------> LCD_SEG30
		PD2     ------> LCD_SEG31
		PB3     ------> LCD_SEG7
		PB4     ------> LCD_SEG8
		PB5     ------> LCD_SEG9
		PB8     ------> LCD_SEG16
		PB9     ------> LCD_COM3 
		*/
		HAL_GPIO_DeInit(GPIOC, SEG14_Pin|SEG15_Pin|SEG16_Pin|SEG17_Pin 
													|SEG18_Pin|SEG19_Pin|SEG20_Pin|SEG21_Pin 
													|SEG22_Pin|SEG23_Pin|GPIO_PIN_12);

		HAL_GPIO_DeInit(GPIOA, SEG0_Pin|SEG1_Pin|SEG2_Pin|COM0_Pin 
													|COM1_Pin|COM2_Pin|SEG12_Pin);

		HAL_GPIO_DeInit(GPIOB, SEG6_Pin|SEG7_Pin|SEG8_Pin|SEG9_Pin 
													|SEG10_Pin|SEG11_Pin|SEG3_Pin|SEG4_Pin 
													|SEG5_Pin|SEG13_Pin|COM3_Pin);

		HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

	}
	/* USER CODE BEGIN LCD_MspDeInit 1 */

	/* USER CODE END LCD_MspDeInit 1 */
} 

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
	* @}
	*/

/**
	* @}
	*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
