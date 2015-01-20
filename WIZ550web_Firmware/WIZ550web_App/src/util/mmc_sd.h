#ifndef _MMC_SD_H_
#define _MMC_SD_H_
//#include <stm32f10x_lib.h>


#define MMC_SD_SPI2_PORT		GPIOB

#define MMC_SD_SCK_PORT			MMC_SD_SPI2_PORT
#define MMC_SD_SCK_PIN			GPIO_Pin_13

#define MMC_SD_MISO_PORT		MMC_SD_SPI2_PORT
#define MMC_SD_MISO_PIN			GPIO_Pin_14

#define MMC_SD_MOSI_PORT		MMC_SD_SPI2_PORT
#define MMC_SD_MOSI_PIN			GPIO_Pin_15


#define MMC_SD_CS_PORT			GPIOB
#define MMC_SD_CS_PIN			GPIO_Pin_10



// SD data transmission whether to release the bus after the end of the macro definition
#define NO_RELEASE 0
#define RELEASE 1

// SD Card Type Definition
#define SD_TYPE_MMC 1
#define SD_TYPE_V1 2
#define SD_TYPE_V2 3
#define SD_TYPE_V2HC 4

// SD Card instruction sheet
#define CMD0 0 // Card Reset
#define CMD1 1
#define CMD9 9 // command 9, CSD data read
#define CMD10 10 // Command 10, read CID data
#define CMD12 12 // command 12, to stop data transmission
#define CMD16 16 // Command 16, set SectorSize should return 0x00
#define CMD17 17 // Command 17, read sector
#define CMD18 18 // Command 18, read Multi sector
#define ACMD23 23 // Command 23, set the multi-sector erase writing in advance of a block N
#define CMD24 24 // Command 24, write sector
#define CMD25 25 // Command 25, write Multi sector
#define ACMD41 41 // command to 41, should return 0x00
#define CMD55 55 // command to 55, should return 0x01
#define CMD58 58 // Command 58, read OCR information
#define CMD59 59 // command to 59, enables / disables the CRC, should return 0x00

// Write data to respond to the word meaning
#define MSD_DATA_OK 0x05
#define MSD_DATA_CRC_ERROR 0x0B
#define MSD_DATA_WRITE_ERROR 0x0D
#define MSD_DATA_OTHER_ERROR 0xFF

// SD card labeled word response
#define MSD_RESPONSE_NO_ERROR 0x00
#define MSD_IN_IDLE_STATE 0x01
#define MSD_ERASE_RESET 0x02
#define MSD_ILLEGAL_COMMAND 0x04
#define MSD_COM_CRC_ERROR 0x08
#define MSD_ERASE_SEQUENCE_ERROR 0x10
#define MSD_ADDRESS_ERROR 0x20
#define MSD_PARAMETER_ERROR 0x40
#define MSD_RESPONSE_FAILURE 0xFF
 
// This part should be modified depending on the connection!
// Mini STM32 uses SD cards as CS PA3 feet.
// #define SD_CS PAout (3) // SD card selection pin
/* Select MSD Card: ChipSelect pin low  */ 
#define MSD_CS_ENABLE()     GPIO_ResetBits(MMC_SD_CS_PORT, MMC_SD_CS_PIN)  

/* Deselect MSD Card: ChipSelect pin high */  
#define MSD_CS_DISABLE()     GPIO_SetBits(MMC_SD_CS_PORT, MMC_SD_CS_PIN)  

extern u8 SD_Type; // SD card type


	
// Function state area
void bsp_set_spi1_speed_mmcsd(u16 prescaler);
u8 SD_WaitReady (void); // SD card ready to wait
u8 SD_SendCommand (u8 cmd, u32 arg, u8 crc); // SD card to send a command
u8 SD_SendCommand_NoDeassert (u8 cmd, u32 arg, u8 crc);
u8 SD_Init (void); // SD Card initialization
u8 SD_Idle_Sta (void); // set the SD card into suspend mode

u8 SD_ReceiveData (u8 * data, u16 len, u8 release); // SD card reader data
u8 SD_GetCID (u8 * cid_data); // reading SD card CID
u8 SD_GetCSD (u8 * csd_data); // reading SD card CSD
u32 SD_GetCapacity (void); // check SD card capacity

// USB SD card reader operation function
u8 MSD_WriteBuffer (u8 * pBuffer, u32 WriteAddr, u32 NumByteToWrite);
u8 MSD_ReadBuffer (u8 * pBuffer, u32 ReadAddr, u32 NumByteToRead);

u8 SD_ReadSingleBlock (u32 sector, u8 * buffer); // read a sector
u8 SD_WriteSingleBlock (u32 sector, const u8 * buffer); // write a sector
u8 SD_ReadMultiBlock (u32 sector, u8 * buffer, u8 count); // read multiple sector
u8 SD_WriteMultiBlock (u32 sector, const u8 * data, u8 count); // write multiple sector
u8 SD_Read_Bytes (unsigned long address, unsigned char * buf, unsigned int offset, unsigned int bytes); // read a byte
u8 SD_WaitReady(void);

extern void bsp_sd_gpio_init(void);
extern int g_mkfs_done;
extern int g_sdcard_done;

#endif

