#ifndef __DATAFLASH_INCLUDED
#define __DATAFLASH_INCLUDED

#include "wizchip_conf.h"
#include "boardutil.h"

#define DF_CS_LOW() 				GPIO_ResetBits(DF_CS_PORT, DF_CS_PIN);	// SSEL(CS)
#define DF_CS_HIGH() 				GPIO_SetBits(DF_CS_PORT, DF_CS_PIN);	// SSEL(CS)


//Dataflash opcodes
#define FlashPageRead				0xD2	// Main memory page read
#define FlashToBuf1Transfer 		0x53	// Main memory page to buffer 1 transfer
#define Buf1Read					0xD4	// Buffer 1 read
#define FlashToBuf2Transfer 		0x55	// Main memory page to buffer 2 transfer
#define Buf2Read					0xD6	// Buffer 2 read
#define StatusReg					0xD7	// Status register
#define AutoPageReWrBuf1			0x58	// Auto page rewrite through buffer 1
#define AutoPageReWrBuf2			0x59	// Auto page rewrite through buffer 2
#define FlashToBuf1Compare    		0x60	// Main memory page to buffer 1 compare
#define FlashToBuf2Compare	    	0x61	// Main memory page to buffer 2 compare
#define ContArrayRead				0x0B	// Continuous Array Read (Note : Only A/B-parts supported)
#define FlashProgBuf1				0x82	// Main memory page program through buffer 1
#define Buf1ToFlashWE   			0x83	// Buffer 1 to main memory page program with built-in erase
#define Buf1Write					0x84	// Buffer 1 write
#define FlashProgBuf2				0x85	// Main memory page program through buffer 2
#define Buf2ToFlashWE   			0x86	// Buffer 2 to main memory page program with built-in erase
#define Buf2Write					0x87	// Buffer 2 write
#define Buf1ToFlash     			0x88	// Buffer 1 to main memory page program without built-in erase
#define Buf2ToFlash		         	0x89	// Buffer 2 to main memory page program without built-in erase

/**
  * @brief External Flash memory SPI Interface pins
  */
#define DF_SPI                       SPI2
#define DF_SPI_CLK                   RCC_APB2Periph_SPI2

#define DF_CS_PIN           		GPIO_Pin_11                  	/* PB.11 */
#define DF_CS_PORT          		GPIOB                       	/* GPIOB */
#define DF_CS_GPIO_CLK     			RCC_APB2Periph_GPIOB

//#define DF_DUMMY_BYTE		0xFF
//#define DF_RESET_PIN                 GPIO_Pin_9   // PB.9 China board

//#define PAGE_PER_BYTE				256
#define PAGE_PER_BYTE				264


void read_from_flashbuf(unsigned long Address, unsigned char* Buffer, unsigned int Size);
void write_to_flashbuf(unsigned long Address, unsigned char* Buffer, unsigned int Size);

// ## Duplicate functions for dataflash access
void DataFlash_Init(void);
void DataFlash_ReInit(void);
void write_to_flash(unsigned char flash_data);
void write_end_page(void);
unsigned char read_from_flash(unsigned long Address);

//Dataflash Access Function definitions
void DF_Init(void);
unsigned char DF_BufferReadByte(unsigned char BufferNo, unsigned int IntPageAdr);
void DF_BufferWriteByte(unsigned char BufferNo, unsigned int IntPageAdr, unsigned char Data);
void DF_PageToBuffer(unsigned char BufferNo, unsigned int PageAdr);
void DF_BufferToPage (unsigned char BufferNo, unsigned int PageAdr);
void DF_Chip_Erase(void);

void DF_BufferReadStr (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr);
void DF_BufferWriteStr (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr);
void DF_BufferWriteEnable (unsigned char BufferNo, unsigned int IntPageAdr);

unsigned char df_read_status (void);
void Cont_Flash_Read_Enable (unsigned int PageAdr, unsigned int IntPageAdr);

#if defined(F_SPI_FLASH)
void Flash_WaitReady(void);
#endif

// *****************************[ End Of DATAFLASH.H ]*****************************
#endif
