// system util.c

#include "system_util.h"

void system_init(void)
{
	/* Low level mcu init */
	HAL_Init();

	/* Configure the system clock to 32 MHz */
	SystemClock_Config();
	
	/* SPI, NSS-pin and reset-pin initialization */
	spi_init();
	
	/* LCD display initialization */
	BSP_LCD_GLASS_Init();

	/* Blue push button initialization */
	BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

	/* Initialize LEDs */
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
}

/**
	* @brief  System Clock Configuration
	*         The system Clock is configured as follow :
	*            System Clock source            = PLL (HSI)
	*            SYSCLK(Hz)                     = 32000000
	*            HCLK(Hz)                       = 32000000
	*            AHB Prescaler                  = 1
	*            APB1 Prescaler                 = 1
	*            APB2 Prescaler                 = 1
	*            HSI Frequency(Hz)              = 16000000
	*            PLLMUL                         = 6
	*            PLLDIV                         = 3
	*            Flash Latency(WS)              = 1
	* @retval None
	*/
void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};

	/* Enable HSE Oscillator and Activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLL_MUL6;
	RCC_OscInitStruct.PLL.PLLDIV          = RCC_PLL_DIV3;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		/* Initialization Error */
		while(1);
	}

	/* Set Voltage scale1 as MCU will run at 32MHz */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	
	/* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
	while (__HAL_PWR_GET_FLAG(PWR_FLAG_VOS) != RESET) {};

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		/* Initialization Error */
		while(1); 
	}
}

/**
	* @brief  This function is executed in case of error occurrence.
	* @param  None
	* @retval None
	*/
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler */
	/* User can add his own implementation to report the HAL error return state */
	while(1) 
	{
	}
	/* USER CODE END Error_Handler */ 
}

/* Statistics ----------------------------------------------------------------*/
double mean(int8_t a[], uint8_t n)
{
	double sum = 0.0;
	for(int i = 0; i < n; i++)
	{
		sum += (double)a[i] / (double)n;
	}
	return sum;
}

/* Calculates sample variance */
double variance(int8_t a[], uint8_t n, double mean) 
{
	double sum = 0.0;
	double diff = 0.0;
	for(int i = 0; i < n; i++)
	{
		diff = (double)a[i] - mean;
		sum += (diff * diff);
	}
	sum /= (double)(n - 1);
	return sum;
}

/* Calculates sample standard deviation */
double std_dev(int8_t a[], uint8_t n, double mean) 
{
	return sqrt(variance(a, n, mean));
}

/* Button pushing ------------------------------------------------------------*/

/*
 * brief : small helper function that polls the user button for a press 
 */
void wait_for_user_button(void)
{
	while(BSP_PB_GetState(BUTTON_USER) != 0);
	while(BSP_PB_GetState(BUTTON_USER) != 1);
}

/*
 * brief  : small helper function that polls the user button for a press 
 * retval : time in sysclock ticks that the button was held (should be ms) 
 */
uint32_t wait_for_user_button_timed(void)
{
	uint32_t tickstart;

	while(BSP_PB_GetState(BUTTON_USER) != 0); // make sure button wasn't held 
	while(BSP_PB_GetState(BUTTON_USER) != 1); // wait for button push
	tickstart = HAL_GetTick(); // check how long button is held 
	while(BSP_PB_GetState(BUTTON_USER) != 0); // wait for button release 

	return (HAL_GetTick() - tickstart); // return how long button was held 
}