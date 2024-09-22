/*
 * stm32f407vg_SPI_driver.h
 *
 *  Created on: Mar 23, 2024
 *      Author: butle
 */

#ifndef INC_STM32F407VG_SPI_DRIVER_H_
#define INC_STM32F407VG_SPI_DRIVER_H_

#include "stm32f407vg.h"

/*
 * This is the SPIx (where x = 1, 2, or 3) configuration settings structure
 */

typedef struct
{
	uint8_t SPI_DeviceMode;										/* Possible values from @SPI_DEVICE_MODES */
	uint8_t SPI_BusConfig;										/* Possible values from @SPI_BUS_CONFIG */
	uint8_t SPI_SclkSpeed;										/* Possible values from @SPI_SCLK_SPEEDS */
	uint8_t SPI_DFF;											/* Possible values from @SPI_DFF */
	uint8_t SPI_CPOL;											/* Possible values from @SPI_CPOL */
	uint8_t SPI_CPHA;											/* Possible values from @SPI_CPHA */
	uint8_t SPI_SSM;											/* Possible values from @SPI_SSM */
}SPI_Config_t;

/*
 * This is the handle structure for a SPIx (where x = 1, 2, or 3) peripheral
 */

typedef struct
{
	SPI_RegDef_t *pSPIx;										/* This holds the base address of the SPIx (where x = 1, 2, or 3) peripheral the user wants to use*/
	SPI_Config_t SPIConfig;										/* This structure holds SPI configuration settings */
	uint8_t *TxBuffer;											/* This holds the address of the global Tx buffer to be used in user application */
	uint8_t *RxBuffer;											/* This holds the address of the global Rx buffer to be used in user application */
	uint32_t TxLen;												/* This holds the length of the Tx buffer */
	uint32_t RxLen;												/* This holds the length of the Rx buffer */
	uint8_t TxState;											/* This holds the current state of the Tx buffer */
	uint8_t RxState;											/* This holds the current state of the Rx buffer */
}SPI_Handle_t;


/*
 * @SPI_DEVICE_MODES
 * Macros for SPI device modes
 */

#define SPI_DEVICE_MODE_MASTER					1
#define SPI_DEVICE_MODE_SLAVE					0

/*
 * @SPI_BUS_CONFIG
 * Macros for SPI bus configurations
 */

#define SPI_BUS_CONFIG_FD						0
#define SPI_BUS_CONFIG_HD						1
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY			2
/* #define SPI_BUS_CONFIG_SIMPLEX_TXONLY		3 				Do not need this macro actually - simplex Tx is nothing but full-duplex with the MISO pin disconnected. Therefore, it can use the same macro as FD. */

/*
 * @SPI_SCLK_SPEEDS
 * Macros for possible values of SPI serial clock speeds
 */

#define SPI_SCLK_SPEED_DIV2						0
#define SPI_SCLK_SPEED_DIV4						1
#define SPI_SCLK_SPEED_DIV8						2
#define SPI_SCLK_SPEED_DIV16					3
#define SPI_SCLK_SPEED_DIV32					4
#define SPI_SCLK_SPEED_DIV64					5
#define SPI_SCLK_SPEED_DIV128					6
#define SPI_SCLK_SPEED_DIV256					7

/*
 * @SPI_DFFS
 * Macros for possible values of SPI data frame formats
 */

#define SPI_DFF_8BITS							0
#define SPI_DFF_16BITS							1

/*
 * @SPI_CPOL
 * Macros for possible values of SPI clock polarity
 */

#define SPI_CPOL_LOW							0
#define SPI_CPOL_HIGH							1

/*
 * @SPI_CPHA
 * Macros for possible values of SPI clock phase
 */

#define SPI_CPHA_LOW							0
#define SPI_CPHA_HIGH							1

/*
 * @SPI_SSM
 * Macros for possible values of software slave select (enable software or disable software?)
 */

#define SPI_SSM_DI								0
#define SPI_SSM_EN								1

/*
 * Macros for possible SPI application states
 */

#define SPI_READY								0
#define SPI_BUSY_IN_RX							1
#define SPI_BUSY_IN_TX							2

/*
 * Macros for possible SPI application events (used in call back function)
 */

#define SPI_EVENT_TX_CMPLT						1
#define SPI_EVENT_RX_CMPLT						2
#define SPI_EVENT_OVR_ERR						3

/*
 * SPI status register related flag status definitions
 */

#define SPI_RXNE_FLAG							( 1 << SPI_SR_RXNE )
#define SPI_TXE_FLAG							( 1 << SPI_SR_TXE )
#define SPI_CHSIDE_FLAG							( 1 << SPI_SR_CHSIDE )
#define SPI_UDR_FLAG							( 1 << SPI_SR_UDR )
#define SPI_CRC_ERR_FLAG						( 1 << SPI_SR_CRC_ERR )
#define SPI_MODF_FLAG							( 1 << SPI_SR_MODF )
#define SPI_OVR_FLAG							( 1 << SPI_SR_OVR )
#define SPI_BSY_FLAG							( 1 << SPI_SR_BSY )
#define SPI_FR_FLAG								( 1 << SPI_SR_FRE )

/**********************************************************************************************************************
 * 									APIs supported by this driver
 * 							For more information about the APIs check the function definitions
 **********************************************************************************************************************/

/*
 * Peripheral clock setup
 */

void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);

/*
 * Peripheral Init and De-init
 */

void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);

/*
 * SPI data send and receive
 */

void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len);

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len);
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len);

/*
 * IRQ configuration and ISR handling
 */

void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle);

/*
 * Other peripheral control APIs
 */

void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName);
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx);
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle);
void SPI_CloseReception(SPI_Handle_t *pSPIHandle);

/*
 * Application call back
 */

void SPI_ApplicationEventCallBack(SPI_Handle_t *pSPIHandle, uint8_t AppEvent); //inform the application that transmission of data is complete. To be implemented by user application

#endif /* INC_STM32F407VG_SPI_DRIVER_H_ */
