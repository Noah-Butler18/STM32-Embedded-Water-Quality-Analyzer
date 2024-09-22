/*
 * stm32f407_vg_tim_driver.h
 *
 *  Created on: Jul 2, 2024
 *      Author: butle
 */

#ifndef INC_STM32F407VG_TIM_DRIVER_H_
#define INC_STM32F407VG_TIM_DRIVER_H_

#include "stm32f407vg.h"

/*
 * I2C status register related flag status definitions
 */

#define TIM_FLAG_UIF							( 1 << TIM2_5_SR_UIF )



/**********************************************************************************************************************
 * 									APIs supported by this driver
 * 							For more information about the APIs check the function definitions
 **********************************************************************************************************************/

/*
 * Peripheral clock setup
 */
void TIM_PeriClockControl(TIM2_5_RegDef_t *pTIMx, uint8_t EnOrDi);
void TIM2_5_SetDelayInit(TIM2_5_RegDef_t *pTIMx);

/*
 * Peripheral control
 */
uint8_t TIM2_5_GetFlagStatus(TIM2_5_RegDef_t *pTIMx, uint8_t FlagName);
void TIM2_5_ClearFlag(TIM2_5_RegDef_t *pTIMx, uint8_t FlagName);
void TIM2_5_Delay(TIM2_5_RegDef_t *pTIMx, float MicroSeconds);

void TIM2_5_SetIT(TIM2_5_RegDef_t *pTIMx, float freq);

/*
 * IRQ configuration and ISR handling
 */
void TIM2_5_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi);
void TIM2_5_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void TIM2_5_IRQHandling(TIM2_5_RegDef_t *pTIMx);


#endif /* INC_STM32F407VG_TIM_DRIVER_H_ */
