/*
 * stm32f407vg_gpio_driver.h
 *
 *  Created on: Jan 15, 2024
 *      Author: butle
 */

#ifndef INC_STM32F407VG_GPIO_DRIVER_H_
#define INC_STM32F407VG_GPIO_DRIVER_H_

#include "stm32f407vg.h"

/*
 * This is the GPIO pin configuration settings structure
 */

typedef struct
{
	uint8_t GPIO_PinNumber;										/* Possible values from @GPIO_PIN_NUMBERS */
	uint8_t GPIO_PinMode;										/* Possible values from @GPIO_PIN_MODES */
	uint8_t GPIO_PinSpeed;										/* Possible values from @GPIO_PIN_OSPEEDS */
	uint8_t GPIO_PinPuPdControl;								/* Possible values from @GPIO_PIN_PUPD_CONFIG */
	uint8_t GPIO_PinOPType;										/* Possible values from @GPIO_PIN_OTYPES */
	uint8_t GPIO_PinAltFunMode;									/* Possible values from @GPIO_PIN_ALTFUNS */
}GPIO_Pin_Config_t;

/*
 * This is the handle structure for a GPIO pin
 */

typedef struct
{
	//pointer to hold the base address of the GPIO peripheral
	GPIO_RegDef_t *pGPIOx;										/*This holds the base address of the GPIO port to which the pin belongs*/
	GPIO_Pin_Config_t GPIO_PinConfig;							/*This variable holds GPIO pin configuration settings*/

}GPIO_Handle_t;

/*
 * @GPIO_PIN_NUMBERS
 * Macros for GPIO pin possible modes
 */

#define GPIO_PIN_NO_0							0
#define GPIO_PIN_NO_1							1
#define GPIO_PIN_NO_2							2
#define GPIO_PIN_NO_3							3
#define GPIO_PIN_NO_4							4
#define GPIO_PIN_NO_5							5
#define GPIO_PIN_NO_6							6
#define GPIO_PIN_NO_7							7
#define GPIO_PIN_NO_8							8
#define GPIO_PIN_NO_9							9
#define GPIO_PIN_NO_10							10
#define GPIO_PIN_NO_11							11
#define GPIO_PIN_NO_12							12
#define GPIO_PIN_NO_13							13
#define GPIO_PIN_NO_14							14
#define GPIO_PIN_NO_15							15

/*
 * @GPIO_PIN_MODES
 * Macros for GPIO pin possible modes
 */

#define GPIO_MODE_IN							0				//Input mode
#define GPIO_MODE_OUT							1				//Output mode
#define GPIO_MODE_ALTFN							2				//Alternate function mode
#define GPIO_MODE_ANALOG						3				//Analog mode
#define GPIO_MODE_IT_FT							4				//Interrupt mode - when in input mode, can configure pin to trigger interrupt to processor on falling edge, "FT"
#define GPIO_MODE_IT_RT							5				//Interrupt mode - when in input mode, can configure pin to trigger interrupt to processor on rising edge, "RT"
#define GPIO_MODE_IT_RFT						6				//Interrupt mode - when in input mode, can configure pin to trigger interrupt to processor on rising or falling edge "RFT"

/*
 * @GPIO_PIN_OTYPES
 * Macros for GPIO pin possible output types
 */

#define GPIO_OP_TYPE_PP							0				//Output type push-pull (reset state)
#define GPIO_OP_TYPE_OD							1				//Output type open drain

/*
 * @GPIO_PIN_OSPEEDS
 * Macros for GPIO pin possible output speeds
 */

#define GPIO_OSPEED_LOW							0				//Output speed LOW						Refer to table 50 of the data sheet for exact frequencies - in the case of the STM32F407 MCU, LOW speed max switching frequency of output is 8MHz
#define GPIO_OSPEED_MEDIUM						1				//Output speed MEDIUM					Refer to table 50 of the data sheet for exact frequencies - in the case of the STM32F407 MCU, MEDIUM speed max switching frequency of output is 50MHz
#define GPIO_OSPEED_HIGH						2				//Output speed HIGH						Refer to table 50 of the data sheet for exact frequencies - in the case of the STM32F407 MCU, HIGH speed max switching frequency of output is 100MHz
#define GPIO_OSPEED_VERYHIGH					3				//Output speed VERY HIGH				Refer to table 50 of the data sheet for exact frequencies - in the case of the STM32F407 MCU, VERY HIGH speed max switching frequency of output is 180MHz

/*
 * @GPIO_PIN_PUPD_CONFIG
 * Macros for GPIO pin pull-up and pull-down configurations
 */

#define GPIO_NO_PUPD							0				//GPIO pin no pull up, pull down
#define GPIO_PIN_PU								1				//GPIO pin pull up
#define GPIO_PIN_PD								2				//GPIO pin pull down

/*
 * @GPIO_PIN_ALTFUNS
 * Macros for GPIO pin possible alternate functions
 * Refer to table 9. Alternate function mapping in the STM32F407 data sheet for allowed pin mappings
 */

#define GPIO_MODE_AF0							0				//GPIO pin alternate function 0 - SYS
#define GPIO_MODE_AF1							1				//GPIO pin alternate function 1 - TIM1/2
#define GPIO_MODE_AF2							2				//GPIO pin alternate function 2 - TIM3/4/5
#define GPIO_MODE_AF3							3				//GPIO pin alternate function 3 - TIM8/9/10/11
#define GPIO_MODE_AF4							4				//GPIO pin alternate function 4 - I2C1/2/3
#define GPIO_MODE_AF5							5				//GPIO pin alternate function 5 - SPI1/SPI2/I2S2/I2S2ext
#define GPIO_MODE_AF6							6				//GPIO pin alternate function 6 - SPI3/I2Sext/I2S3
#define GPIO_MODE_AF7							7				//GPIO pin alternate function 7 - USART1/2/3/I2S3ext
#define GPIO_MODE_AF8							8				//GPIO pin alternate function 8 - UART4/5/USART6
#define GPIO_MODE_AF9							9				//GPIO pin alternate function 9 - CAN1/2TIM12/13/14
#define GPIO_MODE_AF10							10				//GPIO pin alternate function 10 - OTG_FS/OTG_HS
#define GPIO_MODE_AF11							11				//GPIO pin alternate function 11 - ETH
#define GPIO_MODE_AF12							12				//GPIO pin alternate function 12 - FSMC/SDIO/OTG_FS
#define GPIO_MODE_AF13							13				//GPIO pin alternate function 13 - DCMI
#define GPIO_MODE_AF14							14				//GPIO pin alternate function 14 -
#define GPIO_MODE_AF15							15				//GPIO pin alternate function 15 -


/**********************************************************************************************************************
 * 									APIs supported by this driver
 * 							For more information about the APIs check the function definitions
 **********************************************************************************************************************/

/*
 * Peripheral clock setup
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnOrDi); //Parameters needed: 1)Base address of GPIO peripheral to enable.disable clock 2) Enable or disable variable

/*
 * GPIO Initialization and De-initialization (setting register back to reset state)
 */

void GPIO_Init(GPIO_Handle_t *pGPIOHandle); //Parameters needed: GPIO Handle contains both the base address of the GPIO peripheral to be initialized as well as the actually initialization parameters defined by the programmer
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);
//We need not have to go through each register and manually set it to its reset values... there is a register in RCC peripheral that can reset any other peripheral (peripheral reset registers)


/*
 * Data Read and Write
 */

uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber); //Parameters needed: pin number to read from & the address of the GPIO peripheral to read from
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber);

/*
 * IRQ configuration and ISR handling
 */

void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void GPIO_IRQHandling(uint8_t pinNumber);




#endif /* INC_STM32F407VG_GPIO_DRIVER_H_ */
