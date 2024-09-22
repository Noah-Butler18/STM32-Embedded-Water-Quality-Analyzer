/*
 * 001led_toggle.c
 *
 *  Created on: Mar 18, 2024
 *      Author: butle
 */

#include "stm32f407vg.h"

void delay(void);

int main(void)
{
	GPIO_Handle_t GpioLed;

	GpioLed.pGPIOx = GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_PeriClockControl(GPIOD, ENABLE);
	GPIO_Init(&GpioLed);

	while(1)
	{
		GPIO_ToggleOutputPin(GPIOD, 12);
		delay();
	}
}

void delay(void)
{
	for(uint32_t t = 0; t < 200000 ; t++);
}
