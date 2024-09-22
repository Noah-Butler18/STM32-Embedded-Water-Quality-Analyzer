/*
 * 014usart_tx.c
 *
 *  Created on: Apr 17, 2024
 *      Author: butle
 */



/*
 * Exercise:
 *
 * Usart send data to arduino from STM32
 *
 * Write a program to send some message over UART from STM32 board to Arduino board.
 * The Arduino board will display the message on its serial monitor (sent from STM)
 *
 * 1. Baud - 115200 bps
 * Frame format - 1 stop bit, 8 data bits, no parity
 *
 */


#include "stm32f407vg.h"
#include <string.h>
#include <stdio.h>






void initialize_gpios(void);
void initialize_usart(void);
void delay(void);

char Tx[] = "UART Tx Testing..\n\r";

USART_Handle_t usart2;

int main(void)
{



	//1. Configure the STM32 GPIO pins for usart
	initialize_gpios();

	//2. Configure the STM32 usart peripheral
	initialize_usart();

	//Enable peripheral
	USART_PeripheralControl(usart2.pUSARTx, ENABLE);

	while(1)
	{
		//wait for button press
		while( !( GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)) )
			;

		delay();

		//Send data
		USART_SendData(&usart2, (uint8_t *) Tx, strlen(Tx));
	}


}

void initialize_gpios(void)
{
	//Initialize usart pins of the master STM32
	GPIO_Handle_t GPIOusartPins;
	memset(&GPIOusartPins,0,sizeof(GPIOusartPins)); 		//sets each member element of the structure to zero. Avoids bugs caused by random garbage values in local variables upon first declaration

	GPIOusartPins.pGPIOx = GPIOA;
	GPIOusartPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	GPIOusartPins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_MODE_AF7;
	GPIOusartPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIOusartPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	GPIOusartPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH; //4nS t_fall when pulling line down

		//USART 2 Tx
	GPIOusartPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
	GPIO_Init(&GPIOusartPins);

		//USART 2 Rx
	GPIOusartPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
	GPIO_Init(&GPIOusartPins);

		//internal push button
	GPIO_Handle_t Button;

	Button.pGPIOx = GPIOA;
	Button.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	Button.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	Button.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	Button.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&Button);

}

void initialize_usart(void)
{
	memset(&usart2,0,sizeof(usart2)); 		//sets each member element of the structure to zero. Avoids bugs caused by random garbage values in local variables upon first declaration


	usart2.pUSARTx = USART2;
	usart2.USART_Config.USART_Mode = USART_MODE_ONLY_TX;
	usart2.USART_Config.USART_Baud = USART_STD_BAUD_115200;
	usart2.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	usart2.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
	usart2.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
	usart2.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;


	USART_Init(&usart2);
}


void delay(void)
{
	//This will cause ~200ms of delay with a 16MHz clock
	for( int i = 0 ; i < 500000/2 ; i++ );
}
