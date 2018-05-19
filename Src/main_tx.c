/*
********************************************************************************
* @file    main.c 
* @author  Rasmus Källqvist
* @brief   Main for transmitter
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h" 

/* Private variables ---------------------------------------------------------*/
static union two_byte_union package_id;

/* Function declarations -----------------------------------------------------*/
/**
	* @brief  Main program
	* @param  None
	* @retval None
	*/
int main(void)
{
	/* Initialize mcu system, gpio and spi peripherals */
	system_init();

	/* Initialize the RFM96 LoRa radio chip */
	if(rfm96_init() == 0)
	{
		/* SPI connection error */
		lcd_display_str("BADSPI");
		while(1);
	}
	else
	{
		/* Signal boot ok */
		lcd_display_str("BOOTOK");
		HAL_Delay(1000);
	}
	
	/* Wait for button push */
	lcd_display_str("ready");
	while(BSP_PB_GetState(BUTTON_USER) != 0);
	while(BSP_PB_GetState(BUTTON_USER) != 1);
		
	/* Infinite loop */
	while(1)
	{	
	  	/* Display number of sent packages */
		package_id.num++;
		lcd_display_int((int)package_id.num);

		/* Send a package */
		rfm96_begin_packet();
		rfm96_write_packet(package_id.buffer, 2);
		rfm96_send_packet();
	}
}

/**
	* @brief  Reports the name of the source file and the source line number
	*         where the assert_param error has occurred.
	* @param  file: pointer to the source file name
	* @param  line: assert_param error line source number
	* @retval None
	*/
void assert_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
		 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}

/**
	* @}
	*/

/**
	* @}
	*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
