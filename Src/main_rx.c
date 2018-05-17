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
//static uint8_t rx_buff[MAX_PKT_LENGTH];
static int8_t pkt_rssi;
static int8_t pkt_snr;

// dev test
// rember to move this definition somewhere else, Don't Repeat Yourself!
union buffer_union
{
	uint8_t buffer[2];
	uint16_t num;
};

static union buffer_union received_packages; 

/* Function declarations -----------------------------------------------------*/
/*
 *  brief   : receives package from RFM96 if available, else turns on RX mode
 *  rx_buff : pointer to buffer to receive data, caller is responsible for 
 *            allocaing enough data for the buffer! Max payload is 255 bytes
 *  retval  : length of received package in byte, 0 if no package arrived
 */
uint8_t rfm96_receive_package(uint8_t* rx_buff)
{
	/* Setup variables  */
	uint8_t packet_length = 0;
	uint8_t irq_flags = rfm96_read_reg(REG_IRQ_FLAGS);

	/* Clear IRQ's */
	rfm96_write_reg(REG_IRQ_FLAGS, irq_flags);

	/* Check if a package has arrived */
	if((irq_flags & IRQ_RX_DONE_MASK) && (irq_flags & IRQ_PAYLOAD_CRC_ERROR_MASK) == 0)
	{
		/* Read payload length from package header */
		packet_length = rfm96_read_reg(REG_RX_NB_BYTES);

		/* Set FIFO buffer pointer to current RX address */
	rfm96_write_reg(REG_FIFO_ADDR_PTR, rfm96_read_reg(REG_FIFO_RX_CURRENT_ADDR));

	/* Rx done, return radio chip to standby mode */
	rfm96_standby_mode();
	}
	/* If not already in receive mode, switch to it if no package's arrived */ 
	else if(rfm96_read_reg(REG_OP_MODE) != (MODE_LONG_RANGE_MODE | MODE_RX_SINGLE))
	{
		/* Reset the FIFO buffer address pointer */
		rfm96_write_reg(REG_FIFO_ADDR_PTR, 0);

		/* Set radio chip to single receive mode */
		rfm96_write_reg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_SINGLE);
	}	

	return packet_length;
}


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
	
	/* Infinite loop */
	uint8_t packet_length;
	uint8_t packet_index;
	uint8_t rx_buff[MAX_PKT_LENGTH];
	while(1)
	{	
		packet_index = 0;

		// TODO: put this into its own function later when this code's working
		/* Single Rx Function ------------------------------------------------*/
		// /* Setup variables  */
  // 		uint8_t packet_length = 0;
  // 		uint8_t irq_flags = rfm96_read_reg(REG_IRQ_FLAGS);

  // 		  		/* Clear IRQ's */
  // 		rfm96_write_reg(REG_IRQ_FLAGS, irq_flags);

  // 		/* Check if a package has arrived */
  // 		if((irq_flags & IRQ_RX_DONE_MASK) && (irq_flags & IRQ_PAYLOAD_CRC_ERROR_MASK) == 0)
  // 		{
  // 			/* Read payload length from package header */
  // 			packet_length = rfm96_read_reg(REG_RX_NB_BYTES);

  // 			/* Set FIFO buffer pointer to current RX address */
  //   		rfm96_write_reg(REG_FIFO_ADDR_PTR, rfm96_read_reg(REG_FIFO_RX_CURRENT_ADDR));

  //   		/* Rx done, return radio chip to standby mode */
  //   		rfm96_standby_mode();
  // 		}
  // 		/* If not already in receive mode, switch to it if no package's arrived */ 
  // 		else if(rfm96_read_reg(REG_OP_MODE) != (MODE_LONG_RANGE_MODE | MODE_RX_SINGLE))
  // 		{
  // 			/* Reset the FIFO buffer address pointer */
  // 			rfm96_write_reg(REG_FIFO_ADDR_PTR, 0);

  // 			/* Set radio chip to single receive mode */
  // 			rfm96_write_reg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_SINGLE);
  // 		}

		packet_length = rfm96_receive_package(rx_buff);
		
  		/* Main while loop ---------------------------------------------------*/
  		/* Extract payload if package is ready */
  		if(packet_length > 0)
  		{
			/* Read received package */
  			while(rfm96_read_reg(REG_RX_NB_BYTES) - packet_index)
  			{
  				/* Read next received byte from FIFO */
  				received_packages.buffer[packet_index] = rfm96_read_reg(REG_FIFO);
  				packet_index++;
  			}
			
			/* Read package RSSI and SNR */
			pkt_rssi = -137 + rfm96_read_reg(REG_PKT_RSSI_VALUE);
			pkt_snr = (int8_t)(rfm96_read_reg(REG_PKT_SNR_VALUE) * 0.25);

			// dev test, display package
			lcd_display_int(received_packages.num);
		
			/* Wait a bit, then continue */
			// HAL_Delay(10);
  		}  		
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
