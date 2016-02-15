

#ifndef  SPI_PRESENT
#define  SPI_PRESENT



/* ------------------------------------------------------------------------------------------------- */
/* BSP SPI */
/* ------------------------------------------------------------------------------------------------- */

#define SPI_SPEED_2   0
#define SPI_SPEED_4   1
#define SPI_SPEED_8   2
#define SPI_SPEED_16  3
#define SPI_SPEED_256 4

// for vs1003b
#define SPI_SPEED_LOW   0
#define SPI_SPEED_HIGH  1


/* ------------------------------------------------------------------------------------------------- */
/* function SPI */
/* ------------------------------------------------------------------------------------------------- */




/* ------------------------------------------------------------------------------------------------- */
/* extern SPI */
/* ------------------------------------------------------------------------------------------------- */

extern void bsp_set_spi1_speed (u8 speed);
extern u8 bsp_readwritebyte_spi1 (u8 tx_data);

extern void bsp_set_spi2_speed (u8 speed);
extern void bsp_set_spi2_speed_mp3(u8 SpeedSet);
extern u8 bsp_readwritebyte_spi2 (u8 tx_data);
	
#endif                                                          

/* End of module include.                               */

