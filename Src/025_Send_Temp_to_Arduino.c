/*
 * 025_Send_Temp_to_Arduino.c
 *
 *  Created on: Aug 16, 2024
 *      Author: butle
 */

/*
 * 1. Use 024_temp_sensor_test.c to record data from temperature sensor
 * 2. Send temperature data to arduino using i2c
 * 3. Have Arduino print data onto console. Then, have it use the data in EC calculation.
 *
 * STM32 hardware connections:
 * 	i2c:
 * 		SCL -> PB6
 * 		SDA -> PB7
 *
 * 	1-wire:
 * 		DQ -> PA3
 *
 *
 * 	~~~ Use 5V to 3.3V logic level converter to interface between the 2 boards~~~
 *
 *
 * 	Arduino hardware connections:
 *	 i2c:
 * 		SCL -> A5
 * 		SDA -> A4
*/

#include "stm32f407vg.h"
#include "ds18b20_temp_sensor.h"

void initialize_gpios(void);
void initialize_i2c(void);
void I2C_MasterSendTemperature(void);
void DS18B20_MasterGetTemperature(void);


uint8_t TempBuffer[2];
uint8_t TxBuf;
float Temperature;

I2C_Handle_t i2c1;

extern void initialise_monitor_handles(void);


int main(void)
{

	//Semi-hosting enable
	initialise_monitor_handles();

	printf("Application starting...\n");


	//0.1 Configure the STM32 GPIO pins for I2C as well as button
	initialize_gpios();

	//0.2 Configure the STM32 I2C peripheral
	initialize_i2c();

	//0.3 Initialize 1-wire communication with value in ds18b20_temp_sensor.h header file
	DS18B20_Config();

	while(1)
	{
		//1. Get temp from DS18B20 and print on console
		DS18B20_MasterGetTemperature();
		Temperature = DS18B20_ConvertTemp(TempBuffer);
		printf("The temperature in my room is currently %f degrees C\n", Temperature);

		//2. Send temperature buffer to Arduino via i2c bus
		I2C_MasterSendTemperature();

		//3. Wait some seconds to do it again ad infinitum
		TIM2_5_Delay(TIM5, 1000000);
	}
}



void initialize_gpios(void)
{
	//Initialize I2C pins of the master STM32
	GPIO_Handle_t GPIOi2cPins;
	memset(&GPIOi2cPins,0,sizeof(GPIOi2cPins)); 		//sets each member element of the structure to zero. Avoids bugs caused by random garbage values in local variables upon first declaration

	GPIOi2cPins.pGPIOx = GPIOB;
	GPIOi2cPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	GPIOi2cPins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_MODE_AF4;
	GPIOi2cPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	GPIOi2cPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	GPIOi2cPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH; //4nS t_fall when pulling line down

		//SCL
	GPIOi2cPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&GPIOi2cPins);

		//SDA
	GPIOi2cPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&GPIOi2cPins);
}

void initialize_i2c(void)
{
	memset(&i2c1,0,sizeof(i2c1)); 		//sets each member element of the structure to zero. Avoids bugs caused by random garbage values in local variables upon first declaration

	i2c1.pI2Cx = I2C1;
	i2c1.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
	i2c1.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
	i2c1.I2C_Config.I2C_DeviceAddress = 0x01;
	i2c1.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;


	I2C_Init(&i2c1);
}

void I2C_MasterSendTemperature(void)
{
	//1. enable peripheral hardware
	I2C_PeripheralControl(i2c1.pI2Cx, ENABLE);

	//2. start comms, send address phase, then send all information. Close comms once finished
	uint8_t Len = ( sizeof(TempBuffer)/sizeof(TempBuffer[0]) );
	uint8_t SlaveAddr = 0x68;
	I2C_MasterSendData((&i2c1), TempBuffer, Len, SlaveAddr, 0);

	//3. Disable the I2C peripheral once communication is over
	I2C_PeripheralControl(i2c1.pI2Cx,DISABLE);
}

void DS18B20_MasterGetTemperature(void)
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
}

