/*
 * 006spi_tx_testing.c
 *
 *  Created on: Mar 26, 2024
 *      Author: butle
 */

#include "stm32f407vg.h"
#include <string.h>

//Valid GPIO pin alternate function mappings to SPI2 peripheral:
//Alt function mode: 5
	//PB15 - SPI2 MOSI
	//PB10 - SPI2 SCLK

int main(void)
{
	//configure GPIO pins for needed SPI lines
	GPIO_Handle_t gpioSpiMosi;

	gpioSpiMosi.pGPIOx = GPIOB;
	gpioSpiMosi.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	gpioSpiMosi.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	gpioSpiMosi.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_MODE_AF5;
	gpioSpiMosi.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	gpioSpiMosi.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	gpioSpiMosi.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;

	GPIO_Init(&gpioSpiMosi);

	GPIO_Handle_t gpioSpiSclk;

	gpioSpiSclk.pGPIOx = GPIOB;
	gpioSpiSclk.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_10;
	gpioSpiSclk.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	gpioSpiSclk.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_MODE_AF5;
	gpioSpiSclk.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	gpioSpiSclk.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	gpioSpiSclk.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;

	GPIO_Init(&gpioSpiSclk);





	SPI_Handle_t spi2;

	spi2.pSPIx = SPI2;
	spi2.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	spi2.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	spi2.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	spi2.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	spi2.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	spi2.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV2; //generates sclk of 8MHz
	spi2.SPIConfig.SPI_SSM = SPI_SSM_EN; //software slave management enables for NSS pin


	SPI_Init(&spi2);

	//Bring master's NSS line to +Vcc
	SPI_SSIConfig(((&spi2)->pSPIx), ENABLE);


	//enable the SPI2 peripheral
	SPI_PeripheralControl(((&spi2)->pSPIx), ENABLE);



	char user_data[] = "hello world";

	SPI_SendData(((&spi2)->pSPIx), (uint8_t *) user_data, strlen(user_data));

	//Confirm SPI is not busy
	while( SPI_GetFlagStatus((pSPI2->pSPIx), SPI_BSY_FLAG) ); //block program execution

	SPI_PeripheralControl(((&spi2)->pSPIx), DISABLE);

	while(1);

	return 0;
}
