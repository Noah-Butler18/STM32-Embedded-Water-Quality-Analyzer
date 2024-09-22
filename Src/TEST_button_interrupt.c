/*
 * TEST_button_interrupt.c
 *
 *  Created on: Mar 27, 2024
 *      Author: butle
 */

#include "stm32f407vg.h"
#include <string.h>

void delay(void);



int main(void)
{

	GPIO_Handle_t GpioButton;
	GPIO_Handle_t GpioLed;

	memset(&GpioButton,0,sizeof(GpioButton)); 		//sets each member element of the structure to zero. Avoids bugs caused by random garbage values in local variables upon first declaration
	memset(&GpioLed,0,sizeof(GpioLed));				//sets each member element of the structure to zero. Avoids bugs caused by random garbage values in local variables upon first declaration


	GpioLed.pGPIOx = GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GpioLed);


	GpioButton.pGPIOx = GPIOA;
	GpioButton.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GpioButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GpioButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	GpioButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GpioButton);


	//IRQ Configurations
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI0, ENABLE);
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI0, NVIC_IRQ_PRIO_15);

	while(1)
	{

	}

	return 0;
}

void EXTI0_IRQHandler(void)
{
	//Implementation of actual interrupt - what happens after an interrupt is sent? What do we want the processor to now do?
	delay();
	GPIO_IRQHandling(GPIO_PIN_NO_0);
	GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
}

void delay(void)
{
	//This will cause ~200ms of delay with a 16MHz clock
	for( int i = 0 ; i < 500000/2 ; i++ );
}
