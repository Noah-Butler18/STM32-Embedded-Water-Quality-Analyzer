/*
 * 024_temp_sensor_test.c
 *
 *  Created on: Aug 13, 2024
 *      Author: butle
 */

/*
 * 1. Set GPIO pin as data line
 * 2. Send and receive data over the data line using 1-wire interface
 * 3. Save temperature into a global variable and print to console
 */

#include "stm32f407vg.h"
#include "ds18b20_temp_sensor.h"

uint8_t TempBuffer[2];
uint8_t TxBuf;
float Temperature;

extern void initialise_monitor_handles(void);

int main(void)
{
	//Semi-hosting enable
	initialise_monitor_handles();

	printf("Application starting...\n");

	DS18B20_Config();

	while(1)
	{
		//1. Master initiates communication sequence (Master Tx) and waits for presence pulse from DS18B20 (Master Rx)
		DS18B20_MasterSendInitializeSequence();

		//2. Master sends skip ROM command since there is only 1 slave on bus (Master Tx)
		TxBuf = MASTER_COMMAND_SKIP_ROM;
		DS18B20_MasterSendData( &TxBuf , 1);

		//3. Master sends convert T command to make DS18B20 start converting (Master Tx)
		TxBuf = MASTER_COMMAND_CONVERT_T;
		DS18B20_MasterSendData( &TxBuf , 1);

		//4. Master continuously sends read time slots to gauge when DS18B20 is finished converting temp (Master Tx)
		//5. Master waits until it receives 1 '1' on the bus notifying it the temperature is ready (Master Rx)
		while( !DS18B20_MasterGenerateReadTimeSlot() )
			;

		//6. Master initiates another communication sequence (Master Tx) and waits for presence pulse from DS18B20 (Master Rx)
		DS18B20_MasterSendInitializeSequence();

		//7. Master sends skip ROM command since there is only 1 slave on bus (Master Tx)
		TxBuf = MASTER_COMMAND_SKIP_ROM;
		DS18B20_MasterSendData( &TxBuf , 1);

		//8. Master sends read scratch pad command (Master Tx)
		TxBuf = MASTER_COMMAND_READ_SCRATCHPAD;
		DS18B20_MasterSendData( &TxBuf , 1);

		//9. Master waits until 2 bytes of data have been sent (Master Rx)
		DS18B20_MasterReceiveData(TempBuffer, 2);

		//10. Master sends a reset pulse to stop reading scratch pad (the temperature is only the first 2 bytes) is (Master Tx)
		DS18B20_MasterSendInitializeSequence();

		//11. Program displays temperature onto console
		Temperature = DS18B20_ConvertTemp(TempBuffer);
		printf("The temperature in my room is currently %f degrees C\n", Temperature);

		//12. Wait some seconds to do it again ad infinitum
		TIM2_5_Delay(TIM5, 2000000);
	}
}
