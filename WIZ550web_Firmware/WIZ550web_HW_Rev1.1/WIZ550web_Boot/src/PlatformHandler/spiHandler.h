
#ifndef __SPIHANDLER_H__
#define __SPIHANDLER_H__

#define W5500_SPI                       SPI2
#define W5500_SPI_CLK                   RCC_APB1Periph_SPI2

#define W5500_SPI_SCK_PIN               GPIO_Pin_13                  /* PB.13 */
#define W5500_SPI_SCK_GPIO_PORT         GPIOB                       /* GPIOB */
#define W5500_SPI_SCK_GPIO_CLK          RCC_APB2Periph_GPIOB

#define W5500_SPI_MISO_PIN              GPIO_Pin_14                  /* PB.14 */
#define W5500_SPI_MISO_GPIO_PORT        GPIOB                       /* GPIOB */
#define W5500_SPI_MISO_GPIO_CLK         RCC_APB2Periph_GPIOB

#define W5500_SPI_MOSI_PIN              GPIO_Pin_15                  /* PB.15 */
#define W5500_SPI_MOSI_GPIO_PORT        GPIOB                       /* GPIOB */
#define W5500_SPI_MOSI_GPIO_CLK         RCC_APB2Periph_GPIOB

#define W5500_CS_PIN                    GPIO_Pin_12                  /* PB.12 */
#define W5500_CS_GPIO_PORT              GPIOB                       /* GPIOB */
#define W5500_CS_GPIO_CLK               RCC_APB2Periph_GPIOB

#define W5500_DUMMY_BYTE				0xFF
#define W5500_RESET_PIN                 GPIO_Pin_9 

void W5500_SPI_Init(void);
void W5500_Init(void);
//void Net_Conf();

#endif
