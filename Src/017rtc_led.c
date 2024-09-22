/*
 * 017rtc_led.c
 *
 *  Created on: Apr 26, 2024
 *      Author: butle
 */

#include <stdio.h>
#include "stm32f407vg.h"
#include "ds1307.h"
#include "lcd.h"

#define SYSTICK_TIM_CLK 		(16000000UL)


char *date_to_string(RTC_Date_t *Date);
char *time_to_string(RTC_Time_t *Time);
char *day_to_string(uint8_t Day);
void numberToString(uint8_t num, char *string);
void init_systick_timer(uint32_t tick_hz);

uint8_t TxOngoingFlag = RESET;
uint8_t RxOngoingFlag = RESET;

RTC_Date_t currDate;
RTC_Time_t currTime;


int main(void)
{

	printf("RTC test\n");

	lcd_init();

	lcd_print_string("RTC Test...");

	mDelay(2000);

	lcd_display_clear();

	lcd_display_return_home();


	if( DS1307_Init() )
	{
		printf("RTC failed\n");
		while(1);
	}

	currDate.day = TUESDAY;
	currDate.date = 30;
	currDate.month = 4;
	currDate.year = 24;

	currTime.hour = 7;
	currTime.minute = 50;
	currTime.second = 35;
	currTime.time_format = TIME_FORMAT_12HRS_PM;

	DS1307_SetCurrentTime(&currTime);
	DS1307_SetCurrentDate(&currDate);

	init_systick_timer(1);

	while(1)
		;
}


// Helper functions

char *date_to_string(RTC_Date_t *Date)
{
	//mm:dd:yy

	static char dateString[9];

	dateString[2] = '/';
	dateString[5] = '/';

	numberToString(Date -> month, dateString);
	numberToString(Date -> date , &dateString[3]);
	numberToString(Date -> year , &dateString[6]);

	dateString[8] = '\0';

	return dateString;
}


char *time_to_string(RTC_Time_t *Time)
{
	//hh:mm:ss

	static char timeString[9];

	timeString[2] = ':';
	timeString[5] = ':';

	numberToString(Time -> hour, timeString);
	numberToString(Time -> minute, &timeString[3]);
	numberToString(Time -> second, &timeString[6]);

	timeString[8] = '\0';

	return timeString;
}


char *day_to_string(uint8_t Day)
{
	char *days[] = { "SUNDAY" , "MONDAY" , "TUESDAY" , "WEDNESDAY" , "THURSDAY" , "FRIDAY" , "SATURDAY" };

	return days[Day - 1];
}

void numberToString(uint8_t num, char *string)
{
	if( num < 10 )
	{
		string[0] = '0';
		string[1] = num + 48;
	}
	else if( num >= 10 && num < 100 )
	{
		string[0] = ( num / 10 ) + 48;
		string[1] = ( num % 10 ) + 48;
	}
}


void init_systick_timer(uint32_t tick_hz)
{
	uint32_t *pSRVR = (uint32_t*)0xE000E014;
	uint32_t *pSCSR = (uint32_t*)0xE000E010;

	uint32_t *pSCPR = (uint32_t*)0xE000ED20;

    /* calculation of reload value */
    uint32_t count_value = (SYSTICK_TIM_CLK/tick_hz)-1;

    //Clear the value of SVR
    *pSRVR &= ~(0x00FFFFFFFF);

    //load the value in to SVR
    *pSRVR |= count_value;

    //do some settings
    *pSCPR |= ( 0xA << 28 );	//Set priority of exception handler
    *pSCSR |= ( 1 << 1); 		//Enables SysTick exception request:
    *pSCSR |= ( 1 << 2);  		//Indicates the clock source, processor clock source

    //enable the systick
    *pSCSR |= ( 1 << 0); //enables the counter

}


void I2C_ApplicationEventCallBack(I2C_Handle_t *pI2CHandle, uint8_t AppEvent)
{
	if( AppEvent == I2C_EV_TX_COMPLETE )
	{
		TxOngoingFlag = RESET;
	}
	else if( AppEvent == I2C_EV_RX_COMPLETE )
	{
		RxOngoingFlag = RESET;
	}
}


void SysTick_Handler(void)
{
	DS1307_GetCurrentTime(&currTime);

	lcd_set_cursor(1,1);

	/*	Print time */

	char *am_pm;

	if(currTime.time_format != TIME_FORMAT_24HRS)
	{
		am_pm = currTime.time_format ? "PM" : "AM";
		printf("Current time = %s %s\n", time_to_string(&currTime), am_pm);					// ex: 02:45:41 PM
		lcd_print_string(time_to_string(&currTime));
		lcd_print_string(am_pm);
	}
	else
	{
		printf("Current time = %s\n", time_to_string(&currTime));							// ex: 14:45:41
		lcd_print_string(time_to_string(&currTime));
	}



	DS1307_GetCurrentDate(&currDate);

	lcd_set_cursor(2,1);

	/*	Print date */

	printf("Current date = %s %s\n", date_to_string(&currDate), day_to_string(currDate.day));	// ex: 4/26/24 Friday
	lcd_print_string(date_to_string(&currDate));
	lcd_print_string(" ");
	lcd_print_string(day_to_string(currDate.day));
}
