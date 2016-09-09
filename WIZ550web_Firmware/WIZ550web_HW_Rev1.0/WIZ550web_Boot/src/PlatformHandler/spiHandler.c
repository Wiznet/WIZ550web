
#include "stm32f10x.h"
#include "spiHandler.h"
#include "uartHandler.h"
#include "wizchip_conf.h"

void Delay(volatile unsigned int nCount)
{
	for(; nCount!= 0;nCount--);
}

static uint8_t wizchip_rw(uint8_t byte)
{
	/*!< Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(W5500_SPI, SPI_I2S_FLAG_TXE) == RESET);

	/*!< Send byte through the SPI2 peripheral */
	SPI_I2S_SendData(W5500_SPI, byte);

	/*!< Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(W5500_SPI, SPI_I2S_FLAG_RXNE) == RESET);

	/*!< Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(W5500_SPI);
}

static void  wizchip_select(void)
{
	GPIO_ResetBits(W5500_CS_GPIO_PORT, W5500_CS_PIN);
}

static void  wizchip_deselect(void)
{
	GPIO_SetBits(W5500_CS_GPIO_PORT, W5500_CS_PIN);
}

static void  wizchip_write(uint8_t wb)
{
	wizchip_rw(wb);
}

static uint8_t wizchip_read(void)
{
	return wizchip_rw(0xFF);
}

void W5500_SPI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	/*!< Configure W5500_SPI pins: SCK */
	GPIO_InitStructure.GPIO_Pin = W5500_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(W5500_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure W5500_SPI pins: MOSI */
	GPIO_InitStructure.GPIO_Pin = W5500_SPI_MOSI_PIN;
	GPIO_Init(W5500_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure W5500_SPI pins: MISO */
	GPIO_InitStructure.GPIO_Pin = W5500_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(W5500_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure W5500_CS_PIN pin: W5500 CS pin */
	GPIO_InitStructure.GPIO_Pin = W5500_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(W5500_CS_GPIO_PORT, &GPIO_InitStructure);

	/*!< SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(W5500_SPI, &SPI_InitStructure);

	/*!< Enable the W5500_SPI  */
	SPI_Cmd(W5500_SPI, ENABLE);
}

void W5500_Init(void)
{
	uint8_t memsize[2][8] = { {2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};

	wizchip_deselect();

	GPIO_ResetBits(GPIOB, W5500_RESET_PIN);
	Delay(1000); // Is this enough?
	GPIO_SetBits(GPIOB, W5500_RESET_PIN);

	// Wiznet
	reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
	reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);

	/* wizchip initialize*/
	if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1) {
		myprintf("WIZCHIP Initialized fail.\r\n");
		return;
	}

	Delay(10000);
}

#if 0
/* Temp */
static void display_Net_Info()
{
	wiz_NetInfo gWIZNETINFO;

	ctlnetwork(CN_GET_NETINFO, (void*) &gWIZNETINFO);
	myprintf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", gWIZNETINFO.mac[0], gWIZNETINFO.mac[1], gWIZNETINFO.mac[2], gWIZNETINFO.mac[3], gWIZNETINFO.mac[4], gWIZNETINFO.mac[5]);
	myprintf("IP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);
	myprintf("GW: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0], gWIZNETINFO.gw[1], gWIZNETINFO.gw[2], gWIZNETINFO.gw[3]);
	myprintf("SN: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0], gWIZNETINFO.sn[1], gWIZNETINFO.sn[2], gWIZNETINFO.sn[3]);
	myprintf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0], gWIZNETINFO.dns[1], gWIZNETINFO.dns[2], gWIZNETINFO.dns[3]);
}

void Net_Conf()
{
	wiz_NetInfo gWIZNETINFO = {
		{ 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 },				// Mac address
		{ 10, 0, 1, 100 },									// IP address
		{ 10, 0, 1, 1},										// Gateway
		{ 255, 255, 255, 0},								// Subnet mask
		{ 8, 8, 8, 0},										// DNS Server
		NETINFO_STATIC
	};

	ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);

	display_Net_Info();
}
#endif
