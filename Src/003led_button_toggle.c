/*
 * 003led_button_toggle.c
 *
 *  Created on: Mar 20, 2024
 *      Author: butle
 */

#include "stm32f407vg.h"

void delay(void);

int main(void)
{
	GPIO_Handle_t GpioButton;
	GPIO_Handle_t GpioLed;

	GpioButton.pGPIOx = GPIOD;
	GpioButton.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GpioButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GpioButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	GpioButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_PeriClockControl(GpioButton.pGPIOx, ENABLE);

	GPIO_Init(&GpioButton);


	GpioLed.pGPIOx = GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;


	GPIO_PeriClockControl(GpioLed.pGPIOx, ENABLE);

	GPIO_Init(&GpioLed);


	//IRQ Configurations
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI9_5, ENABLE);
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, NVIC_IRQ_PRIO_15);

	while(1)
	{
		if( GPIO_ReadFromInputPin(GpioButton.pGPIOx,GpioButton.GPIO_PinConfig.GPIO_PinNumber) == 0 )
		{
			delay();
			GPIO_ToggleOutputPin(GpioLed.pGPIOx,GpioLed.GPIO_PinConfig.GPIO_PinNumber);
		}
	}

	return 0;
}

void delay(void)
{
	for( int i = 0 ; i < 500000/2 ; i++ );
}

