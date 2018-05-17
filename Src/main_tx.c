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
// dev test
union buffer_union
{
	uint8_t buffer[2];
	uint16_t num;
};

static union buffer_union sent_packages;

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
	rfm96_init();
	
	/* Wait for button push */
	lcd_display_str("ready");
	while(BSP_PB_GetState(BUTTON_USER) != 0);
	while(BSP_PB_GetState(BUTTON_USER) != 1);
		
	/* Infinite loop */
	while(1)
	{	
	  	/* Display number of sent packages */
		sent_packages.num++;
		lcd_display_int((int)sent_packages.num);

		/* Send a package */
		rfm96_begin_packet();
		rfm96_write_packet(sent_packages.buffer, 2);
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
