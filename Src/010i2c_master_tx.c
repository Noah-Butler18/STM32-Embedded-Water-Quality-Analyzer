/*
 * 010_i2c_tx_testing.c
 *
 *  Created on: Apr 4, 2024
 *      Author: butle
 */


/*
 * Exercise:
 *
 * I2C Master (STM32) and I2C slave (Arduino) communication
 *
 * When button on master is pressed, master should send data to the Arduino board (slave)
 * The data received by the Arduino board will be displayed on the serial monitor terminal of the Arduino IDE
 *
 * 1. Use I2C SCL = 100kHz (Sm)
 * 2. Can use internal pull up resistors on both SDA and SCL line
 * 		Note: logic level converter has pull up resistors already. Test with and without enabling the internal PUs
 */


#include "stm32f407vg.h"
#include <string.h>

void initialize_gpios(void);
void initialize_i2c(void);
void delay(void);

char data[] = "hello world";
I2C_Handle_t i2c1;

int main(void)
{

	//1. Configure the STM32 GPIO pins for I2C as well as button
	initialize_gpios();

	//1.5 Configure button as an interrupt - this will trigger I2C communication
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI0, ENABLE);
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI0, NVIC_IRQ_PRIO_15);


	//2. Configure the STM32 I2C peripheral
	initialize_i2c();

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
	i2c1.I2C_Config.I2C_DeviceAddress = 0x01;
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

	//start comms, send address phase, then send all information. Close comms once finished
	uint8_t Len = strlen(data);
	uint8_t SlaveAddr = 0x68;
	I2C_MasterSendData((&i2c1), (uint8_t *) data, Len, SlaveAddr, 0);

	//Disable the I2C peripheral once communication is over
	I2C_PeripheralControl(i2c1.pI2Cx,DISABLE);

}

void delay(void)
{
	//This will cause ~200ms of delay with a 16MHz clock
	for( int i = 0 ; i < 500000/2 ; i++ );
}
