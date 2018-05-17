/* defines.h */

/* Definition for SPIx clock resources */
#define SPIx                             SPI3
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI3_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI3_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI3_RELEASE_RESET()

/* Definition for RFM96 Pins */
#define RFM96_SCK_PIN                     GPIO_PIN_5
#define RFM96_SCK_GPIO_PORT               GPIOA
#define RFM96_SCK_AF                      GPIO_AF5_SPI1
#define RFM96_MISO_PIN                    GPIO_PIN_11
#define RFM96_MISO_GPIO_PORT              GPIOA
#define RFM96_MISO_AF                     GPIO_AF5_SPI1
#define RFM96_MOSI_PIN                    GPIO_PIN_12
#define RFM96_MOSI_GPIO_PORT              GPIOA
#define RFM96_MOSI_AF                     GPIO_AF5_SPI1
#define RFM96_NSS_PORT                    GPIOB
#define RFM96_NSS_PIN                     GPIO_PIN_2
#define RFM96_RESET_PORT                  GPIOA
#define RFM96_RESET_PIN                   GPIO_PIN_4

/* Size of buffer */
#define BUFFERSIZE                       (COUNTOF(aTxBuffer) - 1)

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))