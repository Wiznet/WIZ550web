/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* by grqd_xp                                                            */
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/
#include <string.h>

#include "stm32f10x.h"
//#include "hw_config.h"

#include "diskio.h"
#include "mmc_sd.h"
#include "dataflash.h"

/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */
/* Note that Tiny-FatFs supports only single drive and always            */
/* accesses drive number 0.                                              */

#define SECTOR_SIZE 512U

int rtc_year = 2014;
int rtc_month = 10;
int rtc_day = 31;
int rtc_hour = 10;
int rtc_min = 10;
int rtc_sec = 30;

//u32 buff2[512/4];
/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
    u8 state;

    if(drv)
    {
        return STA_NOINIT;  // only supports the operation of the disk 0
    }

    state = SD_Init();
    if(state == STA_NODISK)
    {
        return STA_NODISK;
    }
    else if(state != 0)
    {
        return STA_NOINIT;  // other error: initialization failed
    }
    else
    {
        return 0;           // initialization succeeded
    }
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{	
    if(drv)
    {
        return STA_NOINIT;  // only supports disk-0 operation
    }

	// Check whether the inserted SD card
	//  if(!SD_DET())
	//  {
	//     return STA_NODISK;
	//  }
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	u8 res=0;
    if (drv || !count)
    {    
        return RES_PARERR;  // only supports single disk operation, count is not equal to 0, otherwise parameter error
    }
   // if(!SD_DET())
   // {
   //     return RES_NOTRDY;  // does not detect SD card, NOT READY error reported
   // }

    
	
    if(count==1) // sector reads 1
    {                                                
        res = SD_ReadSingleBlock(sector, buff);      
    }                                                
    else // multiple sector read operations
    {                                                
        res = SD_ReadMultiBlock(sector, buff, count);
    }                                                
	/*
    do                           
    {                                          
        if(SD_ReadSingleBlock(sector, buff)!=0)
        {                                      
            res = 1;                           
            break;                             
        }                                      
        buff+=512;                             
    }while(--count);                                         
    */
    // Process the return value, the return value of the SPI_SD_driver.c the return value turned into ff.c
    if(res == 0x00)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	u8 res;

    if (drv || !count)
    {    
        return RES_PARERR;  // only supports single disk operation, count is not equal to 0, otherwise parameter error
    }
	/*
    if(!SD_DET())
    {
        return RES_NOTRDY;  // does not detect SD card, NOT READY error reported
    }  */

    // Read and write operations
    if(count == 1)
    {
        res = SD_WriteSingleBlock(sector, buff);
    }
    else
    {
        res = SD_WriteMultiBlock(sector, buff, count);
    }
    // Return value to
    if(res == 0)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{

    DRESULT res;


    if (drv)
    {    
        return RES_PARERR;  // only supports single disk operation, or return parameter error
    }
    
    // FATFS only deal with the current version of CTRL_SYNC, GET_SECTOR_COUNT, GET_BLOCK_SIZ three commands
    switch(ctrl)
    {
    case CTRL_SYNC:
        MSD_CS_ENABLE();
        if(SD_WaitReady()==0)
        {
            res = RES_OK;
        }
        else
        {
            res = RES_ERROR;
        }
        MSD_CS_DISABLE();
        break;
        
    case GET_BLOCK_SIZE:
        *(WORD*)buff = 512;
        res = RES_OK;
        break;

    case GET_SECTOR_COUNT:
        *(DWORD*)buff = SD_GetCapacity();
        res = RES_OK;
        break;
    default:
        res = RES_PARERR;
        break;
    }

    return res;
}

DWORD get_fattime(void)
{
	DWORD tmr = 0;

	//__disable_irq();
	tmr = (((DWORD)rtc_year - 1980) << 25)
		 | ((DWORD)rtc_month << 21)
		 | ((DWORD)rtc_day << 16)
		 | ((WORD)rtc_hour << 11)
		 | ((WORD)rtc_min << 5)
		 | ((WORD)rtc_sec << 1);
	//__enable_irq();

	return tmr;
}























