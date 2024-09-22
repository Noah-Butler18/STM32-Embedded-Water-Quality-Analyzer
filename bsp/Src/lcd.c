/*
 * lcd.c
 *
 *  Created on: Apr 23, 2024
 *      Author: butle
 */

#include "lcd.h"

static void write_4_bits(uint8_t val);
static void lcd_enable(void);


void lcd_init(void)
{
	//1. Configure the GPIO pins which are used for LCD connections

	GPIO_Handle_t LCDPins;

	LCDPins.pGPIOx = LCD_GPIO_PORT;
	LCDPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	LCDPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	LCDPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	LCDPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;

	LCDPins.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_RS;
	GPIO_Init(&LCDPins);

	LCDPins.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_RW;
	GPIO_Init(&LCDPins);

	LCDPins.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_EN;
	GPIO_Init(&LCDPins);



	LCDPins.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D4;
	GPIO_Init(&LCDPins);

	LCDPins.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D5;
	GPIO_Init(&LCDPins);

	LCDPins.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D6;
	GPIO_Init(&LCDPins);

	LCDPins.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D7;
	GPIO_Init(&LCDPins);


	//Initialize all value of the pins to be outputting 0 initially

	GPIO_WriteToOutputPin(LCDPins.pGPIOx, LCD_GPIO_RS, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCDPins.pGPIOx, LCD_GPIO_RW, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCDPins.pGPIOx, LCD_GPIO_EN, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCDPins.pGPIOx, LCD_GPIO_D4, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCDPins.pGPIOx, LCD_GPIO_D5, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCDPins.pGPIOx, LCD_GPIO_D6, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCDPins.pGPIOx, LCD_GPIO_D7, GPIO_PIN_RESET);




	//2. Do the LCD initializations
		//Based off of LCD datasheet

		// Wait for more than 40 ms after VCC rises to 2.7 V
	mDelay(40);

		// RS = 0, for LCD command
	GPIO_WriteToOutputPin(LCDPins.pGPIOx, LCD_GPIO_RS, GPIO_PIN_RESET);

		// RnW = 0, Writing to LCD
	GPIO_WriteToOutputPin(LCDPins.pGPIOx, LCD_GPIO_RW, GPIO_PIN_RESET);

		// Init step
	write_4_bits(0x3);

		//  Wait for more than 4.1 ms
	mDelay(5);

		// Init step
	write_4_bits(0x3);

		// Wait for more than 100 µs
	uDelay(200);

		// Init step
	write_4_bits(0x3);


		// Init step
	write_4_bits(0x2);

		//Function set command
	lcd_send_command(LCD_CMD_4DL_2N_5X8F);

		//Display ON and cursor ON
	lcd_send_command(LCD_CMD_DON_CURON);

		//Display clear
	lcd_display_clear();

		//Entry mode set
	lcd_send_command(LCD_CMD_INCADD);

}

void lcd_send_command(uint8_t cmd)
{
	// RS = 0, for LCD command
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);

	// RnW = 0, Writing to LCD
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	//send higher nibble
	write_4_bits(cmd >> 4);

	//send lower nibble
	write_4_bits(cmd & 0xF);
}

void lcd_send_char(uint8_t cmd)
{
	// RS = 1, for LCD user data
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_SET);

	// RnW = 0, Writing to LCD
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	//send higher nibble
	write_4_bits(cmd >> 4);

	//send lower nibble
	write_4_bits(cmd & 0xF);
}

void lcd_print_string(char *message)
{
	do
	{
		lcd_send_char((uint8_t) *message++);
	}
	while( *message != '\0');
}

void lcd_display_clear(void)
{
	//Display clear
	lcd_send_command(LCD_CMD_DIS_CLEAR);

	// must wait 2 mS
	mDelay(2);
}

void lcd_display_return_home(void)
{
	//send return cursor to home command
	lcd_send_command(LCD_CMD_DIS_RETURN_HOME);

	// must wait 2 mS
	mDelay(2);
}

/*
 * Set LCD to a specified location given by rown and column information
 * Row numbers - 1 to 2
 * Column numbers - 1 to 16 assuming a 2x16 character display
 */
void lcd_set_cursor(uint8_t row, uint8_t column)
{
	column--;

	switch(row)
	{
		case 1:
			//Set cursor to 1st row address and add index
			lcd_send_command((column |= 0x80));
			break;
		case 2:
			//Set cursor to 1st row address and add index
			lcd_send_command((column |= 0xC0));
			break;
		default:
			break;
	}
}

void mDelay(uint32_t cnt)
{
	for(uint32_t i = 0 ; i < ( cnt * 1000 ) ; i++);
}

void uDelay(uint32_t cnt)
{
	for(uint32_t i = 0 ; i < ( cnt * 1 ) ; i++);
}

static void write_4_bits(uint8_t val)
{
	uint8_t write[4];

	for( uint8_t i = 0 ; i < 4 ; i++ )
	{
		write[i] = ( val >> i ) & 0x1;
	}


	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, write[0]); //Least significant
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, write[1]);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, write[2]);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, write[3]); //Most significant

	lcd_enable();
}

static void lcd_enable(void)
{
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_SET);
	uDelay(10);

	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_RESET);
	uDelay(100); /* Execution time > 37uS */

}

