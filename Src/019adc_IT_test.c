/*
 * 019adc_IT_test.c
 *
 *  Created on: Jul 20, 2024
 *      Author: butle
 */


/*
 * ADC test:
 *
 * 1. Configure GPIO pin PA1 as analog input channel 1 of ADC1 peripheral
 * 2. Configure TIM2 timer to generate an interrupt every 5 seconds
 * 3. When TIM2 timer interrupt is generated, read ADC value and store in user variable
 *
 * Can use 0V and 3V pins to test the ADC - this should give you 0 and 4096 digital values if using 12-bit resolution
 * NOTE: DISABLE HARDWARE FPU OR ELSE CRITICAL FAULT OCCURS AND PROGRAM WILL NOT WORK
 */


#include "stm32f407vg.h"

ADC_Handle_t pADC1Handle;
GPIO_Handle_t pGPIOAHandle;

//User data buffer pointer
uint16_t UserData;
float volts;

extern void initialise_monitor_handles(void);

int main(void)
{
	//Semi-hosting enable
	initialise_monitor_handles();

	printf("Application starting...\n");



	/************************ GPIO INIT ***************/
	memset(&pGPIOAHandle,0,sizeof(pGPIOAHandle));

	pGPIOAHandle.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;		//GPIO pin in analog mode for ADC
	pGPIOAHandle.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;		//GPIO pin in analog mode for ADC
	pGPIOAHandle.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;		//GPIO output type - don't care
	pGPIOAHandle.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;		//GPIO output speed - don't care
	pGPIOAHandle.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;	//No pull up or pull down resistor

	pGPIOAHandle.pGPIOx = GPIOA;										//Using GPIOA peripheral

	GPIO_Init(&pGPIOAHandle);

	/************************ ADC INIT ***************/
	memset(&pADC1Handle,0,sizeof(pADC1Handle));

	pADC1Handle.ADC_Config.ADC_ClkPrescaler = ADC_CLK_DIV_2; 			//ADC clk = 8MHz
	pADC1Handle.ADC_Config.ADC_Resolution = ADC_RES_12BITS;			//DR resolution = 12 bits
	pADC1Handle.ADC_Config.ADC_DataAlignment = ADC_RIGHT_ALIGNMENT;	//DR alignment = right
	pADC1Handle.ADC_Config.ADC_Mode = ADC_SCAN_CONVERSION_MODE;		//ADC mode = scan
	pADC1Handle.ADC_Config.ADC_SamplingTime[ADC_IN1] = ADC_SMP_3_CYCLES;		//Channel 1 sampling time = 3 cycles
	pADC1Handle.ADC_Config.ADC_AWDHT = 0xFFF;							//High voltage threshold: digital 4095 | analog 3.3V /// digital 2048 | analog 1.65V
	pADC1Handle.ADC_Config.ADC_AWDLT = 0x0;							//Low voltage threshold: digital 0 | analog 0V /// digital 2048 | analog 1.65V
	pADC1Handle.ADC_Config.ADC_Seq_Len = 1;							//channel conversion sequence length = 1 channel
	pADC1Handle.ADC_Config.ADC_Seq_Order[0] = ADC_IN1;					//ADC channel sequence order = 1) ADC_IN1

	pADC1Handle.pADCx = ADC1;											//Using ADC1 peripheral

	ADC_Init(&pADC1Handle);


	/************************ ADC INTERRUPT INIT ***************/
	ADC_IRQInterruptConfig(IRQ_NO_ADC, ENABLE);
	ADC_IRQPriorityConfig(IRQ_NO_ADC, NVIC_IRQ_PRIO_1 );


	/************************ TIM INIT ***************/
	TIM2_5_IRQInterruptConfig(IRQ_NO_TIM2, ENABLE );
	TIM2_5_IRQPriorityConfig(IRQ_NO_TIM2, NVIC_IRQ_PRIO_2 );

	float freq = (float) 1/2;
	TIM2_5_SetIT(TIM2, freq);


	while(1)
	{
		//Voltage calculation
		volts = ( ( UserData / 4095.0 ) * 3.3 );

		printf("The ADC DR is currently reading a value of %d, or %fV\n", UserData, volts);
	}
}


void ADC_ApplicationEventCallBack(ADC_Handle_t *pADCHandle, uint8_t AppEvent)
{
	//User implementation of ADC_ApplicationEventCallBack API

	if( AppEvent == ADC_FLAG_STRT )
	{
		//Application attempting starting conversion with one already in progress

		printf("ADC repeated start condition detected - suspending program...\n");

		while(1);
	}

	if( AppEvent == ADC_FLAG_AWD )
	{
		//Analog watch dog threshold triggered

		if( UserData > ( pADCHandle->pADCx->HTR ) )
		{
			printf("Analog watch dog triggered - over voltage condition detected.\n");
		}
		else if( UserData < ( pADCHandle->pADCx->LTR ) )
		{
			printf("Analog watch dog triggered - under voltage condition detected.\n");
		}

		volts = ( ( UserData / 4095.0 ) * 3.3 );

		printf("\nCurrent voltage reading: %f\n\n", volts);

		printf("Shutting down ADC...\n");
		ADC_PeripheralOnOffControl(pADCHandle->pADCx, DISABLE);

		while(1);
	}

	if( AppEvent == ADC_FLAG_OVR )
	{
		//Overrun triggered

		printf("Overrun condition detected - suspending program...\n");

		//while(1);

		//Clear OVR flag in SR
		ADC_ClearFlag(pADCHandle->pADCx, ADC_FLAG_OVR);

		//Trigger start conversion to recover from OVR condition
		ADC_EnableIT(pADCHandle, &UserData, (pADCHandle->ADC_SeqLen) );
	}
}

void TIM2_IRQHandler(void)
{
	TIM2_5_IRQHandling(TIM2);

	//ADC start up code
	ADC_EnableIT(&pADC1Handle, &UserData, (pADC1Handle.ADC_Config.ADC_Seq_Len) );
}

void ADC_IRQHandler(void)
{
	ADC_IRQHandling(&pADC1Handle);
}
