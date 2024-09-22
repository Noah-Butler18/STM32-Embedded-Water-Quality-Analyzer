/*
 * 007spi_txonly_to_arduino.c
 *
 *  Created on: Mar 26, 2024
 *      Author: butle
 */

/*
Exercise:

SPI Master (STM) and SPI Slave (Arduino) command and response based communication

When the button on the master is pressed, master sends a command to the slave and slave responds as per the command implementation

	1. Use SPI full duplex mode
	2. ST board will be in master mode and arduino will be configured as slave
	3. Use DFF = 0
	4. Use hardware slave management
SCLK speed = 2MHz (fclk = 16MHz)
 */

#include "stm32f407vg.h"
#include <string.h>


//Command code
#define COMMAND_LED_CTRL				0x50
#define COMMAND_SENSOR_READ				0x51
#define COMMAND_LED_READ				0x52
#define COMMAND_PRINT					0x53
#define COMMAND_ID_READ					0x54


#define LED_ON							1
#define LED_OFF							0

//Arduino analog pins
#define ANALOG_PIN0						0
#define ANALOG_PIN1						1
#define ANALOG_PIN2						2
#define ANALOG_PIN3						3
#define ANALOG_PIN4						4
#define ANALOG_PIN5						5

//Arduino LED
#define LED_PIN							9



void initialize_gpios(void);
void initialize_spi(void);
void delay(void);
uint8_t SPI_VerifyResponse(uint8_t ackbyte);


SPI_Handle_t spi2;

int main(void)
{
	uint8_t dummy_write = 0xFF;
	uint8_t dummy_read;


	//Configure the STM32 GPIO pins for SPI as well as button
	initialize_gpios();

	//Configure the STM32 SPI peripheral
	initialize_spi();

	//NSS bit is in hardware mode however to enable the NSS output from the peripheral, you must set its SSOE bit
	SPI_SSOEConfig(((&spi2)->pSPIx),ENABLE);

	while(1)
	{
		while( !(GPIO_ReadFromInputPin(GPIOA, 0)) ) //blocking function waiting for button press
			;

		delay(); //avoid debouncing issues

		//Enable the SPI peripheral to begin communication
		SPI_PeripheralControl(((&spi2)->pSPIx),ENABLE);


		//Send very first command
		//CMD_LED_CTRL		<pin no(1)>		<value(1)>
		uint8_t cmd_code = COMMAND_LED_CTRL;
		uint8_t ackbyte;
		uint8_t args[2];

		SPI_SendData(spi2.pSPIx, &cmd_code, 1);

		//do dummy read to clear RXNE
		SPI_ReceiveData(spi2.pSPIx, &dummy_read, 1);


		//Send some dummy bits (1 byte) in order to extract response from the slave
		SPI_SendData(spi2.pSPIx, &dummy_write, 1);

		//After control has left the SPI_SendData API, there will be new data ready in the Rx buffer. Read whatever the response form the slave was
		SPI_ReceiveData(spi2.pSPIx, &ackbyte, 1);


		//If ACK from slave, continue with program. If NACK, display error message
		if( SPI_VerifyResponse(ackbyte) )
		{
			//send argument
			args[0] = LED_PIN;
			args[1] = LED_ON;
			SPI_SendData(spi2.pSPIx, args, 2);
		}


		//END OF CMD_LED_CTRL



		// 2. Read data from sensor after button press
		// CMD_SENSOR_READ  <analog pin number(1)>
		while( !(GPIO_ReadFromInputPin(GPIOA, 0)) ) //blocking function waiting for button press
			;

		delay(); //avoid debouncing issues

		cmd_code = COMMAND_SENSOR_READ;
		uint8_t SensorVal;
		SPI_SendData(spi2.pSPIx, &cmd_code, 1);

		//do dummy read of DR to clear RXNE and after do a dummy read of the SPI_SR reg to clear OVR flag
		SPI_ReceiveData(spi2.pSPIx, &dummy_read, 1);

		//Send some dummy bits (1 byte) in order to extract response from the slave
		SPI_SendData(spi2.pSPIx, &dummy_write, 1);

		//After control has left the SPI_SendData API, the OVR bit in the status register will be set because of unread input data on the MISO line piling up
		//If OVR flag is not cleared, we will
		SPI_ReceiveData(spi2.pSPIx, &ackbyte, 1);
		SPI_ReceiveData(spi2.pSPIx, &ackbyte, 1);

		//If ACK from slave, continue with program. If NACK, display error message
		if( SPI_VerifyResponse(ackbyte) )
		{
			//send argument
			args[0] = ANALOG_PIN0;

			SPI_SendData(spi2.pSPIx, args, 1);

			//do dummy read to clear RXNE
			SPI_ReceiveData(spi2.pSPIx, &dummy_read, 1);

			//Insert some delay so that we can get a valid response from slave
			delay();

			//Send some dummy bits (1 byte) in order to extract response from the slave
			SPI_SendData(spi2.pSPIx, &dummy_write, 1);

			//After control has left the SPI_SendData API, there will be new data ready in the Rx buffer. Read whatever the response form the slave was
			//Read the value of the sensor
			SPI_ReceiveData(spi2.pSPIx, &SensorVal, 1);
		}

		//END OF CMD_SENSOR_READ


		// 3. Read status of LED on Arduino after button press
		// CMD_LED_READ  <pin number(1)>
		while( !(GPIO_ReadFromInputPin(GPIOA, 0)) ) //blocking function waiting for button press
			;

		delay(); //avoid debouncing issues

		cmd_code = COMMAND_LED_READ;
		SPI_SendData(spi2.pSPIx, &cmd_code, 1);

		//do dummy read of DR to clear RXNE and after do a dummy read of the SPI_SR reg to clear OVR flag
		SPI_ReceiveData(spi2.pSPIx, &dummy_read, 1);

		//Send some dummy bits (1 byte) in order to extract response from the slave
		SPI_SendData(spi2.pSPIx, &dummy_write, 1);

		//After control has left the SPI_SendData API, the OVR bit in the status register will be set because of unread input data on the MISO line piling up
		SPI_ReceiveData(spi2.pSPIx, &ackbyte, 1);

		//If ACK from slave, continue with program. If NACK, display error message
		if( SPI_VerifyResponse(ackbyte) )
		{
			//send argument
			args[0] = LED_PIN;

			SPI_SendData(spi2.pSPIx, args, 1);

			//do dummy read to clear RXNE
			SPI_ReceiveData(spi2.pSPIx, &dummy_read, 1);

			//Send some dummy bits (1 byte) in order to extract response from the slave
			SPI_SendData(spi2.pSPIx, &dummy_write, 1);

			//After control has left the SPI_SendData API, there will be new data ready in the Rx buffer. Read whatever the response form the slave was
			//Read the value of the sensor
			uint8_t ledStatus;
			SPI_ReceiveData(spi2.pSPIx, &ledStatus, 1);
		}

		//END OF CMD_LED_READ



		// 4. Send message to Arduino to print on its serial port
		// CMD_PRINT  <len>  <message>
		while( !(GPIO_ReadFromInputPin(GPIOA, 0)) ) //blocking function waiting for button press
			;

		delay(); //avoid debouncing issues

		cmd_code = COMMAND_PRINT;
		SPI_SendData(spi2.pSPIx, &cmd_code, 1);

		//do dummy read of DR to clear RXNE and after do a dummy read of the SPI_SR reg to clear OVR flag
		SPI_ReceiveData(spi2.pSPIx, &dummy_read, 1);

		//Send some dummy bits (1 byte) in order to extract response from the slave
		SPI_SendData(spi2.pSPIx, &dummy_write, 1);

		//After control has left the SPI_SendData API, the OVR bit in the status register will be set because of unread input data on the MISO line piling up
		SPI_ReceiveData(spi2.pSPIx, &ackbyte, 1);

		//If ACK from slave, continue with program. If NACK, display error message
		if( SPI_VerifyResponse(ackbyte) )
		{
			//send argument
			char message[] = "my name is Noah";
			args[0] = strlen(message);


			SPI_SendData(spi2.pSPIx, args, 1); //send length information first
			SPI_SendData(spi2.pSPIx, (uint8_t *) message, args[0]); //send message next
		}

		//END OF COMMAND_PRINT



		// 5. Receive ID data of Arduino
		// COMMAND_ID_READ
		while( !(GPIO_ReadFromInputPin(GPIOA, 0)) ) //blocking function waiting for button press
			;

		delay(); //avoid debouncing issues

		cmd_code = COMMAND_ID_READ;
		SPI_SendData(spi2.pSPIx, &cmd_code, 1);

		//do dummy read of DR to clear RXNE and after do a dummy read of the SPI_SR reg to clear OVR flag
		SPI_ReceiveData(spi2.pSPIx, &dummy_read, 1);

		//Send some dummy bits (1 byte) in order to extract response from the slave
		SPI_SendData(spi2.pSPIx, &dummy_write, 1);

		//After control has left the SPI_SendData API, the OVR bit in the status register will be set because of unread input data on the MISO line piling up
		SPI_ReceiveData(spi2.pSPIx, &ackbyte, 1);
		SPI_ReceiveData(spi2.pSPIx, &ackbyte, 1);

		//If ACK from slave, continue with program. If NACK, display error message
		if( SPI_VerifyResponse(ackbyte) )
		{
			//Begin to receive data from Arduino - should be 10 bytes of board ID string
			uint8_t ID[10];
			SPI_ReceiveData(spi2.pSPIx, ID, 10);

		}

		//END OF COMMAND_ID_READ




		//6. Send last command - turn OFF LED
		//CMD_LED_CTRL		<pin no(1)>		<value(1)>
		while( !(GPIO_ReadFromInputPin(GPIOA, 0)) ) //blocking function waiting for button press
					;

		delay(); //avoid debouncing issues


		cmd_code = COMMAND_LED_CTRL;

		SPI_SendData(spi2.pSPIx, &cmd_code, 1);

		//do dummy read to clear RXNE
		SPI_ReceiveData(spi2.pSPIx, &dummy_read, 1);


		//Send some dummy bits (1 byte) in order to extract response from the slave
		SPI_SendData(spi2.pSPIx, &dummy_write, 1);

		//After control has left the SPI_SendData API, there will be new data ready in the Rx buffer. Read whatever the response form the slave was
		SPI_ReceiveData(spi2.pSPIx, &ackbyte, 1);


		//If ACK from slave, continue with program. If NACK, display error message
		if( SPI_VerifyResponse(ackbyte) )
		{
			//send argument
			args[0] = LED_PIN;
			args[1] = LED_OFF;
			SPI_SendData(spi2.pSPIx, args, 2);
		}


		//END OF CMD_LED_CTRL



		//Confirm SPI is not busy
		while( SPI_GetFlagStatus(((&spi2)->pSPIx), SPI_BSY_FLAG) ); //block program execution

		//Disable the SPI peripheral once communication is over
		SPI_PeripheralControl(((&spi2)->pSPIx),DISABLE);

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

		//MISO
	GPIOSpiPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	GPIO_Init(&GPIOSpiPins);

		//MOSI
	GPIOSpiPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&GPIOSpiPins);


	//Initialize button - connected to PA0
	GPIO_Handle_t GPIOButton;
	memset(&GPIOButton,0,sizeof(GPIOButton)); 		//sets each member element of the structure to zero. Avoids bugs caused by random garbage values in local variables upon first declaration

	GPIOButton.pGPIOx = GPIOA;
	GPIOButton.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIOButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
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
	spi2.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV256;
	spi2.SPIConfig.SPI_SSM = SPI_SSM_DI;

	SPI_Init(&spi2);
}

void delay(void)
{
	//This will cause ~200ms of delay with a 16MHz clock
	for( int i = 0 ; i < 500000/2 ; i++ );
}

uint8_t SPI_VerifyResponse(uint8_t ackbyte)
{
	if(ackbyte==0xF5) //ACK
	{
		return 1;
	}
	else //NACK
	{
		return 0;
	}
}
