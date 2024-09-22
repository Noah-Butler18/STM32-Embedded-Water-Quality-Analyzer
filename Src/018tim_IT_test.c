/*
 * 018tim_IT_test.c
 *
 *  Created on: Jul 3, 2024
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


	//TIM2 IRQ configuration
	TIM2_5_IRQInterruptConfig(IRQ_NO_TIM2, ENABLE );
	TIM2_5_IRQPriorityConfig(IRQ_NO_TIM2, NVIC_IRQ_PRIO_1 );

	//TIM2 counter init
	//freq = 0.0167 is equivalent to 1 trigger every minute
	double freq = 0.0167;

	TIM2_5_SetIT(TIM2, freq);

	while(1);




}




void TIM2_IRQHandler(void)
{
	TIM2_5_IRQHandling(TIM2);
	GPIO_ToggleOutputPin(led.pGPIOx, led.GPIO_PinConfig.GPIO_PinNumber);

}

