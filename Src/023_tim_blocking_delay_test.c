/*
 * 023_tim_blocking_delay_test.c
 *
 *  Created on: Aug 13, 2024
 *      Author: butle
 */


#include "stm32f407vg.h"

GPIO_Handle_t led;

int main(void)
{


	memset(&led,0,sizeof(led));				//sets each member element of the structure to zero. Avoids bugs caused by random garbage values in local variables upon first declaration


	led.pGPIOx = GPIOD;
	led.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	led.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&led);


	//TIM5 counter init
	//us = 1000000 is equivalent to 1 trigger every second
	double us;

	TIM2_5_SetDelayInit(TIM5);

	while(1)
	{
		us = 1000000;

		TIM2_5_SetDelay(TIM5, us);

		//WAIT FOR FLAG TO INDICATE TIMER UP
		while( !(TIM2_5_GetFlagStatus(TIM5, TIM_FLAG_UIF)) )
			;

		GPIO_ToggleOutputPin(led.pGPIOx, led.GPIO_PinConfig.GPIO_PinNumber);

		// Reset UIF flag in SR for after an update event occurs, otherwise interrupts will continue to happen over and over again
		TIM2_5_ClearFlag(TIM5, TIM_FLAG_UIF);

		us = 5000000;

		TIM2_5_SetDelay(TIM5, us);

		//WAIT FOR FLAG TO INDICATE TIMER UP
		while( !(TIM2_5_GetFlagStatus(TIM5, TIM_FLAG_UIF)) )
			;

		GPIO_ToggleOutputPin(led.pGPIOx, led.GPIO_PinConfig.GPIO_PinNumber);

		// Reset UIF flag in SR for after an update event occurs, otherwise interrupts will continue to happen over and over again
		TIM2_5_ClearFlag(TIM5, TIM_FLAG_UIF);
	}
}
