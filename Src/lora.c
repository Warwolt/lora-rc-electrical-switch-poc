/*
********************************************************************************
* @file    lora.c
* @author  Sandeep Mistry, ported to stm32 by Rasmus Källqvist
* @version V1.0.0
* @date    08-May-2018
* @brief   Driver code for interfacing with rfm96w LoRa-radio chip
********************************************************************************
*/

#include "LoRa.h"

/* Function definitions ------------------------------------------------------*/
/* 
 * brief : initializes the RFM96 radio chip 
 */
void rfm96_init(void)
{
	/* Set NSS, disable radio chip  */
	HAL_GPIO_WritePin(RFM96_NSS_PORT, RFM96_NSS_PIN, GPIO_PIN_SET);

	/* Perform reset */
	HAL_GPIO_WritePin(RFM96_RESET_PORT, RFM96_RESET_PIN, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(RFM96_RESET_PORT, RFM96_RESET_PIN, GPIO_PIN_SET);
	HAL_Delay(10);

	/* Put radio chip in sleep mode */
	rfm96_sleep_mode();

	/* Calculate frequency register value given 32 MHz radio chip oscillator */
	uint64_t frf = ((uint64_t)RFM96_FREQUENCY << 19) / 32000000;
	
	/* Set frequency */
	rfm96_write_reg(REG_FRF_MSB, (uint8_t)(frf >> 16));
	rfm96_write_reg(REG_FRF_MID, (uint8_t)(frf >> 8));
	rfm96_write_reg(REG_FRF_LSB, (uint8_t)(frf >> 0));

	/* Set FIFO pointer base addresses */
	rfm96_write_reg(REG_FIFO_TX_BASE_ADDR, 0);
  	rfm96_write_reg(REG_FIFO_RX_BASE_ADDR, 0);
	
	/* Set Low Noise Amplifier boost */
	rfm96_write_reg(REG_LNA, rfm96_read_reg(REG_LNA) | 0x03);
	
	/* Turn on automatic gain control */
	rfm96_write_reg(REG_MODEM_CONFIG_3, 0x04);
	
	/* Set output power to 10 dBm */
	rfm96_write_reg(REG_PA_CONFIG, PA_BOOST | (RFM96_TX_POWER - 2));

	/* Set code rate and bandwidth */
	uint8_t config_1_val = rfm96_read_reg(REG_MODEM_CONFIG_1);
	config_1_val = (config_1_val & 0x0F) | (0x7 << 4); //0x7 = bandwidth 125 kHz
	config_1_val = (config_1_val & 0xF1) | (0x4 << 1); //0x4 = code rate 4/8
	rfm96_write_reg(REG_MODEM_CONFIG_1, config_1_val);

	/* Set spread factor */
	uint8_t config_2_val = rfm96_read_reg(REG_MODEM_CONFIG_2);
	config_2_val = (config_2_val & 0x0F) | (0xA << 4); //0xA = spread factor 12
	rfm96_write_reg(REG_MODEM_CONFIG_2, config_2_val);

	// debug
	static volatile uint8_t debug_val = 0x00;
	debug_val = rfm96_read_reg(REG_MODEM_CONFIG_2);

	/* Put in standby mode */
	rfm96_standby_mode();

	return;
}
/* SPI communication functions -----------------------------------------------*/
/*
 * brief     : wrapper function for GPIO_WritePin when enabling rfm96 chip spi
 */
void rfm96_spi_enable(void)
{
	HAL_GPIO_WritePin(SPI_NSS_GPIO_Port, SPI_NSS_Pin, GPIO_PIN_RESET);
}

/*
 * brief     : wrapper function for GPIO_WritePin when disabling rfm96 chip spi
 */
void rfm96_spi_disable(void)
{
	HAL_GPIO_WritePin(SPI_NSS_GPIO_Port, SPI_NSS_Pin, GPIO_PIN_SET);
}

/* 
 * brief   : wrapper for SPI write access of one byte to radio chip 
 * address : rfm96 register to write to
 * value   : byte value to write to register 
 */
void rfm96_write_reg(uint8_t address, uint8_t value)
{
	rfm96_single_transfer(address | WNR_WRITE_ACCESS, value);
}

/* 
 * brief   : wrapper for SPI read access of one byte to radio chip
 * address : rfm96 register to read from 
 * retval  : the read value  
 */
uint8_t rfm96_read_reg(uint8_t address)
{
  return rfm96_single_transfer(address & WNR_READ_ACCESS, 0x00);
}

/* 
 * brief   : transfers a single data byte over SPI to rfm96 radio chip 
 * address : rfm96 register to read from or write to
 * retval  : the read value, if applicable 
 */ 
uint8_t rfm96_single_transfer(uint8_t address, uint8_t value)
{
	uint8_t response;

	/* Enable radio chip */
  	rfm96_spi_enable();

  	/* Transfer address byte, then exchange data in duplex mode */
  	spi_transmit(&address, 1);
  	spi_transmit_receive(&value, &response, 1);

	/* Disable radio chip */
  	rfm96_spi_disable();
	
	return response;
}

/* RFM96 mode selection functions --------------------------------------------*/
/*
 *  brief : wrapper function, sets radio chip in standby mode
 */
void rfm96_standby_mode(void)
{
	rfm96_write_reg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
}

/*
 *  brief : wrapper function, sets radio chip in sleep mode
 */
void rfm96_sleep_mode(void)
{
	rfm96_write_reg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
}

/*
 *  brief : wrapper function, 
 *          turns on continous transmission for spectral analysis 
 */
void rfm96_continous_tx(void)
{
	rfm96_write_reg(REG_MODEM_CONFIG_2, rfm96_read_reg(REG_MODEM_CONFIG_2)|0x8);
}
/* Packet transmission functions ---------------------------------------------*/ 
/*
 *  brief : begins a packet to send by resetting the radio chip FIFO buffer 
 */
void rfm96_begin_packet(void)
{
	/* Set radio chip to standby mode*/
	rfm96_standby_mode();

	/* reset FIFO address and payload length */
	rfm96_write_reg(REG_FIFO_ADDR_PTR,  0);
	rfm96_write_reg(REG_PAYLOAD_LENGTH, 0);
} 

/*
 *  brief : fills packet with data by writing to FIFO buffer 
 */
size_t rfm96_write_packet(const uint8_t *payload, size_t size)
{
	/* Get current payload size */
	int current_length = rfm96_read_reg(REG_PAYLOAD_LENGTH);
	
	/* Check if new data would overflow FIFO buffer */
	if ((current_length + size) > MAX_PKT_LENGTH) 
	{
		size = MAX_PKT_LENGTH - current_length;
	}
	
	/* Write payload data to FIFO buffer */
	for (size_t i = 0; i < size; i++) 
	{
		rfm96_write_reg(REG_FIFO, payload[i]);
	}
	
	/* Update playload length register in radio chip */
	rfm96_write_reg(REG_PAYLOAD_LENGTH, current_length + size);
	
	return size;
}

/*
 *  brief : sends packet by setting radio chip in transmission mode 
 */
void rfm96_send_packet(void)
{
	/* Put radio chip in transmission mode */
	rfm96_write_reg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);
	
	/* Wait for tranmission to complete */
	while ((rfm96_read_reg(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) == 0) 
	{
	}
	
	/* Clear interrupt request flags */
	rfm96_write_reg(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
}