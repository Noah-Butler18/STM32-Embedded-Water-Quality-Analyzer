/*
 * ds1307.c
 *
 *  Created on: Apr 23, 2024
 *      Author: butle
 */


#include "ds1307.h"

static void DS1307_I2CPinConfig(void);
static void DS1307_I2CConfig(void);
static void DS1307_I2CInterruptConfig(void);
static void DS1307_Write(uint8_t reg_address, uint8_t *pValue, uint8_t size);
static uint8_t DS1307_ReadByte(uint8_t reg_address);
extern void I2C_ApplicationEventCallBack(I2C_Handle_t *pI2CHandle, uint8_t AppEvent);
static uint8_t ConvertBinaryToBCD(uint8_t DecimalNum);
static uint8_t ConvertBCDToBinary(uint8_t BCDNum);

I2C_Handle_t g_DS1307I2CHandle;

extern __vo uint8_t TxOngoingFlag;
extern __vo uint8_t RxOngoingFlag;

/*
 * DS1307 chip init.
 * If function returns 1: CH bit = 1; initialization of clock failed
 * If function returns 0: DS1307 system clock is on; initialization of clock successful
 */

uint8_t DS1307_Init(void)
{
	//1. Initialize I2C pins
	DS1307_I2CPinConfig();


	//2. Initialize I2C peripheral
	DS1307_I2CConfig();


	//3. Enable I2C peripheral
	I2C_PeripheralControl(g_DS1307I2CHandle.pI2Cx, ENABLE);


	//4. Enable & configure I2C peripheral interrupts
	DS1307_I2CInterruptConfig();



	//Initially, the oscillator (system clock) will not be running on DS1307 chip (refer to DS "CLOCK AND CALENDAR")
	//5. Enable the DS1307 oscillator (CH bit in seconds register)

	uint8_t DS1307RegAddress = DS1307_ADDR_SEC;
	uint8_t WriteData[] = {0x00};

	DS1307_Write(DS1307RegAddress, WriteData, 2);

	//6. Read back clock halt bit to confirm it is set to 0

	uint8_t DS1307_ClockState;

	DS1307_ClockState = DS1307_ReadByte(DS1307RegAddress);

	return ( ( DS1307_ClockState >> 7 ) & 0x1 ) ;
}


/*
 * Input set-up information
 */

void DS1307_SetCurrentTime(RTC_Time_t* pTime)
{
	//1. Set register pointer to seconds register, after each register is written to it will automatically increment to the next register
	//2. In same transmission, load registers with rest of time information

	uint8_t RegAddress = DS1307_ADDR_SEC;
	uint8_t currSecond = ( ConvertBinaryToBCD( pTime -> second ) ) & ( ~( 1 << 7 ) ); //Make sure CH bit is not set to 1 accidentally
	uint8_t currMinute = ConvertBinaryToBCD( pTime -> minute );
	uint8_t currHour = ConvertBinaryToBCD( pTime -> hour );


	//Write desired time format into hour register

	if( ( pTime -> time_format ) == TIME_FORMAT_12HRS_AM )
	{
		//When bit 6 of hour register is high, 12hr mode is selected

		currHour |= 1 << 6;

		//When bit 5 of the hour register is low, AM is indicated when in AM/PM mode

		currHour &= ~( 1 << 5 );
	}
	else if( ( pTime -> time_format ) == TIME_FORMAT_12HRS_PM )
	{
		//When bit 6 of hour register is high, 12hr mode is selected

		currHour |= 1 << 6;

		//When bit 5 of the hour register is high, PM is indicated when in AM/PM mode

		currHour |= ( 1 << 5 );
	}
	else if( ( pTime -> time_format ) == TIME_FORMAT_24HRS )
	{
		//When bit 6 of hour register is low, 24hr mode is selected

		currHour &= ~( 1 << 6 );
	}


	uint8_t WriteCurrTime[3] = { currSecond , currMinute , currHour };
	uint8_t TransmitSize = ( sizeof(WriteCurrTime) ) / ( sizeof(WriteCurrTime[0]) ) + 1; //Adding 1 to include address byte in data-to-transmit size

	DS1307_Write(RegAddress, WriteCurrTime, TransmitSize);
}


void DS1307_SetCurrentDate(RTC_Date_t* pDate)
{
	//1. Set register pointer to day register, after each register is written to it will automatically increment to the next register
	//2. In same transmission, load registers with rest of date information

	uint8_t RegAddress = DS1307_ADDR_DAY;
	uint8_t currDay = ConvertBinaryToBCD( pDate -> day );
	uint8_t currDate = ConvertBinaryToBCD( pDate -> date );
	uint8_t currMonth = ConvertBinaryToBCD( pDate -> month );
	uint8_t currYear = ConvertBinaryToBCD( pDate -> year );


	uint8_t WriteCurrDate[4] = { currDay , currDate , currMonth , currYear };
	uint8_t TransmitSize = ( sizeof(WriteCurrDate) ) / ( sizeof(WriteCurrDate[0]) ) + 1; //Adding 1 to include address byte in data-to-transmit size

	DS1307_Write(RegAddress, WriteCurrDate, TransmitSize);
}


/*
 * Retrieve output timing information
 */

void DS1307_GetCurrentTime(RTC_Time_t* pTime)
{
	//Grab seconds, minutes, hours, and time format info
	uint8_t second = ( ( DS1307_ReadByte(DS1307_ADDR_SEC) ) & 0x7F ); 		//Only last 7 bits important
	pTime->second = ConvertBCDToBinary(second);

	uint8_t minute = ( ( DS1307_ReadByte(DS1307_ADDR_MIN) ) & 0x7F );  		//Only last 7 bits important
	pTime->minute = ConvertBCDToBinary(minute);

	uint8_t hour = ( ( DS1307_ReadByte(DS1307_ADDR_HOUR) ) & 0x7F ); 		//Only last 7 bits important

	//If hour is in AM/PM format, need to update time_format element of time structure

	if( ( hour >> 6 ) & 0x1 )
	{
		// AM/PM is time format

		if( ( hour >> 5 ) & 0x1 )
		{
			// PM is indicated
			pTime->time_format = TIME_FORMAT_12HRS_PM;
		}
		else
		{
			// AM is indicated
			pTime->time_format = TIME_FORMAT_12HRS_AM;
		}

		// In AM/PM time format, only last 5 bits are counted
		hour &= 0x1F;
	}
	else
	{
		// 24hr is time format
		pTime->time_format = TIME_FORMAT_24HRS;

		// In 24hr time format, bits 7 and 6 of hour register are 0, so no masking is necessary
	}

	pTime->hour = ConvertBCDToBinary(hour);
}


void DS1307_GetCurrentDate(RTC_Date_t* pDate)
{
	//Grab day, date, month, and year info
	//Values will be in BCD initially and have to be converted to binary

	uint8_t day = DS1307_ReadByte(DS1307_ADDR_DAY) & 0x7; 		//Only last 3 bits important
	pDate -> day = ConvertBCDToBinary(day);

	uint8_t date = DS1307_ReadByte(DS1307_ADDR_DATE) & 0x3F;	//Only last 6 bits important
	pDate -> date = ConvertBCDToBinary(date);

	uint8_t month = DS1307_ReadByte(DS1307_ADDR_MONTH) & 0x1F; 	//Only last 5 bits important
	pDate -> month = ConvertBCDToBinary(month);

	uint8_t year = DS1307_ReadByte(DS1307_ADDR_YEAR);
	pDate -> year = ConvertBCDToBinary(year);
}




/*************************** Helper functions ****************************/


static void DS1307_I2CPinConfig(void)
{
	GPIO_Handle_t i2c_pins;

	memset(&i2c_pins,0,sizeof(i2c_pins));

	//Configure SDA pin
	i2c_pins.pGPIOx = DS1307_I2C_GPIO_PORT;

	i2c_pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	i2c_pins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_MODE_AF4;
	i2c_pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	i2c_pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;		//SDA line using external PUs
	i2c_pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH; 		//4nS t_fall when pulling line down
	i2c_pins.GPIO_PinConfig.GPIO_PinNumber = DS1307_I2C_SDA_PIN;

	GPIO_Init(&i2c_pins);

	//Configure SCL pin
	i2c_pins.GPIO_PinConfig.GPIO_PinPuPdControl = DS1307_I2C_PUPD;

	i2c_pins.GPIO_PinConfig.GPIO_PinNumber = DS1307_I2C_SCL_PIN;

	GPIO_Init(&i2c_pins);
}


static void DS1307_I2CConfig(void)
{
	g_DS1307I2CHandle.pI2Cx = DS1307_I2C;

	g_DS1307I2CHandle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
	g_DS1307I2CHandle.I2C_Config.I2C_DeviceAddress = DS1307_I2C_ADDR;
	g_DS1307I2CHandle.I2C_Config.I2C_SCLSpeed = DS1307_I2C_SPEED;

	I2C_Init(&g_DS1307I2CHandle);
}


static void DS1307_I2CInterruptConfig(void)
{
	I2C_IRQInterruptConfig(DS1307_I2C_EV_IRQ_NO, ENABLE);
	I2C_IRQInterruptConfig(DS1307_I2C_ER_IRQ_NO, ENABLE);

	I2C_IRQPriorityConfig(DS1307_I2C_EV_IRQ_NO, DS1307_I2C_EV_IRQ_PRIORITY);
	I2C_IRQPriorityConfig(DS1307_I2C_ER_IRQ_NO, DS1307_I2C_ER_IRQ_PRIORITY);
}


static void DS1307_Write(uint8_t reg_address, uint8_t *pValue, uint8_t size)
{
	//NOTE: "size" argument should include the register address byte

	//Create array of address as first byte and proceeding data bytes after
	uint8_t SendData[size];

	SendData[0] = reg_address;
	for(uint32_t i = 1 ; i < size ; i++)
	{
		SendData[i] = *pValue;
		pValue++;
	}

	TxOngoingFlag = SET;

	while( I2C_MasterSendDataIT(&g_DS1307I2CHandle, SendData, size, g_DS1307I2CHandle.I2C_Config.I2C_DeviceAddress, I2C_DISABLE_SR) != I2C_READY )
		;

	//wait for message to be fully transmitted before moving onto reading the data from the registers we have written to (implemented by user I2C_ApplicationEventCallBack function)
	while( TxOngoingFlag != RESET )
		;
}


static uint8_t DS1307_ReadByte(uint8_t reg_address)
{
	//1. Set address pointer of DS1307 registers to the desired register that is to be read (send reg_address byte first to set the register pointer)
		//Do a repeated start after word address is sent

	TxOngoingFlag = SET;

	while( I2C_MasterSendDataIT(&g_DS1307I2CHandle, &reg_address, 1, g_DS1307I2CHandle.I2C_Config.I2C_DeviceAddress, I2C_ENABLE_SR) != I2C_READY )
		;

	while( TxOngoingFlag != RESET )
		;
	//2. Read the address pointed to in the DS1307 registers and store in local variable to return to the main


	uint8_t RxByte;

	RxOngoingFlag = SET;

	while( I2C_MasterReceiveDataIT(&g_DS1307I2CHandle, &RxByte, 1, g_DS1307I2CHandle.I2C_Config.I2C_DeviceAddress, I2C_DISABLE_SR) != I2C_READY)
		;

	//3. Wait for read data transfer (implemented by user I2C_ApplicationEventCallBack function)
	while( RxOngoingFlag != RESET )
		;

	return RxByte;
}

static uint8_t ConvertBinaryToBCD(uint8_t DecimalNum)
{
	//Function that converts a decimal number to binary-coded decimal
	//MUST INPUT A DECIMAL NUMBER

	uint8_t Reg = 0;
	uint8_t ShiftAmt = 0;

	while( DecimalNum > 0 )
	{
		Reg |= ( DecimalNum % 10 ) << ShiftAmt;
		DecimalNum /= 10;
		ShiftAmt += 4;
	}

	return Reg;
}


static uint8_t ConvertBCDToBinary(uint8_t BCDNum)
{
	//Function that converts a BCD number to decimal
	//MUST INPUT A BCD NUMBER

	uint8_t Reg = 0;
	uint8_t m , n;


	m = (uint8_t) ( ( BCDNum >> 4 ) * 10 );
	n = BCDNum & (uint8_t) 0x0F;

	Reg = m + n;

	return Reg;
}


/*
 * Interrupt implementation
 * NOTE: TO BE CHANGED BASED OFF OF WHAT I2Cx PERIPHERAL IS BEING USED FOR COMMUNICATION
 */

void I2C1_EV_IRQHandler(void)
{
	I2C_EV_IRQHandling(&g_DS1307I2CHandle);
}

