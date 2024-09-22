/*
 * 002led_button.c
 *
 *  Created on: Mar 19, 2024
 *      Author: butle
 */

#include "stm32f407vg.h"



int main(void)
{
	GPIO_Handle_t Button; //user push button is jumpered internally into port A pin 0
	GPIO_Handle_t GpioLed;

	Button.pGPIOx = GPIOA;
	Button.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	Button.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	Button.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	Button.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;



	GpioLed.pGPIOx = GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_PeriClockControl(GPIOD, ENABLE);
	GPIO_Init(&Button);
	GPIO_Init(&GpioLed);


	while(1)
	{
		while( (GPIO_ReadFromInputPin(Button.pGPIOx,0) ))
		{
			GPIO_WriteToOutputPin(GpioLed.pGPIOx, GpioLed.GPIO_PinConfig.GPIO_PinNumber, 1);
		}

		GPIO_WriteToOutputPin(GpioLed.pGPIOx, GpioLed.GPIO_PinConfig.GPIO_PinNumber, 0);
	}
}

