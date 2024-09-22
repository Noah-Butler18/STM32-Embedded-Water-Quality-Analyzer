/*
 * ds18b20_temp_sensor.c
 *
 *  Created on: Aug 8, 2024
 *      Author: butle
 */

#include "ds18b20_temp_sensor.h"

GPIO_Handle_t DS18B20_pin;

/* Limited visibility helper function prototypes */
static void DS18B20_GPIOControl(uint8_t InOrOut);


void DS18B20_Config(void)
{

	memset(&DS18B20_pin,0,sizeof(DS18B20_pin));

	//Configure DQ pin
	DS18B20_pin.pGPIOx = DS18B20_GPIO_PORT;

	DS18B20_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;						//Pin will start out as output, then switch between input / output as comms. progress
	DS18B20_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;					//As required by Maxim’s exclusive 1-Wire bus protocol. Data line requires a PU resisotr of 4.7k Ohms
	DS18B20_pin.GPIO_PinConfig.GPIO_PinPuPdControl = DS18B20_GPIO_PIN_NO_PUPD;		//Using external 4.7kOhm resistor
	DS18B20_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH; 					//4nS t_fall when pulling line down
	DS18B20_pin.GPIO_PinConfig.GPIO_PinNumber = DS18B20_GPIO_PIN;

	GPIO_Init(&DS18B20_pin);

	TIM2_5_SetDelayInit(DS18B20_TIM_PERIPHERAL);

}


void DS18B20_MasterSendData(uint8_t *TxBuffer, uint8_t len)
{
	//1. Begin sending data over bus bit by bit, LSB first
	//NOTE: data is transmitted least significant to most significant over bus

	uint8_t i, temp, temp2;
	while( len )
	{
		//temporary variable to save contents of buffer
		temp = *TxBuffer;

		for( i = 0 ; i < 8 ; i++ )
		{
			temp2 = temp & 0x1;
			DS18B20_MasterGenerateWriteTimeSlot( temp2 );
			temp >>= 1;
		}

		len--;
		TxBuffer++;
	}
}


void DS18B20_MasterReceiveData(uint8_t *RxBuffer, uint8_t len)
{
	//1. Begin reading data over bus bit by bit, LSB first
	//NOTE: data is transmitted least significant to most significant over bus
	//Strategy: start at the end of the buffer. load the first bit into the LSB of the buffer, second bit into the next LSB, etc.

	RxBuffer += (len-1);
	uint8_t temp, i;

	while( len )
	{
		//temporary variable to save contents of read
		temp = 0;

		//Save bits of DS18B20 registers (going LSB to MSB) in proper order by shifting each bit into correct position
		for( i = 0 ; i < 8 ; i++ )
		{
			temp |= ( ( DS18B20_MasterGenerateReadTimeSlot() ) << i );
		}

		//2. Save contents of temp buffer into user Rx buffer
		*RxBuffer = temp;

		len--;
		RxBuffer--;
	}
}


void DS18B20_MasterSendInitializeSequence(void)
{
	//1. Master send reset pulse - send logic low on bus
	DS18B20_GPIOControl( MASTER_SET_PIN_OUTPUT );
	GPIO_WriteToOutputPin(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN, 0);

	//2. Wait 480us
	TIM2_5_Delay(DS18B20_TIM_PERIPHERAL, MASTER_TX_RESET_HOLD_USECS);

	//3. DS18B20 waits 15-60us to send

	//4. Master read presence pulse - DS18B20 write a logic 0 to the bus and holds it for 60-180us
	DS18B20_GPIOControl( MASTER_SET_PIN_INPUT );
	TIM2_5_Delay(DS18B20_TIM_PERIPHERAL, MASTER_RX_PRESENCE_PULSE_USECS);
		//Time elapsed at this point: ~630us+

	//5. Master confirms presence pulse was sent from DS18B20
	while( !(GPIO_ReadFromInputPin(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN)) )
		;

	//6. Fulfill 1-wire requirement of master Rx phase being at least 480us
	TIM2_5_Delay(DS18B20_TIM_PERIPHERAL, MASTER_RX_PRESENCE_HOLD_USECS);
		//Time elapsed at this point: ~960us+
}


void DS18B20_MasterGenerateWriteTimeSlot(uint8_t WriteValue)
{
	//1. Master pulls 1-wire bus low and releases within 15us
	DS18B20_GPIOControl(MASTER_SET_PIN_OUTPUT);
	GPIO_WriteToOutputPin(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN, 0);

	if( WriteValue )
	{
		//2. If generating a write '1' time slot, release bus within 15us but wait atleast 1us. Pull-up resistor will automatically pull bus up to HIGH

		//NOTE: THERE IS A NATURAL 5.75us DELAY FROM SETTING GPIO PIN AS INPUT
		DS18B20_GPIOControl(MASTER_SET_PIN_INPUT);
	}

	//3. wait until end of write time slot for DS18B20 to sample the data bus (min. 60us)
	TIM2_5_Delay(DS18B20_TIM_PERIPHERAL, MASTER_TX_RX_TIMESLOT_HOLD_USECS);

	//4. Release bus and wait recovery time in-between read or write time slots

	//NOTE: THERE IS A NATURAL 5.75us DELAY FROM SETTING GPIO PIN AS INPUT
	DS18B20_GPIOControl(MASTER_SET_PIN_INPUT);
}


uint8_t DS18B20_MasterGenerateReadTimeSlot(void)
{
	//1. Master pulls 1-wire bus low for at least 1us then and releases

	DS18B20_GPIOControl(MASTER_SET_PIN_OUTPUT);
	GPIO_WriteToOutputPin(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN, 0);

	//NOTE: THERE IS A NATURAL 5.75us DELAY FROM SETTING GPIO PIN AS INPUT
	DS18B20_GPIOControl(MASTER_SET_PIN_INPUT);

	//2. DS18B20 begins transmitting either a 1 or 0. Data is valid for at most 15us, so master should sample data before then
	//TIM2_5_Delay(DS18B20_TIM_PERIPHERAL, MASTER_RX_SAMPLE_USECS);
	uint8_t val = GPIO_ReadFromInputPin(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN);

	//3. wait until end of read time slot for DS18B20 to sample the data bus (min. 60us)
	TIM2_5_Delay(DS18B20_TIM_PERIPHERAL, MASTER_TX_RX_TIMESLOT_HOLD_USECS);

	return val;
}


float DS18B20_ConvertTemp(uint8_t *TempBuffer)
{
	//This assumes a 2-element array is passed with MSB byte first then LSB byte last
	uint16_t temperature = (*TempBuffer) << 8;
	temperature |= *(++TempBuffer);

	return ( (float) temperature / 16.0 );
}


/*************************** Helper functions ****************************/

static void DS18B20_GPIOControl(uint8_t InOrOut)
{
	if( InOrOut == MASTER_SET_PIN_INPUT )
	{
		DS18B20_GPIO_PORT->MODER &= ~( 0x3 << ( DS18B20_GPIO_PIN * 2 ) );
	}
	else
	{
		DS18B20_GPIO_PORT->MODER &= ~( 0x3 << ( DS18B20_GPIO_PIN * 2 ) ); 	//clear mode bit field first
		DS18B20_GPIO_PORT->MODER |= ( 0x1 << ( DS18B20_GPIO_PIN * 2 ) );	//set pin as OUT
	}
}
