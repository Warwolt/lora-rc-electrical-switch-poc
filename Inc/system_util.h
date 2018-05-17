/* system util.h */

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"
#include "stm32l152c_discovery.h"
#include "stm32l152c_discovery_glass_lcd.h"
#include "spi.h"
#include "math.h"

/* Function declarations -----------------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
void system_init(void);
double mean(int8_t a[], uint8_t n);
double std_dev(int8_t a[], uint8_t n, double mean);