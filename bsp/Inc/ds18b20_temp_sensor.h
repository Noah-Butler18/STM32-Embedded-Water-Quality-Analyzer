/*
 * ds18b20_temp_sensor.h
 *
 *  Created on: Aug 8, 2024
 *      Author: butle
 */

#ifndef INC_DS18B20_TEMP_SENSOR_H_
#define INC_DS18B20_TEMP_SENSOR_H_

#include "stm32f407vg.h"

/*
 * Application configurable items
 */
#define DS18B20_GPIO_PORT						GPIOA					//PA3 is free IO according to user manual
#define DS18B20_GPIO_PIN						GPIO_PIN_NO_3
#define DS18B20_GPIO_PIN_OP_TYPE				GPIO_OP_TYPE_OD			//Fixed value - required for 1-wire protocol
#define DS18B20_GPIO_PIN_NO_PUPD				GPIO_NO_PUPD			//Fixed value - will be using ~5kOhm external resistor. Should not be changed

#define DS18B20_TIM_PERIPHERAL					TIM5


/*
 * Master - DS18B20 Maxim 1-wire communication protocol timing requirements
 */
#define MASTER_TX_RESET_HOLD_USECS				480
#define MASTER_HOLD_FOR_DS18B20_USECS			60

#define MASTER_RX_PRESENCE_PULSE_USECS			150
#define MASTER_RX_PRESENCE_HOLD_USECS			330

#define MASTER_TX_RX_TIMESLOT_HOLD_USECS		60
#define MASTER_TX_RX_RECOVERY_HOLD_USECS		(0.1)

#define MASTER_RX_INITIATE_USECS				1
#define MASTER_RX_SAMPLE_USECS					10

/*
 * Master GPIO pin control
 */
#define MASTER_SET_PIN_INPUT					0
#define MASTER_SET_PIN_OUTPUT					1

/*
 * Master Tx ROM commands
 */
#define MASTER_COMMAND_SEARCH_ROM				0xF0
#define MASTER_COMMAND_READ_ROM					0x33
#define MASTER_COMMAND_MATCH_ROM				0x55
#define MASTER_COMMAND_SKIP_ROM					0xCC
#define MASTER_COMMAND_ALARM_SEARCH				0xEC

/*
 * Master Tx Function commands
 */
#define MASTER_COMMAND_CONVERT_T				0x44
#define MASTER_COMMAND_WRITE_SCRATCHPAD			0x4E
#define MASTER_COMMAND_READ_SCRATCHPAD			0xBE
#define MASTER_COMMAND_COPY_SCRATCHPAD			0x48
#define MASTER_COMMAND_RECALL_E2				0xB8
#define MASTER_COMMAND_READ_POWER_SUPPLY		0xB4


/******************************************************************************************
 *								APIs supported by this driver
 *		 For more information about the APIs check the function definitions
 ******************************************************************************************/


/*
 * Init. GPIO pin on STM32 for Maxim 1-wire communication
 */
void DS18B20_Config(void);


/*
 * Master <-> DS18B20 communication functions
 */
void DS18B20_MasterSendInitializeSequence(void);
void DS18B20_MasterSendData(uint8_t *TxBuffer, uint8_t len);
void DS18B20_MasterReceiveData(uint8_t *RxBuffer, uint8_t len);

void DS18B20_MasterGenerateWriteTimeSlot(uint8_t WriteValue);
uint8_t DS18B20_MasterGenerateReadTimeSlot(void);

/*
 * Temperature conversion
 */
float DS18B20_ConvertTemp(uint8_t *TempBuffer);



#endif /* INC_DS18B20_TEMP_SENSOR_H_ */
