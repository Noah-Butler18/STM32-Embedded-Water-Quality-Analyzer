/*
 * stm32f407vg_rcc_driver.h
 *
 *  Created on: Apr 17, 2024
 *      Author: butle
 */

#ifndef INC_STM32F407VG_RCC_DRIVER_H_
#define INC_STM32F407VG_RCC_DRIVER_H_

#include "stm32f407vg.h"



/**********************************************************************************************************************
 * 									APIs supported by this driver
 * 							For more information about the APIs check the function definitions
 **********************************************************************************************************************/

/*
 * Return value of operating frequency for busses that the corresponding peripherals are hanging off of
 */

//APB1 bus
uint32_t RCC_GetPCLK1Val(void);

//APB2 bus
uint32_t RCC_GetPCLK2Val(void);


#endif /* INC_STM32F407VG_RCC_DRIVER_H_ */
