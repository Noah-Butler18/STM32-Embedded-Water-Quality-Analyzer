/*
 * stm32f407vg_adc_driver.h
 *
 *  Created on: May 17, 2024
 *      Author: butle
 */

#ifndef INC_STM32F407VG_ADC_DRIVER_H_
#define INC_STM32F407VG_ADC_DRIVER_H_

#include "stm32f407vg.h"

/*
 * This is the ADCx configuration settings structure
 */

typedef struct
{
	uint8_t 		ADC_ClkPrescaler;							/* Possible values from @ADC_CLK_Prescalers */
	uint8_t 		ADC_Resolution;								/* Possible values from @ADC_Resolution */
	uint8_t 		ADC_DataAlignment;							/* Possible values from @ADC_DataAlignment */
	uint8_t 		ADC_Mode;									/* Possible values from @ADC_Mode */
	uint8_t 		ADC_SamplingTime[19];						/* Possible values from @ADC_SamplingTime */
	uint16_t 		ADC_AWDHT;									/* Possible values range from 0-4095 */
	uint16_t 		ADC_AWDLT;									/* Possible values range from 0-4095 */
	uint8_t 		ADC_Seq_Len;								/* Possible values from 0-16 */
	uint8_t 		ADC_Seq_Order[16];							/* Possible values from @ADC_Seq_Order */
}ADC_Config_t;

/*
 * This is the handle structure for the ADCx peripheral
 */

typedef struct
{
	ADC_RegDef_t 		*pADCx;									/* This pointer holds the base address of the ADC peripheral */
	ADC_Config_t 		ADC_Config;								/* This variable holds ADC peripheral configuration settings */
	uint16_t 			*pADC_DataBuffer;						/* Buffer that holds converted data from the ADC data register */
	uint8_t 			ADC_SeqLen;								/* This variable holds number of conversion in sequence - used in ADC interrupts */
}ADC_Handle_t;

/*
 * @ADC_CLK_Prescalers
 * Macros for ADC clock possible prescalers
 *
 * NOTE:  Refer to the data sheets for the maximum value of ADCCLK
 */

#define ADC_CLK_DIV_2						0					/* PCLK2 divided by 2 */
#define ADC_CLK_DIV_4						1					/* PCLK2 divided by 4 */
#define ADC_CLK_DIV_6						2					/* PCLK2 divided by 6 */
#define ADC_CLK_DIV_8						3					/* PCLK2 divided by 8 */

/*
 * @ADC_Resolution
 * Macros for ADC digital conversion resolutions
 *
 * NOTE:  Refer to the RM for needed ADCCLK cycles
 */

#define ADC_RES_12BITS						0
#define ADC_RES_10BITS						1
#define ADC_RES_8BITS						2
#define ADC_RES_6BITS						3

/*
 * @ADC_DataAlignment
 * Macros for ADC digital data register bit alignment
 */

#define ADC_RIGHT_ALIGNMENT					0
#define ADC_LEFT_ALIGNMENT					1

/*
 * @ADC_Mode
 * Macros for ADC conversion mode (see RM)
 */

#define ADC_SINGLE_CONVERSION_MODE			0
#define ADC_CONT_CONVERSION_MODE			1
#define ADC_SCAN_CONVERSION_MODE			2
#define ADC_DISCONT_CONVERSION_MODE			3

/*
 * @ADC_SamplingTime
 * Macros for ADC sampling times (number of ADCCLK clock cycles)
 *
 * NOTE: During sampling cycles, the channel selection bits must remain unchanged
 */

#define ADC_SMP_3_CYCLES					0
#define ADC_SMP_15_CYCLES					1
#define ADC_SMP_28_CYCLES					2
#define ADC_SMP_56_CYCLES					3
#define ADC_SMP_84_CYCLES					4
#define ADC_SMP_112_CYCLES					5
#define ADC_SMP_144_CYCLES					6
#define ADC_SMP_480_CYCLES					7

/*
 * @ADC_Seq_Order
 * Macros for ADC channel conversion sequence
 */

#define ADC_IN0								0
#define ADC_IN1								1
#define ADC_IN2								2
#define ADC_IN3								3
#define ADC_IN4								4
#define ADC_IN5								5
#define ADC_IN6								6
#define ADC_IN7								7
#define ADC_IN8								8
#define ADC_IN9								9
#define ADC_IN10							10
#define ADC_IN11							11
#define ADC_IN12							12
#define ADC_IN13							13
#define ADC_IN14							14
#define ADC_IN15							15
#define ADC_IN16							16
#define ADC_IN17							17
#define ADC_IN18							18


/*
 * ADC status register related flag status definitions
 */

#define ADC_FLAG_AWD						( 1 << ADC_SR_AWD )
#define ADC_FLAG_EOC						( 1 << ADC_SR_EOC )
#define ADC_FLAG_JEOC						( 1 << ADC_SR_JEOC )
#define ADC_FLAG_JSTRT						( 1 << ADC_SR_JSTRT )
#define ADC_FLAG_STRT						( 1 << ADC_SR_STRT )
#define ADC_FLAG_OVR						( 1 << ADC_SR_OVR )


/*
 * ADC application event macros (possible arguments for "ADC_ApplicationEventCallback" function)
 */

#define ADC_ERROR_STRT						0

#define ADC_EVENT_AWD						1
#define ADC_EVENT_OVR						2
#define ADC_EVENT_EOC						3





/**********************************************************************************************************************
 * 									APIs supported by this driver
 * 							For more information about the APIs check the function definitions
 **********************************************************************************************************************/

/*
 * Peripheral clock setup
 */
void ADC_PeriClockControl(ADC_RegDef_t *pADCx, uint8_t EnOrDi);

/*
 * ADC Initialization and De-initialization (setting register back to reset state)
 */
void ADC_Init(ADC_Handle_t *pADCHandle);
void ADC_DeInit(void);
void ADC_SequenceInit(ADC_Handle_t *pADCHandle);

/*
 * ADC interrupt initialization and handling
 */
void ADC_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi);
void ADC_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void ADC_EnableIT(ADC_Handle_t *pADCHandle, uint16_t *ConvBuffer, uint8_t Length);
void ADC_DisableIT(ADC_Handle_t *pADCHandle);
void ADC_IRQHandling(ADC_Handle_t *pADCHandle);

/*
 * Other peripheral control APIs
 */
void ADC_PeripheralOnOffControl(ADC_RegDef_t *pADCx, uint8_t EnOrDi);
uint8_t ADC_GetFlagStatus(ADC_RegDef_t *pADCx, uint32_t FlagName);
void ADC_ClearFlag(ADC_RegDef_t *pADCx, uint32_t FlagName);
void ADC_StartADC(ADC_Handle_t *pADCHandle);
__weak void ADC_ApplicationEventCallBack(ADC_Handle_t *pADCHandle, uint8_t AppEvent);

#endif /* INC_STM32F407VG_ADC_DRIVER_H_ */
