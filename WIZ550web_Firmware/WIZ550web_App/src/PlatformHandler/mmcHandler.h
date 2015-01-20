#include "ff.h"
#include "diskio.h"
#include "stm32f10x.h"

/*-------------------------------------------------------------------------------*/
/* Option: Transmit/Receive Block using DMA (Platform dependent. STM32 here)     */
/*-------------------------------------------------------------------------------*/
//#define STM32_SD_USE_DMA

/*
#define RCC_APB2Periph_GPIO_CS   RCC_APB2Periph_GPIOB
#define GPIO_SPI_SD              GPIOB
#define GPIO_Pin_SPI_SD_SCK      GPIO_Pin_13
#define GPIO_Pin_SPI_SD_MISO     GPIO_Pin_14
#define GPIO_Pin_SPI_SD_MOSI     GPIO_Pin_15
#define RCC_APBPeriphClockCmd_SPI_SD  RCC_APB1PeriphClockCmd
#define RCC_APBPeriph_SPI_SD     RCC_APB1Periph_SPI2
*/

#define SPI_SD                    		SPI2
#define GPIO_CS          				GPIOB                       	/* GPIOB */
#define GPIO_Pin_CS           			GPIO_Pin_10                  	/* PB.11 */
#define SPI_BaudRatePrescaler_SPI_SD  	SPI_BaudRatePrescaler_2			/* 72MHz / 2 */

// SD Card Selection; CS control
#define SD_CS_SELECT()        			GPIO_ResetBits(GPIO_CS, GPIO_Pin_CS)    // MMC CS = L
#define SD_CS_DESELECT()      			GPIO_SetBits(GPIO_CS, GPIO_Pin_CS)      // MMC CS = H

#ifdef STM32_SD_USE_DMA
#define DMA_Channel_SPI_SD_RX    DMA1_Channel4
#define DMA_Channel_SPI_SD_TX    DMA1_Channel5
#define DMA_FLAG_SPI_SD_TC_RX    DMA1_FLAG_TC4
#define DMA_FLAG_SPI_SD_TC_TX    DMA1_FLAG_TC5
//#define GPIO_SPI_SD              GPIOB
//#define GPIO_Pin_SPI_SD_SCK      GPIO_Pin_13
//#define GPIO_Pin_SPI_SD_MISO     GPIO_Pin_14
//#define GPIO_Pin_SPI_SD_MOSI     GPIO_Pin_15
//#define RCC_APBPeriphClockCmd_SPI_SD  RCC_APB1PeriphClockCmd
//#define RCC_APBPeriph_SPI_SD     RCC_APB1Periph_SPI2
#endif


#define GO_IDLE_STATE			0
#define SEND_OP_COND			1
#define SEND_IF_COND			8
#define SET_BLOCKLEN			16
#define READ_BLOCK				17
#define WRITE_BLOCK				24
#define APP_CMD					55
#define READ_OCR				58
#define SD_SEND_OP_COND			(0x40|41)


typedef enum card_type_id {
	NO_CARD,
	CARD_MMC,
	CARD_SD,
	CARD_SD2,
	CARD_SDHC,
	SPI_FLASHM
} card_type_id_t;



uint8_t mmc_mount();
uint8_t flash_mount();
FRESULT getMountedMemorySize(uint8_t mount_ret, uint32_t * totalSize, uint32_t * availableSize);
void Delay_ms(uint32_t ms);
uint8_t mmc_select();
