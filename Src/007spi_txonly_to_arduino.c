/*
 * 007spi_txonly_to_arduino.c
 *
 *  Created on: Mar 26, 2024
 *      Author: butle
 */

/*
 * Exercise:

When the button on the master is pressed, master should send string of data to the Arduino slave connected.
The data received by the Arduino will be displayed on the Arduino serial port.

	1. Use SPI full duplex mode
	2. ST board will be in SPI master mode and Arduino will be configured for SPI slave mode
	3. Use DFF = 0
	4. Use hardware slave management (SSM = 0)
	5. SCLK speed = 2MHz, fclk = 16MHz

In this exercise master is not going to receive anything from the slave. So you may not configure the MISO pin.

NOTE: slave does not know how many bytes of data master is going to send. So master first sends the number bytes info which slave is going to receive next.

 */

#include "stm32f407vg.h"
#include <string.h>

void initialize_gpios(void);
void initialize_spi(void);
void delay(void);

char data[] = "hello world";
SPI_Handle_t spi2;

int main(void)
{

	//1. Configure the STM32 GPIO pins for SPI as well as button
	initialize_gpios();

	//1.5 Configure button as an interrupt - this will trigger SPI communication
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI0, ENABLE);
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI0, NVIC_IRQ_PRIO_15);

	//2. Configure the STM32 SPI peripheral
	initialize_spi();

	//3. NSS bit is in hardware mode however to enable the NSS output from the peripheral, you must set its SSOE bit
	SPI_SSOEConfig(((&spi2)->pSPIx),ENABLE);

		// a. button will trigger interrupt
	while(1)
	{

	}


}

void initialize_gpios(void)
{
	//Initialize SPI pins of the master STM32
	GPIO_Handle_t GPIOSpiPins;
	memset(&GPIOSpiPins,0,sizeof(GPIOSpiPins)); 		//sets each member element of the structure to zero. Avoids bugs caused by random garbage values in local variables upon first declaration

	GPIOSpiPins.pGPIOx = GPIOB;
	GPIOSpiPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	GPIOSpiPins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_MODE_AF5;
	GPIOSpiPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIOSpiPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIOSpiPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;

		//NSS
	GPIOSpiPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&GPIOSpiPins);

		//SCLK
	GPIOSpiPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&GPIOSpiPins);

		//MOSI
	GPIOSpiPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&GPIOSpiPins);

		//NO MISO needed

	//Initialize button - connected to PA0
	GPIO_Handle_t GPIOButton;
	memset(&GPIOButton,0,sizeof(GPIOButton)); 		//sets each member element of the structure to zero. Avoids bugs caused by random garbage values in local variables upon first declaration

	GPIOButton.pGPIOx = GPIOA;
	GPIOButton.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIOButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GPIOButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	GPIOButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GPIOButton);
}

void initialize_spi(void)
{
	spi2.pSPIx = SPI2;
	spi2.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	spi2.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	spi2.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	spi2.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	spi2.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	spi2.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV128;
	spi2.SPIConfig.SPI_SSM = SPI_SSM_DI;

	SPI_Init(&spi2);
}

void EXTI0_IRQHandler(void)
{
	delay();
	//Implementation of actual interrupt - what happens after an interrupt is sent? What do we want the processor to now do?

	//The interrupt is configured to automatically trigger when button is pressed
	GPIO_IRQHandling(GPIO_PIN_NO_0);

	//Enable the SPI peripheral to begin communication
	SPI_PeripheralControl(((&spi2)->pSPIx),ENABLE);

	//First send length information
	uint8_t dataLen = strlen(data);
	SPI_SendData(((&spi2)->pSPIx), &dataLen, 1);

	//Then, send actual data
	SPI_SendData(((&spi2)->pSPIx), (uint8_t *) data, strlen(data));

	//Confirm SPI is not busy
	while( SPI_GetFlagStatus(((&spi2)->pSPIx), SPI_BSY_FLAG) ); //block program execution

	//Disable the SPI peripheral once communication is over
	SPI_PeripheralControl(((&spi2)->pSPIx),DISABLE);

}

void delay(void)
{
	//This will cause ~200ms of delay with a 16MHz clock
	for( int i = 0 ; i < 500000/2 ; i++ );
}
