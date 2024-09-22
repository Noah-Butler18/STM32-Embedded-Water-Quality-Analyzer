/*
 * lcd.h
 *
 *  Created on: Apr 23, 2024
 *      Author: butle
 */

#ifndef LCD_H_
#define LCD_H_

#include "stm32f407vg.h"

/* BSP exposed APIs */

void lcd_init(void);
void lcd_send_command(uint8_t cmd);
void lcd_send_char(uint8_t cmd);
void lcd_display_clear(void);
void lcd_print_string(char *message);
void lcd_display_return_home(void);
void lcd_set_cursor(uint8_t row, uint8_t column);
void mDelay(uint32_t cnt);
void uDelay(uint32_t cnt);

/*
 * Application configurable items
 */

#define LCD_GPIO_PORT							GPIOD
#define LCD_GPIO_RS								GPIO_PIN_NO_0
#define LCD_GPIO_RW								GPIO_PIN_NO_1
#define LCD_GPIO_EN								GPIO_PIN_NO_2
#define LCD_GPIO_D4								GPIO_PIN_NO_3
#define LCD_GPIO_D5								GPIO_PIN_NO_4
#define LCD_GPIO_D6								GPIO_PIN_NO_5
#define LCD_GPIO_D7								GPIO_PIN_NO_6

/*
 * LCD COMMANDS
 */

#define LCD_CMD_4DL_2N_5X8F						0x28
#define LCD_CMD_DON_CURON						0x0E
#define LCD_CMD_INCADD							0x06
#define LCD_CMD_DIS_CLEAR						0x01
#define LCD_CMD_DIS_RETURN_HOME					0x02


#endif /* LCD_H_ */
