/*
 * stm32f407vg_i2c_driver.c
 *
 *  Created on: Apr 2, 2024
 *      Author: butle
 */

#include "stm32f407vg.h"

/* helper function declarations */

static void USART_SetBaudRate(USART_Handle_t *pUSARTHandle);


/*
 * Peripheral clock setup
 */

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- USART_PeriClockControl

 	 * @brief  		- enables or disables the clock of a USART peripheral

 	 * @param 		- *pUSARTx : USART peripheral base address in MCU memory
 	 * @param  		- EnOrDi : macros to enable or disable the clock (ENABLE or DISABLE macros are in MCU specific header file)

 	 * @retval 		- none

 	 * @Note		- none

*/
void USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnOrDi)
{
	if( EnOrDi == ENABLE )
	{
		if( (uint32_t) pUSARTx == USART1_BASE_ADDR )
		{
			USART1_PCLK_EN();
		}
		else if ( (uint32_t) pUSARTx == USART2_BASE_ADDR )
		{
			USART2_PCLK_EN();
		}
		else if ( (uint32_t) pUSARTx == USART3_BASE_ADDR )
		{
			USART3_PCLK_EN();
		}
		else if ( (uint32_t) pUSARTx == UART4_BASE_ADDR )
		{
			UART4_PCLK_EN();
		}
		else if ( (uint32_t) pUSARTx == UART5_BASE_ADDR )
		{
			UART5_PCLK_EN();
		}
		else if ( (uint32_t) pUSARTx == USART6_BASE_ADDR )
		{
			USART6_PCLK_EN();
		}
	}
	else
	{
		if( (uint32_t) pUSARTx == USART1_BASE_ADDR )
		{
			USART1_PCLK_DI();
		}
		else if ( (uint32_t) pUSARTx == USART2_BASE_ADDR )
		{
			USART2_PCLK_DI();
		}
		else if ( (uint32_t) pUSARTx == USART3_BASE_ADDR )
		{
			USART3_PCLK_DI();
		}
		else if ( (uint32_t) pUSARTx == UART4_BASE_ADDR )
		{
			UART4_PCLK_DI();
		}
		else if ( (uint32_t) pUSARTx == UART5_BASE_ADDR )
		{
			UART5_PCLK_DI();
		}
		else if ( (uint32_t) pUSARTx == USART6_BASE_ADDR )
		{
			USART6_PCLK_DI();
		}
	}
}

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- USART_Init

 	 * @brief  		- API that initializes a specific USART peripheral with programmer-defined values from the USART_Config_t structure

 	 * @param 		- *pUSARTHandle : contains USART peripheral base address in MCU memory as well as configuration values

 	 * @retval 		- none

 	 * @Note		- none

*/
void USART_Init(USART_Handle_t *pUSARTHandle)
{
	uint32_t tempreg = 0;

	//1. enable the USART peripheral clock
	USART_PeriClockControl(pUSARTHandle->pUSARTx, ENABLE);


	/* --------configuration of CR1 register-------- */

	//2. Enable USART Tx and Rx engines according to the USART_Mode configuration item
	if( pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_TX )
	{
		tempreg |= ( 1 << USART_CR1_TE );
	}
	else if( pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_RX )
	{
		tempreg |= ( 1 << USART_CR1_RE );
	}
	else if( pUSARTHandle->USART_Config.USART_Mode == USART_MODE_TXRX )
	{
		tempreg |= ( ( 1 << USART_CR1_TE ) | ( 1 << USART_CR1_RE ) );
	}

	//3. Configure word length
	tempreg |= ( pUSARTHandle->USART_Config.USART_WordLength << USART_CR1_M );

	//4. Configuration of parity control bit fields
	if( pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE )
	{
		//disable hardware parity control (detection and generation)
		pUSARTHandle->pUSARTx->CR1 &= ~( 1 << USART_CR1_PCE );
	}
	else
	{
		//enable hardware parity control (detection and generation)
		tempreg |= ( 1 << USART_CR1_PCE );

		//parity selection
		tempreg |= ( pUSARTHandle->USART_Config.USART_ParityControl << USART_CR1_PS );
	}

	pUSARTHandle->pUSARTx->CR1 |= tempreg;


	/* --------configuration of CR2 register-------- */

	//5. Configure number of STOP bits
	tempreg = 0;
	tempreg |= ( pUSARTHandle->USART_Config.USART_NoOfStopBits << USART_CR2_STOP );
	pUSARTHandle->pUSARTx->CR2 |= tempreg;


	/* --------configuration of CR3 register-------- */

	//6. Configure USART hardware flow control
	tempreg = 0;

	if( pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS )
	{
		tempreg |= ( 1 << USART_CR3_CTSE );
	}
	else if( pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_RTS )
	{
		tempreg |= ( 1 << USART_CR3_RTSE );
	}
	else if( pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS_RTS )
	{
		tempreg |= ( ( 1 << USART_CR3_CTSE ) | ( 1 << USART_CR3_RTSE ) );
	}

	pUSARTHandle->pUSARTx->CR3 |= tempreg;


	/* --------configuration of Baud rate-------- */
	USART_SetBaudRate(pUSARTHandle);
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- USART_DeInit

 	 * @brief  		- API that writes reset values into all of a USART peripheral's registers

 	 * @param 		- *pUSARTx : USART peripheral base address in MCU memory

 	 * @retval 		- none

 	 * @Note		- Function utilizes a feature of the RCC peripheral that quickly resets all the registers of a select peripheral. This saves time as you do not need to individually program the API to reset each register 1 by 1

*/
void USART_DeInit(USART_RegDef_t *pUSARTx)
{
	if( (uint32_t) pUSARTx == USART1_BASE_ADDR )
	{
		USART1_REG_RESET();
	}
	else if ( (uint32_t) pUSARTx == USART2_BASE_ADDR )
	{
		USART2_REG_RESET();
	}
	else if ( (uint32_t) pUSARTx == USART3_BASE_ADDR )
	{
		USART3_REG_RESET();
	}
	else if ( (uint32_t) pUSARTx == UART4_BASE_ADDR )
	{
		UART4_REG_RESET();
	}
	else if ( (uint32_t) pUSARTx == UART5_BASE_ADDR )
	{
		UART5_REG_RESET();
	}
	else if ( (uint32_t) pUSARTx == USART6_BASE_ADDR )
	{
		USART6_REG_RESET();
	}

}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- USART_SendData

 	 * @brief  		- Function that enables and handles the transmission of data to the outside world using the USART peripheral

 	 * @param 		- *pUSARTHandle : contains USART peripheral base address in MCU memory
 	 * @param 		- *pTxBuffer : pointer to a buffer containing the user's data
 	 * @param 		- Len : Length of data buffer, in bytes

 	 * @retval 		- none

 	 * @Note		- Setting the TE bit drives the USART to send an idle frame before the first data frame

*/
void USART_SendData(USART_Handle_t *pUSARTHandle,uint8_t *pTxBuffer, uint32_t Len)
{
	uint16_t *pData;

	//Make sure USART is enabled

	while( Len > 0 )
	{
		//Blocking call - wait until TxE bit is set (transmit buffer empty) before writing data
		while( !( USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TXE) ))
			;

		//If the frame length is 9 bits
		if( pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS )
		{
			pData = (uint16_t *)pTxBuffer;
			pUSARTHandle->pUSARTx->DR = ( *pData ) & ( (uint16_t) 0x1FF );

			if( pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE )
			{
				// Parity bit is disabled - i.e., we will be sending 9 data bits
				pTxBuffer += 2;
			}
			else
			{
				// Parity bit is enabled - i.e., we will be sending 8 bits of data
				// The 9th bit of data will be replaced by parity bit
				pTxBuffer++;
			}

			Len--;
		}
		else
		{
			//If the frame length is 8 bits

			//send data
			pUSARTHandle->pUSARTx->DR = *pTxBuffer;
			pTxBuffer++;
			Len--;
		}
	}

	//Blocking call - wait until TC (transmit complete) bit is set before exiting the function
	while( !( USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TC) ))
		;

	USART_ClearFlag(pUSARTHandle->pUSARTx, USART_SR_TC);
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- USART_ReceiveData

 	 * @brief  		- Function that enables and handles the reception of data from the outside world using the USART peripheral

 	 * @param 		- *pUSARTHandle : contains USART peripheral base address in MCU memory
 	 * @param 		- *pRxBuffer : contains pointer to base address of user's Rx buffer
 	 * @param 		- Len : Length of data buffer, in bytes

 	 * @retval 		- none

 	 * @Note		- none

*/
void USART_ReceiveData(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	//Make sure USART is enabled

	while( Len > 0 )
	{
		//Blocking call - wait until RxNE bit is set (transmit buffer empty) before reading data
		while( !( USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_RXNE) ))
			;

		if( pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS )
		{
			//Will be receiving 9 bits
			if( pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE )
			{
				//Parity bit is disabled - i.e., we will be receiving 9 data bits
				*((uint16_t *) pRxBuffer) = ( pUSARTHandle->pUSARTx->DR & ( (uint16_t) 0x1FF ) );

				//increment buffer pointer by 2 bytes
				pRxBuffer += 2;
			}
			else
			{
				//Parity bit is enabled - i.e., we will be receiving a 8 data bits
				*pRxBuffer = ( pUSARTHandle->pUSARTx->DR & ( (uint8_t) 0xFF ) );

				//increment buffer pointer by 1 byte
				pRxBuffer++;
			}

			Len--;
		}
		else
		{
			//Will be receiving 8 bits
			if( pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE )
			{
				//Parity bit is disabled - i.e., we will be receiving 8 data bits
				*pRxBuffer = ( (uint8_t) pUSARTHandle->pUSARTx->DR & ( (uint8_t) 0xFF ) );
			}
			else
			{
				//Parity bit is enabled - i.e., we will be receiving only 7 data bits
				*pRxBuffer = ( (uint8_t) pUSARTHandle->pUSARTx->DR & ( (uint8_t) 0x7F ) );
			}

			//increment buffer pointer by 1 byte
			pRxBuffer++;
			Len--;
		}

	}
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- USART_SendDataIT

 	 * @brief  		- API that enables a device to send data over USART peripheral using interrupts

 	 * @param 		- *pUSARTHandle : contains USART peripheral base address in MCU memory
 	 * @param 		- *pTxBuffer : pointer to a buffer containing the user's data
 	 * @param 		- Len : Length of data buffer, in bytes

 	 * @retval 		- txstate : current state of USART transmission (ongoing or clear?)

 	 * @Note		- This function call is non-blocking
*/
uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle,uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t txstate = pUSARTHandle->TxBusyState;

	if(txstate != USART_BUSY_IN_TX)
	{
		pUSARTHandle->TxLen = Len;
		pUSARTHandle->pTxBuffer = pTxBuffer;
		pUSARTHandle->TxBusyState = USART_BUSY_IN_TX;

		//Enable interrupt for TxE
		pUSARTHandle->pUSARTx->CR1 |= ( 1 << USART_CR1_TXEIE );

		//Enable interrupt for TC
		pUSARTHandle->pUSARTx->CR1 |= ( 1 << USART_CR1_TCIE );
	}

	return txstate;
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- USART_MasterReceiveDataIT

 	 * @brief  		- API that enables a device to receive data over USART peripheral using interrupts

 	 * @param 		- *pUSARTHandle : contains USART peripheral base address in MCU memory
 	 * @param 		- *pRxBuffer : pointer to a buffer containing the user's data
 	 * @param 		- Len : Length of data buffer, in bytes

 	 * @retval 		- rxstate : current state of USART reception (ongoing or available)

 	 * @Note		- This function call is non-blocking
*/
uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t rxstate = pUSARTHandle->RxBusyState;

	if(rxstate != USART_BUSY_IN_RX)
	{
		pUSARTHandle->RxLen = Len;
		pUSARTHandle->pRxBuffer = pRxBuffer;
		pUSARTHandle->RxBusyState = USART_BUSY_IN_RX;

		//Read the USARTx_DR register to make sure that RXNE is cleared
		(void) pUSARTHandle->pUSARTx->DR;

		//Enable interrupt for RxNE ( and Overrun Error Detected )
		pUSARTHandle->pUSARTx->CR1 |= ( 1 << USART_CR1_RXNEIE );
	}

	return rxstate;
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- USART_IRQInterruptConfig

 	 * @brief  		- API that configures interrupts generated by a USART peripheral. This function handles the configurations needed for interrupts on the processor side
 	 * 				- Refer to processor guide here for more details: https://www.engr.scu.edu/~dlewis/book3/docs/Cortex-M4_Devices_Generic_User_Guide.pdf

 	 * @param 		- IRQNumber : Number associated with the peripheral's exception handler in the NVIC vector table
 	 * @param 		- EnOrDi : macros to enable or disable the IRQ (ENABLE or DISABLE macros in MCU specific header file)

 	 * @retval 		- none

 	 * @Note		- none

*/
void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			//configure ISER0 register in processor //0 to 31
			*NVIC_ISER0 |= ( 1 << IRQNumber );
		}
		else if(IRQNumber > 31 && IRQNumber <= 63)
		{
			//configure ISER1 register in processor //32 to 63
			*NVIC_ISER1 |= ( 1 << (IRQNumber % 32) );
		}
		else if(IRQNumber > 63 && IRQNumber <= 95)
		{
			//configure ISER2 register in processor //64 to 95
			*NVIC_ISER2 |= ( 1 << (IRQNumber % 32) );
		}
	}
	else
	{
		if(IRQNumber <= 31)
		{
			//configure ICER0 register in processor //0 to 31
			*NVIC_ICER0 |= ( 1 << IRQNumber );
		}
		else if(IRQNumber > 31 && IRQNumber <= 63)
		{
			//configure ICER1 register in processor //32 to 63
			*NVIC_ICER1 |= ( 1 << (IRQNumber % 32) );
		}
		else if(IRQNumber > 63 && IRQNumber <= 95)
		{
			//configure ICER2 register in processor //64 to 95
			*NVIC_ICER2 |= ( 1 << (IRQNumber % 32) );
		}
	}
}





/*********************** Function Documentation ***************************************
 *
	 * @fn			- USART_IRQPriorityConfig

	 * @brief  		- API that configures the priority level of a given IRQ (USART-driven interrupt).

	 * @param 		- IRQPriority : Value that contains priority level of interrupt as compared to other interrupts.
	 	 	 	 	 This is handled on the processor side in Cortex-M4 internal peripheral registers

	 * @param		- IRQNumber : Number associated with the peripheral's exception handler in the NVIC vector table

	 * @retval 		- none

	 * @Note		- none

*/
void USART_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//1. Find out which IPR register the IRQ is in
	uint8_t Offset = IRQNumber / 4;

	//2. Clear and write by shifting the priority value into the correct position
	uint8_t temp = (IRQNumber % 4) * 8;

	//Note: In the Cortex-M4, only 4 most significant bits of each of the 4 "sections" in the Interrupt priority register (IPR)
	//are implemented, not the last 4.
	//Because if this, you are limited to only 16 priority levels instead of 256 (this was for cost savings apparently).
	//Keeping this in mind, you must also shift whatever priority level desired over to the left by 4 bits no matter the section of the byte it is in.
	uint8_t shiftAmount = temp + ( 8 - NO_PR_BITS_IMPLEMENTED );
	*(NVIC_IPR_BASE_ADDR + (Offset) ) &= ~( 0xFF << shiftAmount );												//Clear the existing priority level
	*(NVIC_IPR_BASE_ADDR + (Offset) ) |= ( IRQPriority << shiftAmount );										//Write the desired priority level
}


/*********************** Function Documentation ***************************************
 *
	 * @fn			- USART_IRQHandling

	 * @brief  		- API that handles interrupts caused by events that are flagged in the USART peripheral SR register

	 * @param 		- *pUSARTHandle : contains USART peripheral base address in MCU memory as well as data handling information

	 * @retval 		- none

	 * @Note		-

*/
void USART_IRQHandling(USART_Handle_t *pUSARTHandle)
{
	uint16_t *pData;

	//Make sure USART is enabled

	//1. Detect which interrupt has occurred

	//2. Handle each interrupt separately


	/********************************* TC FLAG (transmission complete) *******************************/

	uint32_t temp1 = pUSARTHandle->pUSARTx->CR1 & ( 1 << USART_CR1_TCIE );
	uint32_t temp2 = USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TC);
	if( temp1 && temp2 )
	{
		//Clearing the TC bit will also clear the TxE bit (clearing TC involves a write to the USART_DR register)

		if( pUSARTHandle->TxBusyState == USART_BUSY_IN_TX ) //Make sure we are currently Tx-ing data
		{

			if( ! (pUSARTHandle->TxLen) ) //Make sure the last data chunk has really been sent
			{
				//Clear the TC flag
				USART_ClearFlag(pUSARTHandle->pUSARTx, USART_SR_TC);

				//Disable the TC flag interrupts (TxE interrupts are disabled in TxE handler when data length reaches 0)
				pUSARTHandle->pUSARTx->CR1 &= ~( 1 << USART_CR1_TCIE );

				//Reset USART application Tx state
				pUSARTHandle->TxBusyState = USART_READY;

				//Clear handle Tx variables that are no longer needed
				pUSARTHandle->TxLen = 0;
				pUSARTHandle->pTxBuffer = NULL;

				//Alert application that transmission is complete
				USART_ApplicationEventCallBack(pUSARTHandle, USART_EVENT_TX_COMPLETE);
			}
		}
		else
		{
			//if not Txing data and we trigger TC interrupt, something has gone wrong
			while(1);
		}
	}


	/********************************* TxE FLAG (Tx data register empty) *******************************/

	temp1 = pUSARTHandle->pUSARTx->CR1 & ( 1 << USART_CR1_TXEIE );
	temp2 = USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TXE);

	if(  temp1 && temp2  )
	{
		//Make sure application is currently Tx-ing
		if( pUSARTHandle->TxBusyState == USART_BUSY_IN_TX)
		{
			//Make sure that there is still data to transmit
			if( pUSARTHandle->TxLen > 0 )
			{
				//Check if frame length is 9 bits
				if( ( pUSARTHandle->pUSARTx->CR1 & ( 1 << USART_CR1_M ) ) )
				{
					//Parity bit is disabled
					if( ! ( pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_PCE ) ) )
					{
						//Data is 9 bits - need 2 bytes 2 store
						pData = ((uint16_t *) pUSARTHandle->pTxBuffer);

						//write only last 9 bits to DR
						pUSARTHandle->pUSARTx->DR = (*pData & (uint16_t) 0x1FF);

						//increment buffer 2 bytes to fetch next data
						pUSARTHandle->pTxBuffer += 2;
					}
					else
					{
						//parity bit is enabled

						//Data is 8 bits - write buffer data to DR
						pUSARTHandle->pUSARTx->DR = *(pUSARTHandle->pTxBuffer);

						//Increment buffer 1 byte
						pUSARTHandle->pTxBuffer++;
					}
				}
				else
				{
					//frame length is 8 bits

					//send data
					pUSARTHandle->pUSARTx->DR = ( *(pUSARTHandle->pTxBuffer) & ( (uint8_t) 0xFF ) );
					pUSARTHandle->pTxBuffer++;
				}

				//Decrement buffer length
				pUSARTHandle->TxLen--;
			}
			else if( pUSARTHandle->TxLen == 0 )
			{
				//TxLen = 0, transmission is over

				//Disable Tx interrupts
				pUSARTHandle->pUSARTx->CR1 &= ~( 1 << USART_CR1_TXEIE );
			}
		}
		else
		{
			//Application is not currently Tx-ing, but still got TxE flag. Something went wrong
			while(1);
		}
	}


	/********************************* RxNE FLAG (Rx data register not empty) *******************************/

	temp1 = pUSARTHandle->pUSARTx->CR1 & ( 1 << USART_CR1_RXNEIE );
	temp2 = USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_RXNE);

	if( temp1 && temp2 )
	{
		//Make sure USART is currently Rx-ing data
		if( pUSARTHandle->RxBusyState == USART_BUSY_IN_RX)
		{
			//Make sure buffer still has data to read
			if( pUSARTHandle->RxLen > 0 )
			{
				//Check if frame length is 9 bits
				if( ( pUSARTHandle->pUSARTx->CR1 & ( 1 << USART_CR1_M ) )  )
				{
					//Check if parity bit is disabled
					if( ! ( pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_PCE ) ) )
					{
						//Parity bit is disabled - i.e., we will be receiving 9 data bits

						*((uint16_t *) pUSARTHandle->pRxBuffer) = ( pUSARTHandle->pUSARTx->DR & ( (uint16_t) 0x1FF ) );

						//increment Rx buffer pointer by 2 bytes
						pUSARTHandle->pRxBuffer += 2;
					}
					else
					{
						//Parity bit is enabled - i.e., we will be receiving a 8 data bits
						*pUSARTHandle->pRxBuffer = ( pUSARTHandle->pUSARTx->DR & ( (uint8_t) 0xFF ) );

						//increment buffer pointer by 1 byte
						pUSARTHandle->pRxBuffer++;
					}
				}
				else
				{
					//Frame length is 8 bits

					//Check if parity bit is disabled
					if( ! ( pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_PCE ) ) )
					{
						//Parity bit is disabled - i.e., we will be receiving 8 data bits
						*pUSARTHandle->pRxBuffer = ( pUSARTHandle->pUSARTx->DR & ( (uint8_t) 0xFF ) );
					}
					else
					{
						//Parity bit is enabled - i.e., we will be receiving 7 data bits
						*pUSARTHandle->pRxBuffer = ( pUSARTHandle->pUSARTx->DR & ( (uint8_t) 0x7F ) );
					}

					//increment Rx buffer pointer by 1 byte
					pUSARTHandle->pRxBuffer++;
				}

				//Decrement Rx buffer length
				pUSARTHandle->RxLen--;
			}
			else if( pUSARTHandle->RxLen == 0 )
			{
				//No more data to read into Rx buffer

				//Disable RxNE interrupts
				pUSARTHandle->pUSARTx->CR1 &= ~( 1 << USART_CR1_RXNEIE );

				//Reset Rx application state to ready
				pUSARTHandle->RxBusyState = USART_READY;

				//Notify application that data reception has ended
				USART_ApplicationEventCallBack(pUSARTHandle, USART_EVENT_RX_COMPLETE);
			}
		}
		else
		{
			//USART is not current receiving data, but RxNE flag is still set. Something went wrong
			while(1);
		}
	}


	/********************************* CTS FLAG (clear to send) *******************************/

	//Note: CTS feature not applicable for UART4 and UART5

	temp1 = pUSARTHandle->pUSARTx->CR3 & ( 1 << USART_CR3_CTSE );
	temp2 = pUSARTHandle->pUSARTx->CR3 & ( 1 << USART_CR3_CTSIE ); //(This bit not available for UART4 & UART5)
	uint8_t temp3 = USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_CTS);

	if( temp1 && temp2 && temp3 )
	{
		//Clear flag
		USART_ClearFlag(pUSARTHandle->pUSARTx, USART_SR_CTS);

		//Notify application that event has occurred
		USART_ApplicationEventCallBack(pUSARTHandle, USART_EVENT_CTS);
	}

	/********************************* ORE FLAG (overrun error) *******************************/

	temp1 = pUSARTHandle->pUSARTx->CR1 & ( 1 << USART_CR1_RXNEIE );
	temp2 = USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_ORE);

	if( temp1 && temp2 )
	{
		//Do not need to clear flag here, the user should decide what happens (and clear flag) in their own application

		//Notify application that event has occurred
		USART_ApplicationEventCallBack(pUSARTHandle, USART_ERROR_ORE);
	}

	/********************************* IDLE FLAG (idle line detected) *******************************/

	temp1 = pUSARTHandle->pUSARTx->CR1 & ( 1 << USART_CR1_IDLEIE );
	temp2 = USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_IDLE);

	if( temp1 && temp2 )
	{
		//Clear flag
		USART_ClearFlag(pUSARTHandle->pUSARTx, USART_SR_IDLE);

		//Notify application that event has occurred
		USART_ApplicationEventCallBack(pUSARTHandle, USART_EVENT_IDLE);
	}


	/*************************Check for Error Flag ********************************************/

	//Noise Flag, Overrun error and Framing Error in multibuffer communication
	//The below code will get executed in only if multibuffer mode is used.

	temp2 =  pUSARTHandle->pUSARTx->CR3 & ( 1 << USART_CR3_EIE) ;

	if( temp2 )
	{

		//Check if the interrupt is the framing error (FE)
		temp1 = USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_FE);

		if( temp1 )
		{
			/*
				This bit is set by hardware when a de-synchronization, excessive noise or a break character
				is detected. It is cleared by a software sequence (an read to the USART_SR register
				followed by a read to the USART_DR register).
			*/

			//Cleared by user application

			//Notify application error has occurred
			USART_ApplicationEventCallBack(pUSARTHandle, USART_ERROR_FE);
		}

		//Check if the interrupt is the noise error (NE)
		temp1 = USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_NF);

		if( temp1 )
		{
			/*
				This bit is set by hardware when noise is detected on a received frame. It is cleared by a
				software sequence (an read to the USART_SR register followed by a read to the
				USART_DR register).
			*/

			//Cleared by user application

			//Notify application error has occurred
			USART_ApplicationEventCallBack(pUSARTHandle, USART_ERROR_NE);
		}

		//Check if the interrupt is the overrun error (ORE)
		temp1 = USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_ORE);

		if( temp1 )
		{
			//Cleared by user application

			//Notify application error has occurred
			USART_ApplicationEventCallBack(pUSARTHandle, USART_ERROR_NE);
		}
	}







}



/*********************** Function Documentation ***************************************
 *
 	 * @fn			- USART_PeripheralControl

 	 * @brief  		- API that enables or disables the USARTx peripheral

 	 * @param 		- *pUSARTx : USART peripheral base address in MCU memory
 	 * @param 		- EnOrDi : User-inputted value to enable or disable the device's USART peripheral

 	 * @retval 		- none

 	 * @Note		- When this bit is cleared, the USART prescalers and outputs are stopped and the end of the current
 	  				byte transfer in order to reduce power consumption. This bit is set and cleared by software

*/
void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnOrDi)
{
	if( EnOrDi == ENABLE )
	{
		pUSARTx->CR1 |= (1 << USART_CR1_UE);
	}
	else if( EnOrDi == DISABLE )
	{
		pUSARTx->CR1 &= ~(1 << USART_CR1_UE);
	}
	else
		;
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- USART_GetFlagStatus

 	 * @brief  		- API that returns that value of a specific flag in the USART peripheral status register

 	 * @param 		- *pUSARTx : USART peripheral base address in MCU memory
 	 * @param 		- FlagName : Name of the desired flag you want to check the status of
 	 	 	 	 	Possible values: stm32f407vg_usart_driver.h -> @USART_FlagName

 	 * @retval 		- Data that is read from flag - can only be either 1 or 0 (set or reset)

 	 * @Note		- none

*/
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx , uint32_t FlagName)
{
	if( pUSARTx->SR & FlagName ) return FLAG_SET;
	else return FLAG_RESET;
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- USART_ClearFlag

 	 * @brief  		- API that enables or disables the USARTx peripheral

 	 * @param 		- *pUSARTx : USART peripheral base address in MCU memory
 	 * @param 		- StatusFlagName : Name of the desired flag you want to clear (set to 0)

 	 * @retval 		- none

 	 * @Note		- Function should not handle the clearing of TxE, RxNE, or TC bits. These should be handled by data send and receive APIs

*/
void USART_ClearFlag(USART_RegDef_t *pUSARTx, uint16_t StatusFlagName)
{
	uint32_t DummyRead;

	if( StatusFlagName == USART_SR_LBD )
	{
		//LBD flag
		pUSARTx->SR &= ~( 1 << USART_SR_LBD );
	}
	else if( StatusFlagName == USART_SR_CTS )
	{
		//CTS flag
		pUSARTx->SR &= ~( 1 << USART_SR_CTS );
	}
	else if( StatusFlagName == USART_SR_TC )
	{
		//TC flag
		pUSARTx->SR &= ~( 1 << USART_SR_TC );
	}
	else if( StatusFlagName == USART_SR_RXNE )
	{
		//NOTE: THIS FUNCTION IS NOT USED TO CLEAR RXNE FLAG

		;
	}
	else
	{
		//Clearing IDLE, ORE, NF, FE, PE flags
		DummyRead = pUSARTx -> SR;
		DummyRead = pUSARTx -> DR;
		(void) DummyRead;
	}
}

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- USART_ApplicationEventCallBack

 	 * @brief  		- weak implementation of an API that signals an event during communication has occurred

 	 * @param 		- *pUSARTx : USART peripheral base address in MCU memory
 	 * @param 		- AppEvent : Application event macro. Decides what event has happened, and what the outcome should be

 	 * @retval 		- none

 	 * @Note		- The user should implement this in their application code since how you want the communication protocol to respond is application-specific

*/
__weak void USART_ApplicationEventCallBack(USART_Handle_t *pUSARTHandle, uint8_t AppEvent)
{
	//This is a weak implementation. The application may overwrite this function.
}




/*	--------------------------------- Helper functions ---------------------------------	*/
/*	-------------------- Available only to driver source file --------------------------	*/


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- USART_SetBaudRate

 	 * @brief  		- Helper function for USART driver source file that writes the desired value into the USART_BRR register to set desired baud rate

 	 * @param 		- *pUSARTx : Holds base address of USART peripheral. Used in determining which bus the peripheral is hanging off of

 	 * @retval 		- none

 	 * @Note		- User APIs from
 	 *
*/
static void USART_SetBaudRate(USART_Handle_t *pUSARTHandle)
{
	//1. Determine which APB bus the USART/UART peripheral is connected to

	uint32_t Fclk;

	if( ( pUSARTHandle->pUSARTx == USART6 ) || ( pUSARTHandle->pUSARTx == USART1 ) )
	{
		//USART1, USART6 on APB2

		//Extract APB2 clock frequency
		Fclk = RCC_GetPCLK2Val();
	}
	else
	{
		//USART2, USART3, UART4, UART5 on APB1

		//Extract APB1 clock frequency
		Fclk = RCC_GetPCLK1Val();
	}


	//2. Calculate USARTDIV value for the user's desired baud rate

	uint8_t Over8 = ( pUSARTHandle->pUSARTx->CR1 >> USART_CR1_OVER8 ) & 0x1;
	uint32_t DesiredBaud = pUSARTHandle->USART_Config.USART_Baud;

		//Multiplying original formula by 100 to deal with fractional part easier
	uint32_t USARTDIV = ( 25 * Fclk ) / ( 2 * (2 - Over8) * DesiredBaud );


	//3. Break USARTDIV value into mantissa and fractional part

	uint16_t Mantissa = USARTDIV / 100;

	uint16_t Frac = USARTDIV % 100;

		//Round fractional part to nearest whole number
	if(Over8 == SET)
	{
		//Only use last 3 bits of BRR register for fractional part
		Frac = ( Frac * 8 );

		//Round fractional part to nearest whole number
		Frac = ( (( Frac + 50 ) / 100) & ( (uint8_t) 0x7 ) );

	}
	else
	{
		//Use last 4 bits of BRR register for fractional part
		Frac = ( Frac * 16 );

		//Round fractional part to nearest whole number
		Frac = ( (( Frac + 50 ) / 100) & ( (uint8_t) 0xF ) );
	}


	//4. Write USARTDIV value into USART_BRR register

	uint16_t temp = 0;
	temp |= ( Mantissa << 4 ) | ( Frac );

	pUSARTHandle->pUSARTx->BRR = temp;
}

