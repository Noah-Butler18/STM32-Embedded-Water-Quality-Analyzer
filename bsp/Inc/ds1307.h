/*
 * ds1307.h
 *
 *  Created on: Apr 23, 2024
 *      Author: butle
 */

#ifndef DS1307_H_
#define DS1307_H_

#include "stm32f407vg.h"

/*
 * Application configurable items
 */
#define DS1307_I2C								I2C1
#define DS1307_I2C_GPIO_PORT					GPIOB
#define DS1307_I2C_SDA_PIN						GPIO_PIN_NO_7
#define DS1307_I2C_SCL_PIN						GPIO_PIN_NO_6
#define DS1307_I2C_SPEED						I2C_SCL_SPEED_SM		//Fixed value - only speed supported by RTC chip. Should not be changed
#define DS1307_I2C_PUPD							GPIO_PIN_PU				//Will be using internal PU resistors for I2C bus
#define DS1307_I2C_EV_IRQ_NO					IRQ_NO_I2C1_EV
#define DS1307_I2C_ER_IRQ_NO					IRQ_NO_I2C1_ER
#define DS1307_I2C_EV_IRQ_PRIORITY				NVIC_IRQ_PRIO_0
#define DS1307_I2C_ER_IRQ_PRIORITY				NVIC_IRQ_PRIO_1




/*
 * Register addresses of DS1307 chip
 */
#define DS1307_ADDR_SEC							0x00U					//Base address of seconds register
#define DS1307_ADDR_MIN							0x01U					//Base address of minutes register
#define DS1307_ADDR_HOUR						0x02U					//Base address of hours register
#define DS1307_ADDR_DAY							0x03U					//Base address of days register	(possible values: 1-7)
#define DS1307_ADDR_DATE						0x04U					//Base address of date register	(possible values: 1-31)
#define DS1307_ADDR_MONTH						0x05U					//Base address of month register
#define DS1307_ADDR_YEAR						0x06U					//Base address of year register

#define DS1307_ADDR_CONTROL						0x07U					//Base address of DS1307 control register

/*
 * Time format macro
 */
#define TIME_FORMAT_12HRS_AM					0						//value of hour register bit 5 if hour is to be interpreted as AM
#define TIME_FORMAT_12HRS_PM					1						//value of hour register bit 5 if hour is to be interpreted as PM
#define TIME_FORMAT_24HRS						2

/*
 * Slave address macro for DS1307 chip
 */
#define DS1307_I2C_ADDR							0x68U					//Address derived from DS1307 data sheet - this is a hard-coded value in the chip to recognize messages


/*
 * Day of the week macros (user defined, per DS1307 data sheet [Left to software to interpret, not hardware])
 */
#define SUNDAY									0x1
#define MONDAY									0x2
#define TUESDAY									0x3
#define WEDNESDAY								0x4
#define THURSDAY								0x5
#define FRIDAY									0x6
#define SATURDAY								0x7

/*
 * date data structure
 */
typedef struct
{
	uint8_t	day;
	uint8_t	date;
	uint8_t	month;
	uint8_t	year;
}RTC_Date_t;

/*
 * time data structure
 */
typedef struct
{
	uint8_t	second;
	uint8_t	minute;
	uint8_t	hour;
	uint8_t	time_format;
}RTC_Time_t;

/******************************************************************************************
 *								APIs supported by this driver
 *		 For more information about the APIs check the function definitions
 ******************************************************************************************/

/*
 * DS1307 chip init
 */
uint8_t DS1307_Init(void);

/*
 * Input set-up information
 */
void DS1307_SetCurrentTime(RTC_Time_t* pTime);
void DS1307_SetCurrentDate(RTC_Date_t* pDate);

/*
 * Retrieve output timing information
 */
void DS1307_GetCurrentTime(RTC_Time_t* pTime);
void DS1307_GetCurrentDate(RTC_Date_t* pDate);





#endif /* DS1307_H_ */
