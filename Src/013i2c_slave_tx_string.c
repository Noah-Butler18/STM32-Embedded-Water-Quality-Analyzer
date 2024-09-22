/*
 * 011i2c_master_rx.c
 *
 *  Created on: Apr 5, 2024
 *      Author: butle
 */

/*
 * Exercise:
 *
 * I2C Master (Arduino) and I2C slave (STM32) communication
 *
 * Arduino master should read and display data from STM32 slave connected
 * First master has to get the length of the data from the slave to read subsequent data from the slave
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

#define DEVICE_ADDRESS				0x69

//Flag variable
uint8_t RxCMPLT = RESET;

void initialize_gpios(void);
void initialize_i2c(void);
void delay(void);
extern void initialise_monitor_handles(void);

char Tx[] = "This is STM board..";
I2C_Handle_t i2c1;

int main(void)
{
	//semihosting-enable
	initialise_monitor_handles();

	printf("Application starting...\n");

	//1. Configure the STM32 GPIO pins for I2C
	initialize_gpios();

	//2. Configure the STM32 I2C peripheral
	initialize_i2c();

	//2.5 Configure I2C IRQs
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQPriorityConfig(IRQ_NO_I2C1_EV, NVIC_IRQ_PRIO_2);

	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);
	I2C_IRQPriorityConfig(IRQ_NO_I2C1_ER, NVIC_IRQ_PRIO_1);

	//enable peripheral hardware
	I2C_PeripheralControl(i2c1.pI2Cx, ENABLE);

	//Interrupts need to be configured somehow
	I2C_SlaveEnableDisableCallbackEvents(i2c1.pI2Cx, ENABLE);

	i2c1.pI2Cx->CR1 |= ( 0x1 << I2C_CR1_ACK );

		// interrupts will be triggered by master sending address phase and kicking off sequence with ADDR=1
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
}

void initialize_i2c(void)
{
	memset(&i2c1,0,sizeof(i2c1)); 		//sets each member element of the structure to zero. Avoids bugs caused by random garbage values in local variables upon first declaration


	i2c1.pI2Cx = I2C1;
	i2c1.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
	i2c1.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
	i2c1.I2C_Config.I2C_DeviceAddress = DEVICE_ADDRESS;
	i2c1.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;


	I2C_Init(&i2c1);

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
	static uint8_t cnt = 0;
	static uint8_t cmd = 0;

	if( AppEvent == I2C_ERROR_AF )					//AF
	{
		printf("ACK failure\n");

		//check if device is in master mode when this event happens
		if ( pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_MSL ) )
		{
			//In master, ACK failure happens when slave does not send an ACK after a byte is sent

			//Close sending of data
			I2C_CloseSendData(pI2CHandle);

			//Generate stop condition
			I2C_GenerateCondition(pI2CHandle, STOP);

			//Hang in infinite loop
			while(1);
		}
		else			//Device is in slave mode
		{
			//transmission is done
			cmd = 0xFF;
			cnt = 0;
		}
	}
	else if( AppEvent == I2C_EV_STOPF )				//STOPF
	{
		printf("STOP condition detected\n");


	}
	else if( AppEvent == I2C_EV_DATA_REQUEST )		//Slave data request from master
	{
		if( cmd == COMMAND_READ_LENGTH )
		{
			printf("Sending data length\n");

			uint8_t Len = strlen(Tx);
			I2C_SlaveSendData( pI2CHandle->pI2Cx, Len );
		}
		else if( cmd == COMMAND_READ_DATA )
		{
			printf("Sending data : %c \n", Tx[cnt]);

			I2C_SlaveSendData( pI2CHandle->pI2Cx, Tx[cnt++] );
		}
	}
	else if( AppEvent == I2C_EV_DATA_RECEIVE )		//slave data to receive from master
	{
		cmd = I2C_SlaveReceiveData(pI2CHandle->pI2Cx);

		printf("Data Received : Cmd = %x \n", cmd);
	}
}
