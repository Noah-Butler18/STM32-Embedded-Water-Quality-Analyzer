/*
 * 021adc_tds_test.c
 *
 *  Created on: Aug 7, 2024
 *      Author: butle
 */


/*
 * Objective: Get a temperature-adjusted TDS value from sensors and send over to Arduino over i2c bus
 *
 * HARDWARE CONNECTIONS:
 * 		DS18B20
 * 			VCC <-> 3.3V
 * 			GND <-> GND
 * 			DQ <-> PA3 (STM32) (hanging off 4.7kOhm pull up resistor connected to +Vdd)
 *
 * 		TDS sensor
 * 			VCC <-> 3.3V
 * 			GND <-> GND
 * 			A <-> PA1 (STM32)
 *
 * 		Turbidity sensor
 * 			VCC <-> 5V
 * 			GND <-> GND
 * 			A <-> PA2 (STM32)
 *
 * 		STM32
 *			PA3 <-> DQ (DS18B20) (hanging off 4.7kOhm pull up resistor connected to +Vdd)
 *			PA1 <-> A (TDS sensor)
 *			PB6 <-> SCLK (i2c to Arduino) ~~~ Use 5V to 3.3V logic level converter to interface between the 2 boards~~~
 *			PB7 <-> SDA (i2c to Arduino) ~~~ Use 5V to 3.3V logic level converter to interface between the 2 boards~~~
 *
 *		Arduino
 *			A5 <-> SCLK ~~~ Use 5V to 3.3V logic level converter to interface between the 2 boards~~~
 *			A4 <-> SDA ~~~ Use 5V to 3.3V logic level converter to interface between the 2 boards~~~
 *
 *		LOGIC LEVEL CONVERTER
 *			HV <-> +5V (STM32)
 * 			LV <-> +3.3V (STM32)
 * 			GND <-> GND
 * 			HV1 <-> A5 (Arduino)
 *			HV2 <-> A4 (Arduino)
 *			LV1 <-> PB6 (STM32)
 *			LV2 <-> PB7 (STM32)
 */

#include "stm32f407vg.h"
#include "ds18b20_temp_sensor.h"

#define NUM_OF_ANALOG_CONVERSIONS				2				//TDS and Turbidity

ADC_Handle_t pADC1Handle;
GPIO_Handle_t pGPIOAHandle;
GPIO_Handle_t GPIOi2cPins;
I2C_Handle_t i2c1;

//1-wire DSB18B20 global variables
uint8_t BufferOneWireRawTemperature[2];
uint8_t BufferOneWireCommands;
float Temperature = 0;

//Common ADC global variables
uint16_t BufferADCValues[NUM_OF_ANALOG_CONVERSIONS] = {0,0};
__vo uint8_t ADCSequenceIndex = 1;

//TDS ADC global variables
__vo uint16_t BufferADCTDSValue;
__vo float VoltsTDS;
__vo uint16_t TDS = 0;
__vo float TDSTemperatureCompensationCoefficient;

//i2c global variables
uint8_t BufferDataToArduino[6];
uint8_t SlaveAddr = 0x68;
uint8_t Len;

//Turbidity global variables
__vo uint16_t BufferADCTurbidityValue;
__vo float VoltsTurbidity;
__vo float Turbidity = 0;

//New values ready to display flag
__vo uint8_t NewValuesReady = 0;

extern void initialise_monitor_handles(void);
void I2C_MasterSendDataToArduino(void);
void DS18B20_MasterGetTemperature(uint8_t *BufferCommands, uint8_t *BufferReceiveTemperature);
void initialize_i2c(void);
void initialize_GPIO(void);
void initialize_ADC(void);
uint16_t TDS_ConvertVoltageToPPM(__vo float Voltage, __vo float TemperatureCompensation);
uint16_t TDS_CalibratePPM(uint16_t UncalibratedTDSPPM);
float Turbidity_ConvertVoltageToPercentage(__vo float Voltage);
void I2C_ConvertTurbidityPercentageToBytes(float TurbidityPercentage, uint8_t *Bufferi2c);
void I2C_ConvertTDSPPMToBytes(uint16_t TDSPPM, uint8_t *Bufferi2c);

int main(void)
{
	/************************ Semi-hosting INIT ***************/
	initialise_monitor_handles();

	printf("Application starting...\n");

	/************************ DS18B20 INIT ***************/
	DS18B20_Config();

	/************************ GPIO INIT ***************/
	initialize_GPIO();

	/************************ ADC INIT ***************/
	initialize_ADC();

	/************************ i2c INIT ***************/
	initialize_i2c();

	/************************ ADC INTERRUPT INIT ***************/
	ADC_IRQInterruptConfig(IRQ_NO_ADC, ENABLE);
	ADC_IRQPriorityConfig(IRQ_NO_ADC, NVIC_IRQ_PRIO_1 );

	/************************ TIM INTERRUPT INIT ***************/
	TIM2_5_IRQInterruptConfig(IRQ_NO_TIM2, ENABLE );
	TIM2_5_IRQPriorityConfig(IRQ_NO_TIM2, NVIC_IRQ_PRIO_2 );


	float freq = 0.75;
	TIM2_5_SetIT(TIM2, freq);

	while(1)
	{
		//When TIM2 interrupt is triggered, the ISR handles retrieving temperature reading and enables ADC
			//When ADC ISR is triggered, it handles retrieving DR value from analog pin (TDS). It also handles i2c communications to arduino

		//While not in an ISR values print to console every so often
		while( !NewValuesReady )
				;

		NewValuesReady = 0;

		printf("Sent:  | 0x%X | 0x%X | 0x%X | 0x%X | 0x%X | 0x%X |\n", BufferDataToArduino[0], BufferDataToArduino[1], BufferDataToArduino[2], BufferDataToArduino[3], BufferDataToArduino[4], BufferDataToArduino[5] );
		printf("Current water readings: Temp - %.2f°C   TDS - %dppm   Turbidity - %.2f%% \n", Temperature, TDS, Turbidity);
	}
}




void TIM2_IRQHandler(void)
{
	TIM2_5_IRQHandling(TIM2);

	//1. Get Temperature From DS18B20
	DS18B20_MasterGetTemperature(&BufferOneWireCommands, BufferOneWireRawTemperature);

	//2. Update global temperature variable
	Temperature = DS18B20_ConvertTemp( BufferOneWireRawTemperature);

	//3. Enable ADC start up code
	ADC_EnableIT(&pADC1Handle, BufferADCValues, (pADC1Handle.ADC_Config.ADC_Seq_Len) );
}

void ADC_IRQHandler(void)
{
	//1. Read value(s) in from TDS sensor first, then turbidity sensor
	ADC_IRQHandling(&pADC1Handle);

	//2. At this point in program flow, all data conversions are done.
	//   Calculate display values based off of voltages and then store all in buffer to be sent to Arduino via i2c
	if( ADCSequenceIndex > NUM_OF_ANALOG_CONVERSIONS )
	{
		//2.1 Reset global sequence index
		ADCSequenceIndex = 1;

		//2.2 Update global 1-wire variables - Temperature
		TDSTemperatureCompensationCoefficient = 1.0 + (0.02 * (Temperature - 25.0));

		//2.3 Update global ADC variables - TDS
		BufferADCTDSValue = BufferADCValues[0];
		VoltsTDS = BufferADCTDSValue * 3.3 / 4095.0;
		TDS = TDS_ConvertVoltageToPPM(VoltsTDS, TDSTemperatureCompensationCoefficient);
		TDS = TDS_CalibratePPM(TDS);

		//2.4 Update global ADC variables - Turbidity
		BufferADCTurbidityValue = BufferADCValues[1];
		VoltsTurbidity = BufferADCTurbidityValue * 3.3 / 4095.0;
		Turbidity = Turbidity_ConvertVoltageToPercentage(VoltsTurbidity);

		//2.5 fit Temperature, TDS, and turbidity data into buffer to be sent over i2c bus
		//Structure of bytes of message: | 1) Temperature MSB | 2) Temperature LSB | 3) TDS MSB | 4) TDS LSB | 5) Turbidity (%)
		BufferDataToArduino[0] = BufferOneWireRawTemperature[0];
		BufferDataToArduino[1] = BufferOneWireRawTemperature[1];
		I2C_ConvertTDSPPMToBytes(TDS, &BufferDataToArduino[2]);
		I2C_ConvertTurbidityPercentageToBytes(Turbidity, &BufferDataToArduino[4]);

		//2.6 Update global flag - New values ready to be printed by STM32
		NewValuesReady = 1;

		//2.7 Send all data to Arduino
		I2C_MasterSendDataToArduino();
	}
}





void I2C_MasterSendDataToArduino(void)
{
	//1. enable peripheral hardware
	I2C_PeripheralControl(i2c1.pI2Cx, ENABLE);

	//2. start comms, send address phase, then send all information. Close comms once finished
	Len = ( sizeof(BufferDataToArduino)/sizeof(BufferDataToArduino[0]) );
	I2C_MasterSendData(&i2c1, BufferDataToArduino, Len, SlaveAddr, 0);

	//3. Disable the I2C peripheral once communication is over
	I2C_PeripheralControl(i2c1.pI2Cx,DISABLE);
}


void DS18B20_MasterGetTemperature(uint8_t *BufferCommands, uint8_t *BufferReceiveTemperature)
{
	//1. Master initiates communication sequence (Master Tx) and waits for presence pulse from DS18B20 (Master Rx)
	DS18B20_MasterSendInitializeSequence();

	//2. Master sends skip ROM command since there is only 1 slave on bus (Master Tx)
	*BufferCommands = MASTER_COMMAND_SKIP_ROM;
	DS18B20_MasterSendData( BufferCommands , 1);

	//3. Master sends convert T command to make DS18B20 start converting (Master Tx)
	*BufferCommands = MASTER_COMMAND_CONVERT_T;
	DS18B20_MasterSendData( BufferCommands , 1);

	//4. Master continuously sends read time slots to gauge when DS18B20 is finished converting temp (Master Tx)
	//5. Master waits until it receives 1 '1' on the bus notifying it the temperature is ready (Master Rx)
	while( !DS18B20_MasterGenerateReadTimeSlot() )
		;

	//6. Master initiates another communication sequence (Master Tx) and waits for presence pulse from DS18B20 (Master Rx)
	DS18B20_MasterSendInitializeSequence();

	//7. Master sends skip ROM command since there is only 1 slave on bus (Master Tx)
	*BufferCommands = MASTER_COMMAND_SKIP_ROM;
	DS18B20_MasterSendData( BufferCommands , 1);

	//8. Master sends read scratch pad command (Master Tx)
	*BufferCommands = MASTER_COMMAND_READ_SCRATCHPAD;
	DS18B20_MasterSendData( BufferCommands , 1);

	//9. Master waits until 2 bytes of data have been sent (Master Rx)
	DS18B20_MasterReceiveData(BufferReceiveTemperature, 2);

	//10. Master sends a reset pulse to stop reading scratch pad (the temperature is only the first 2 bytes) is (Master Tx)
	DS18B20_MasterSendInitializeSequence();
}


void initialize_GPIO(void)
{
	//Initialize the ADC input pins - PA1, PA2
	memset(&pGPIOAHandle,0,sizeof(pGPIOAHandle));

	pGPIOAHandle.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;		//GPIO pin in analog mode for ADC
	pGPIOAHandle.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;			//GPIO pin in analog mode for TDS ADC (PA1 is free IO)
	pGPIOAHandle.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;		//GPIO output type - don't care
	pGPIOAHandle.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;		//GPIO output speed - don't care
	pGPIOAHandle.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;		//No pull up or pull down resistor

	pGPIOAHandle.pGPIOx = GPIOA;										//Using GPIOA peripheral

	GPIO_Init(&pGPIOAHandle);

	pGPIOAHandle.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;			//GPIO pin in analog mode for Turbidity ADC (PA2 is free IO)

	GPIO_Init(&pGPIOAHandle);

	//Initialize I2C pins of the master STM32
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

void initialize_ADC(void)
{
	memset(&pADC1Handle,0,sizeof(pADC1Handle));

	pADC1Handle.ADC_Config.ADC_ClkPrescaler = ADC_CLK_DIV_2; 				//ADC clk = 8MHz
	pADC1Handle.ADC_Config.ADC_Resolution = ADC_RES_12BITS;					//DR resolution = 12 bits
	pADC1Handle.ADC_Config.ADC_DataAlignment = ADC_RIGHT_ALIGNMENT;			//DR alignment = right
	pADC1Handle.ADC_Config.ADC_Mode = ADC_SINGLE_CONVERSION_MODE;				//ADC mode
	pADC1Handle.ADC_Config.ADC_SamplingTime[ADC_IN1] = ADC_SMP_480_CYCLES;	//Channel 1 sampling time = 3 cycles
	pADC1Handle.ADC_Config.ADC_SamplingTime[ADC_IN2] = ADC_SMP_480_CYCLES;	//Channel 2 sampling time = 3 cycles
	pADC1Handle.ADC_Config.ADC_AWDHT = 0xFFF;								//High voltage threshold: digital 4095 | analog 3.3V /// digital 2048 | analog 1.65V
	pADC1Handle.ADC_Config.ADC_AWDLT = 0x0;									//Low voltage threshold: digital 0 | analog 0V /// digital 2048 | analog 1.65V
	pADC1Handle.ADC_Config.ADC_Seq_Len = NUM_OF_ANALOG_CONVERSIONS;			//channel conversion sequence length = 1 channel
	pADC1Handle.ADC_Config.ADC_Seq_Order[0] = ADC_IN1;						//ADC channel sequence order = 1) ADC_IN1 - TDS sensor
	pADC1Handle.ADC_Config.ADC_Seq_Order[1] = ADC_IN2;						//ADC channel sequence order = 2) ADC_IN2 - Turbidity sensor

	pADC1Handle.pADCx = ADC1;												//Using ADC1 peripheral

	ADC_Init(&pADC1Handle);
}

uint16_t TDS_ConvertVoltageToPPM(__vo float Voltage, __vo float TemperatureCompensation)
{
	float CompensatedVoltage;
	uint16_t TDSppm;

	CompensatedVoltage = Voltage / TemperatureCompensation; //Temperature Compensation

	TDSppm = (133.42*CompensatedVoltage*CompensatedVoltage*CompensatedVoltage - 255.86*CompensatedVoltage*CompensatedVoltage + 857.39*CompensatedVoltage) * 0.5; //convert voltage value to tds value

	return TDSppm;
}

uint16_t TDS_CalibratePPM(uint16_t UncalibratedTDSPPM)
{
	//Calibration value based off of real life data measuring known TDS value vs sensor TDS value
	//Calibration relationship is a linear function based off of 2 real life data points:
		//(7ppm measured,0ppm known) ; (128ppm measured,707ppm known)
	//This yields the rough relationship: CorrectedValue = 5.84 * MeasuredValue − 41
	if( (5.84 * (int16_t)UncalibratedTDSPPM - 41) < 0 )
	{
		return 0;
	}
	else
		return (5.84 * UncalibratedTDSPPM - 41);
}

float Turbidity_ConvertVoltageToPercentage(__vo float Voltage)
{
	//0V = 3.5% turbidity, 1.51V = 0% turbidity

	//Due to error within the measurements, guard band the readings and say anything above the possible error is 100% clear
	if( Voltage > 1.53 )
	{
		return 0;
	}
	else
	{
		float TurbidityPercentage;
		float Step = 3.5 / 1.53; //3.5% is the highest reading capable on sensor, 1.51V is the theoretical highest value the ADC can read from this sensor

		TurbidityPercentage = Voltage * Step;
		TurbidityPercentage = 3.5 - TurbidityPercentage;
		return TurbidityPercentage;
	}
}

void I2C_ConvertTurbidityPercentageToBytes(float TurbidityPercentage, uint8_t *Bufferi2c)
{
	//Takes percentage and stores it into an 8-bit buffer of size 2. The first byte is the whole number part, the second byte is the fractional part
	//i.e.,   2.5%  -->  | 00000010 | 00000101 |

	uint16_t temp;

	temp = TurbidityPercentage*10;

	//Load MSB of buffer
	*(++Bufferi2c) = temp % 10;

	temp /= 10;
	//Load LSB of buffer
	*(--Bufferi2c) = temp % 10;
}

void I2C_ConvertTDSPPMToBytes(uint16_t TDSPPM, uint8_t *Bufferi2c)
{
	//First byte is MSB, second byte is LSB
	*Bufferi2c = (TDSPPM >> 8) & 0xFF;
	*(++Bufferi2c) = TDSPPM & 0xFF;
}


void ADC_ApplicationEventCallBack(ADC_Handle_t *pADCHandle, uint8_t AppEvent)
{
	//User implementation of ADC_ApplicationEventCallBack API

	if( AppEvent == ADC_ERROR_STRT )
	{
		//Application attempting starting conversion with one already in progress

		printf("ADC repeated start condition detected - suspending program...\n");

		while(1);
	}

	if( AppEvent == ADC_EVENT_AWD )
	{
		//Analog watch dog threshold triggered

		if( BufferADCValues[(pADCHandle->ADC_SeqLen) - 1] > ( pADCHandle->pADCx->HTR ) )
		{
			printf("Analog watch dog triggered - over voltage condition detected.\n");
		}
		else if( BufferADCValues[(pADCHandle->ADC_SeqLen) - 1] < ( pADCHandle->pADCx->LTR ) )
		{
			printf("Analog watch dog triggered - under voltage condition detected.\n");
		}

		float VoltsAWD = ( ( BufferADCValues[(pADCHandle->ADC_SeqLen) - 1] / 4095.0 ) * 3.3 );

		printf("\nCurrent voltage reading: %f\n\n", VoltsAWD);

		printf("Shutting down ADC...\n");
		ADC_PeripheralOnOffControl(pADCHandle->pADCx, DISABLE);

		while(1);
	}

	if( AppEvent == ADC_EVENT_OVR )
	{
		//Overrun triggered

		printf("Overrun condition detected - suspending program...\n");

		while(1);
	}

	if( AppEvent == ADC_EVENT_EOC )
	{
		//For multiple channels, need to be in single conversion mode for this program and will have to manually change channel selects

		//1. Stop the ADC after each conversion
		pADCHandle->pADCx->CR2 &= ~( 1 << ADC_CR2_ADON );

		//2. Increment global tracking flag to see where we are in conversion order
		++ADCSequenceIndex;

		//3. If we are done converting all channels, we need to reinitialize the order with user configuration values
		if( ADCSequenceIndex > NUM_OF_ANALOG_CONVERSIONS )
		{
			ADC_SequenceInit(pADCHandle);
		}
		else
		{
			//4. Change the channel selected to be converted in single channel mode to the next channel.
			pADCHandle->pADCx->SQR3 >>= 5;

			//5. Restart the ADC to keep converting channels
			ADC_StartADC(pADCHandle);
		}
	}

}
