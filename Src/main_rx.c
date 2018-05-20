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
static int8_t rssi_list[MAX_EXPECTED_NUM_PACKAGES];
static int8_t snr_list[MAX_EXPECTED_NUM_PACKAGES];
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

	/* User selects number of expected packages*/
	uint32_t ticks_held = 0;
	uint32_t expected_pkts = 10;
	lcd_display_str_delayed("CHOOSE", 500);
	lcd_display_str_delayed("NUM", 500); 
	lcd_display_str_delayed("PKTS", 600); 
	while(ticks_held < BUTTON_HELD_LONG)
	{
		/* Display current mode */
		lcd_display_int(expected_pkts);
		/* Poll button */
		ticks_held = wait_for_user_button_timed();
		/* If short press, update mode */
		if(ticks_held < BUTTON_HELD_LONG)
		{
			expected_pkts *= 10; // modes are 10, 100, 1000
			if(expected_pkts > 1000) // mode after 1000 is 10 
				expected_pkts = 10; 
		}
	}
	lcd_display_str_delayed("YOU", 250);
	lcd_display_str_delayed("CHOSE", 250);
	lcd_display_int_delayed(expected_pkts, 500);

	/* Signal start of receive mode, waiting for first packet */
	lcd_display_str("RXWAIT");
	
	/* Set up variables */
	uint8_t packet_length;
	uint8_t packet_index;
	uint8_t rx_buff[MAX_PKT_LENGTH];

	/* Receive packages */
	while(num_pkts < expected_pkts)
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
			rssi_list[num_pkts-1] = -137 + rfm96_read_reg(REG_PKT_RSSI_VALUE);
			snr_list[num_pkts-1] = (int8_t)(rfm96_read_reg(REG_PKT_SNR_VALUE) * 0.25);

			/* Display current number of received packages */
			lcd_display_int(num_pkts);
		
			/* Reset package index */
			packet_index = 0;
  		}  		
  	}

  	/* Mean and standard deviation for RSSI */
  	rssi_mean = mean(rssi_list, expected_pkts);
  	rssi_std = std_dev(rssi_list, expected_pkts, rssi_mean);

  	/* Mean and standard deviation for SNR */
	snr_mean = mean(snr_list, expected_pkts);
  	snr_std = std_dev(snr_list, expected_pkts, rssi_mean);
  	
  	/* Packet delivery rate */
  	pdr = (double)expected_pkts / (double)package_id.num;

  	/* Signal results ready to be displayed */
  	BSP_LED_On(LED_GREEN);
	lcd_display_str_delayed("RXDONE", DISPLAY_DELAY); 
	wait_for_user_button();

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
