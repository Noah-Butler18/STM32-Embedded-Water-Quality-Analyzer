/*
 * 011i2c_master_rx.c
 *
 *  Created on: Apr 5, 2024
 *      Author: butle
 */

/*
 * Exercise:
 *
 * I2C Master (STM32) and I2C slave (Arduino) communication
 *
 * When button on master is pressed, master should read and display data from the Arduino board (slave)
 * Master must get the length of the data from the slave to read subsequent data from the slave
 *
 * 1. Use I2C SCL = 100kHz (Sm)
 * 2. Can use internal pull up resistors on both SDA and SCL line
 * 		Note: logic level converter has pull up resistors already. Test with and without enabling the internal PUs
 */


#include "stm32f407vg.h"
#include <string.h>
#include <stdio.h>

#define COMMAND_READ_LENGTH			0x51U
#define COMMAND_READ_DATA			0x52U

//Flag variable
uint8_t RxCMPLT = RESET;

void initialize_gpios(void);
void initialize_i2c(void);
void delay(void);
extern void initialise_monitor_handles(void);

char Rx[50];
I2C_Handle_t i2c1;

int main(void)
{
	//semihosting-enable
	initialise_monitor_handles();

	printf("Application starting...\n");

	//1. Configure the STM32 GPIO pins for I2C as well as button
	initialize_gpios();

	//1.5 Configure button as an interrupt - this will trigger I2C communication
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI0, ENABLE);
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI0, NVIC_IRQ_PRIO_15);


	//2. Configure the STM32 I2C peripheral
	initialize_i2c();

	//2.5 Configure I2C IRQs
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQPriorityConfig(IRQ_NO_I2C1_EV, NVIC_IRQ_PRIO_2);

	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);
	I2C_IRQPriorityConfig(IRQ_NO_I2C1_ER, NVIC_IRQ_PRIO_1);

		// a. button will trigger interrupt
	while(1)
	{

	}


}

void initialize_gpios(void)
{
	//Initialize I2C pins of the master STM32
	GPIO_Handle_t GPIOi2cPins;
	memset(&GPIOi2cPins,0,sizeof(GPIOi2cPins)); 		//sets each member element of the structure to zero. Avoids bugs caused by random garbage values in local variables upon first declaration

	GPIOi2cPins.pGPIOx = GPIOB;
	GPIOi2cPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	GPIOi2cPins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_MODE_AF4;
	GPIOi2cPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	GPIOi2cPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	GPIOi2cPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH; //4nS t_fall when pulling line down

		//SCL
	GPIOi2cPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&GPIOi2cPins);

		//SDA
	GPIOi2cPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&GPIOi2cPins);


	//Initialize button - connected to PA0
	GPIO_Handle_t GPIOButton;
	memset(&GPIOButton,0,sizeof(GPIOButton)); 		//sets each member element of the structure to zero. Avoids bugs caused by random garbage values in local variables upon first declaration

	GPIOButton.pGPIOx = GPIOA;
	GPIOButton.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIOButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RT;
	GPIOButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	GPIOButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GPIOButton);
}

void initialize_i2c(void)
{
	memset(&i2c1,0,sizeof(i2c1)); 		//sets each member element of the structure to zero. Avoids bugs caused by random garbage values in local variables upon first declaration


	i2c1.pI2Cx = I2C1;
	i2c1.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
	i2c1.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
	i2c1.I2C_Config.I2C_DeviceAddress = 0x45;
	i2c1.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;


	I2C_Init(&i2c1);

}

void EXTI0_IRQHandler(void)
{
	delay();
	//Implementation of actual interrupt - what happens after an interrupt is sent? What do we want the processor to now do?

	//The interrupt is configured to automatically trigger when button is pressed
	GPIO_IRQHandling(GPIO_PIN_NO_0);

	//enable peripheral hardware
	I2C_PeripheralControl(i2c1.pI2Cx, ENABLE);

	//1. Master writes to slave the command for send data length
	uint8_t cmd = COMMAND_READ_LENGTH;
	uint8_t SlaveAddr = 0x66;

	while( ( I2C_MasterSendDataIT(&i2c1, &cmd, sizeof(cmd), SlaveAddr, I2C_ENABLE_SR) != I2C_READY ))
		;

		//slave stores the command into a variable for later use. Master shuts down communication after this part is finished

	//2. Master reads from slave the number of bytes in the data set (in a single byte)
	uint8_t RxLen;
	while( ( I2C_MasterReceiveDataIT(&i2c1, &RxLen, 1, SlaveAddr, I2C_ENABLE_SR) != I2C_READY ))
		;

	//3. Master writes to slave the command for send data string
	cmd = COMMAND_READ_DATA;
	while( ( I2C_MasterSendDataIT(&i2c1, &cmd, sizeof(cmd), SlaveAddr, I2C_ENABLE_SR) != I2C_READY ))
		;

	RxCMPLT = RESET;

	//4. Master reads from slave the actual data and stores in RxBuffer
	while( (I2C_MasterReceiveDataIT(&i2c1, (uint8_t *) Rx, RxLen, SlaveAddr, I2C_DISABLE_SR) != I2C_READY ))
		;


	//wait for reception of data to finish
	while( RxCMPLT != SET )
		;

	//print contents to console
	printf("\n");
	printf("Data : ");
	for( uint8_t i = 0; i < 32; i++ )
	{
		if(Rx[i] == '\n')
			break;

		printf("%c", Rx[i]);
	}
	printf("\n");

	RxCMPLT = RESET;

	//Disable the I2C peripheral once communication is over
	I2C_PeripheralControl(i2c1.pI2Cx,DISABLE);

}

void I2C1_EV_IRQHandler(void)
{
	I2C_EV_IRQHandling(&i2c1);
}

void I2C1_ER_IRQHandler(void)
{
	I2C_ER_IRQHandling(&i2c1);
}

void delay(void)
{
	//This will cause ~200ms of delay with a 16MHz clock
	for( int i = 0 ; i < 500000/2 ; i++ );
}

void I2C_ApplicationEventCallBack(I2C_Handle_t *pI2CHandle, uint8_t AppEvent)
{
	if( AppEvent == I2C_EV_TX_COMPLETE )
	{
		printf("Tx is complete\n");
	}
	else if( AppEvent == I2C_EV_RX_COMPLETE )
	{
		RxCMPLT = SET;
		printf("Rx is complete\n");
	}
	else if( AppEvent == I2C_ERROR_AF )
	{
		printf("Error : ACK failure\n");
		//In master, ACK failure happens when slave does not send an ACK after a byte is sent

		//Close sending of data
		I2C_CloseSendData(pI2CHandle);

		//Generate stop condition
		I2C_GenerateCondition(pI2CHandle, STOP);

		//Hang in infinite loop
		while(1);
	}
}
