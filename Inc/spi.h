/*
********************************************************************************
* File Name          : SPI.h
* Description        : This file provides code for the configuration
*                      of the SPI instances.
********************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __spi_H
#define __spi_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"
#include "system_util.h"
#include "defines.h"
#include "gpio.h"
#include "lora.h"

/* Defines -------------------------------------------------------------------*/
#define SPI_WAIT_TIME 5000

/* External variables --------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;

/* Function declarations -----------------------------------------------------*/
void spi_init(void);
void spi_transmit(uint8_t* tx_data, size_t num_bytes);
void spi_transmit_receive(uint8_t* tx_data, uint8_t* rx_data, size_t num_bytes);

#ifdef __cplusplus
}
#endif
#endif /*__ spi_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
