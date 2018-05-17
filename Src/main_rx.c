/*
********************************************************************************
* @file    main.c 
* @author  Rasmus Källqvist, based on code written by Sandeep Mistry
* @brief   Main for receiver
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h" 

/* Private variables ---------------------------------------------------------*/
static union two_byte_union package_id; 
static uint16_t num_pkts;
static int8_t rssi_list[EXPECTED_NUM_PACKAGES];
static int8_t snr_list[EXPECTED_NUM_PACKAGES];
static double rssi_mean;
static double rssi_std;
static double snr_mean;
static double snr_std;
static float pdr; // packet delivery rate 

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
	lcd_display_str("RXMODE");
	
	/* Set up variables */
	uint8_t packet_length;
	uint8_t packet_index;
	uint8_t rx_buff[MAX_PKT_LENGTH];

	/* Receive 1000 packages */
	while(num_pkts < EXPECTED_NUM_PACKAGES)
	{	
		/* Receive package if available, else set radio chip in RX mode */
		packet_length = rfm96_receive_package(rx_buff);
		
 		/* Extract payload if package is ready */
  		if(packet_length > 0)
  		{
			/* Read received package */
  			while(rfm96_read_reg(REG_RX_NB_BYTES) - packet_index)
  			{
  				/* Read next received byte from FIFO */
  				package_id.buffer[packet_index] = rfm96_read_reg(REG_FIFO);
  				packet_index++;
  			}

  			/* Increment received package counter */
  			num_pkts++;
			
			/* Read package RSSI and SNR */
			// change this so it adds values to lists instead 
			rssi_list[num_pkts-1] = -137 + rfm96_read_reg(REG_PKT_RSSI_VALUE);
			snr_list[num_pkts-1] = (int8_t)(rfm96_read_reg(REG_PKT_SNR_VALUE) * 0.25);

			// dev test, display package
			lcd_display_int(num_pkts);
		
			/* Reset package index */
			packet_index = 0;
  		}  		
  	}

  	/* Mean and standard deviation for RSSI */
  	rssi_mean = mean(rssi_list, EXPECTED_NUM_PACKAGES);
  	rssi_std = std_dev(rssi_list, EXPECTED_NUM_PACKAGES, rssi_mean);

  	/* Mean and standard deviation for SNR */
	snr_mean = mean(snr_list, EXPECTED_NUM_PACKAGES);
  	snr_std = std_dev(snr_list, EXPECTED_NUM_PACKAGES, rssi_mean);
  	
  	/* Packet delivery rate */
  	pdr = package_id.num / EXPECTED_NUM_PACKAGES;

  	/* Signal results ready to be displayed */
	lcd_display_str_delayed("RXDONE", DISPLAY_DELAY); 

  	/* Show results on display */
  	while(1)
  	{
  		/* Last Package ID*/
		lcd_display_str_delayed("LASTID", DISPLAY_DELAY);
		lcd_display_int(package_id.num); 
  		wait_for_user_button();

  		/* Payload Delivery Rate */
		lcd_display_str_delayed("PDR", DISPLAY_DELAY);
		lcd_display_percentage(pdr); 
  		wait_for_user_button();

  		/* Arithmetic mean for RSSI */
		lcd_display_str_delayed("RSSI", DISPLAY_DELAY);
		lcd_display_str_delayed("MEAN", DISPLAY_DELAY);
		lcd_display_float(rssi_mean); 
  		wait_for_user_button();

  		/* Standard deviation for RSSI */
		lcd_display_str_delayed("RSSI", DISPLAY_DELAY);
		lcd_display_str_delayed("STD", DISPLAY_DELAY);
		lcd_display_float(rssi_std);
  		wait_for_user_button();

  		/* Arithmetic mean SNR */
		lcd_display_str_delayed("SNR", DISPLAY_DELAY);
		lcd_display_str_delayed("MEAN", DISPLAY_DELAY);
		lcd_display_float(snr_mean);
  		wait_for_user_button();

  		/* Standard deviation for SNR */
		lcd_display_str_delayed("SNR", DISPLAY_DELAY);
		lcd_display_str_delayed("STD", DISPLAY_DELAY);
		lcd_display_float(snr_std);
  		wait_for_user_button();
 	}
}



/*
 * brief : small helper function that polls the user button for a press 
 */
void wait_for_user_button(void)
{
	while(BSP_PB_GetState(BUTTON_USER) != 0);
	while(BSP_PB_GetState(BUTTON_USER) != 1);
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
