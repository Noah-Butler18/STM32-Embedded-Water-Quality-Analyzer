/*
 * stm32f407vg_i2c_driver.c
 *
 *  Created on: Apr 2, 2024
 *      Author: butle
 */

#include "stm32f407vg.h"

//helper functions
// static uint32_t RCC_GetPLLOutputClk(void); //not implemented since we don't use PLL clock
static void I2C_ExecuteAddressPhaseWrite(I2C_Handle_t *pI2CHandle, uint8_t SlaveAddr);
static void I2C_ExecuteAddressPhaseRead(I2C_Handle_t *pI2CHandle, uint8_t SlaveAddr);
static void I2C_ClearAddrFlag(I2C_Handle_t *pI2CHandle);
static void I2C_ClearStopfFlag(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleRxNEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleTxEInterrupt(I2C_Handle_t *pI2CHandle);

void I2C_CloseSendData(I2C_Handle_t *pI2CHandle);
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle);





/*
 * Peripheral clock setup
 */

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- I2C_PeriClockControl

 	 * @brief  		- enables or disables the clock of a I2C peripheral

 	 * @param 		- *pI2Cx : I2C peripheral base address in MCU memory
 	 * @param  		- EnOrDi : macros to enable or disable the clock (ENABLE or DISABLE macros are in MCU specific header file)

 	 * @retval 		- none

 	 * @Note		- none

*/
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if( EnOrDi == ENABLE )
	{
		if( (uint32_t) pI2Cx == I2C1_BASE_ADDR )
		{
			I2C1_PCLK_EN();
		}
		else if ( (uint32_t) pI2Cx == I2C2_BASE_ADDR )
		{
			I2C2_PCLK_EN();
		}
		else if ( (uint32_t) pI2Cx == I2C3_BASE_ADDR )
		{
			I2C3_PCLK_EN();
		}
		else
			;
	}
	else
	{
		if( (uint32_t) pI2Cx == I2C1_BASE_ADDR )
		{
			I2C1_PCLK_DI();
		}
		else if ( (uint32_t) pI2Cx == I2C2_BASE_ADDR )
		{
			I2C2_PCLK_DI();
		}
		else if ( (uint32_t) pI2Cx == I2C3_BASE_ADDR )
		{
			I2C3_PCLK_DI();
		}
		else
			;
	}
}

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- I2C_Init

 	 * @brief  		- API that initializes a specific I2C peripheral with programmer-defined values from the I2C_Config_t structure

 	 * @param 		- *pI2CHandle : contains I2C peripheral base address in MCU memory as well as configuration values

 	 * @retval 		- none

 	 * @Note		- none

*/
void I2C_Init(I2C_Handle_t *pI2CHandle)
{
	// enable the I2C peripheral clock
	I2C_PeriClockControl(pI2CHandle->pI2Cx, ENABLE);

	//Peripheral should be disabled when configuring peripheral settings



	uint32_t tempreg = 0;

	// Configure the ACK control
	tempreg |= ( pI2CHandle->I2C_Config.I2C_ACKControl << I2C_CR1_ACK );
	pI2CHandle->pI2Cx->CR1 |= tempreg;


	//Configure the FREQ field used in - used by hardware to set correctly execute data setup and hold times to be compliant with I2C communication specifications
	tempreg = 0;
	tempreg |= (( RCC_GetPCLK1Val() / 1000000U ) << I2C_CR2_FREQ ); //FREQ[5:0] config bits in I2C_CR2 are in multiples of 1MHz
	pI2CHandle->pI2Cx->CR2 |= tempreg;


	// Configure the device address (applicable only when device is slave)
	tempreg = 0;
	tempreg |= ( pI2CHandle->I2C_Config.I2C_DeviceAddress << I2C_OAR1_ADD7_1 );
	pI2CHandle->pI2Cx->OAR1 |= tempreg;

		//Bit 14 of OAR1 register must always be maintained at 1 (See RM 27.6.3)
	pI2CHandle->pI2Cx->OAR1 |= (1 << 14);

	// Configure the mode (standard or fast)


	// Configure the SCL speed and duty cycle

		//CCR calculations
	uint32_t desired_SCL = 0;
	uint16_t ccr_val;
	uint8_t duty_val;

	desired_SCL = pI2CHandle->I2C_Config.I2C_SCLSpeed;
	duty_val = pI2CHandle->I2C_Config.I2C_FMDutyCycle;

	if(desired_SCL <= I2C_SCL_SPEED_SM) //standard mode (I2C SCL < 100kHz)
	{
		pI2CHandle->pI2Cx->CCR &= ~( 1 << I2C_CCR_FAST_SLOW );	//configure mode bit field in CCR
		ccr_val = ( RCC_GetPCLK1Val() / ( 2 * desired_SCL ) );
	}
	else 								//fast mode ( 100kHz < I2C SCL < 400kHz)
	{
		pI2CHandle->pI2Cx->CCR |= ( 1 << I2C_CCR_FAST_SLOW );	//configure mode bit field in CCR

		if( duty_val == 0 ) 	//Fm mode t_low/t_high = 2
		{
			ccr_val = ( RCC_GetPCLK1Val() / ( 3 * desired_SCL ) );
		}
		else 					//Fm mode t_low/t_high = 16/9
		{
			ccr_val = ( RCC_GetPCLK1Val() / ( 25 * desired_SCL ) );
		}
	}

	ccr_val = ( ccr_val & 0xFFF ); //only last 12 btis are valid
	tempreg = 0;
	tempreg = ( duty_val << I2C_CCR_DUTY ) | ( ccr_val << I2C_CCR_CCR ); //mode already configured above, just need duty value and CCR value configured
	pI2CHandle->pI2Cx->CCR |= tempreg;



	//Configure the rise time of I2C pins
	tempreg = 0;
	double t_rise;
	double Tpclk1 = (double) ( 1.0 / RCC_GetPCLK1Val());

	desired_SCL = pI2CHandle->I2C_Config.I2C_SCLSpeed;
	if(desired_SCL <= I2C_SCL_SPEED_SM) //standard mode (I2C SCL < 100kHz)
	{
		t_rise = ( 1000.0 / 1000000000.0 );
	}
	else 								//fast mode ( 100kHz < I2C SCL < 400kHz
	{
		t_rise = ( 300.0 / 1000000000.0 );
	}

	tempreg = ( t_rise / Tpclk1 ) + 1;
	pI2CHandle->pI2Cx->TRISE |= (tempreg & 0x3F );

}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- I2C_DeInit

 	 * @brief  		- API that writes reset values into all of a I2C peripheral's registers

 	 * @param 		- *pI2Cx : I2C peripheral base address in MCU memory

 	 * @retval 		- none

 	 * @Note		- Function utilizes a feature of the RCC peripheral that quickly resets all the registers of a select peripheral. This saves time as you do not need to individually program the API to reset each register 1 by 1

*/
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
	if( (uint32_t) pI2Cx == I2C1_BASE_ADDR )
	{
		I2C1_REG_RESET();
	}
	else if ( (uint32_t) pI2Cx == I2C2_BASE_ADDR )
	{
		I2C2_REG_RESET();
	}
	else if ( (uint32_t) pI2Cx == I2C3_BASE_ADDR )
	{
		I2C3_REG_RESET();
	}
	else
		;
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- I2C_MasterSendData

 	 * @brief  		- API that configures a device as an I2C master and sends data to a slave

 	 * @param 		- *pI2CHandle : contains I2C peripheral base address in MCU memory
 	 * @param 		- *pTxBuffer : pointer to transmit buffer of user data
 	 * @param 		- Len : Length of user data buffer in bytes
 	 * @param 		- SlaveAddr : Address of slave device that the master wants to send data to
 	 * @param 		- Sr : Sr = repeated start. If set, the API will generate a repeated start instead of releasing the bus and ending communication

 	 * @retval 		- none

 	 * @Note		- none

*/
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
// Make sure peripheral is enabled before attempting communication

//1. Generate start condition
	I2C_GenerateCondition(pI2CHandle, START);

//2. Master sends address phase

	//Must first wait until the start condition has been detected - BLOCKING
	while( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB ) )
		;
	//Clear start condition detection bit in SR1 to stop clock stretching to LOW
	I2C_ExecuteAddressPhaseWrite(pI2CHandle, SlaveAddr);


//3. Determine if address was sent by master. If no match, don't try sending data - BLOCKING
	while( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR ))
		;

	//Clear ADDR flag
	I2C_ClearAddrFlag(pI2CHandle);


//4. Begin sending data
	for(uint32_t i = 0; i < Len; i++)
	{
		//Wait until Data register is empty to send another data byte - BLOCKING
		while( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE ))
			;

		pI2CHandle->pI2Cx->DR = *pTxBuffer;
		pTxBuffer++;
	}

//5. Data transmission done - generate stop signal and close transmission
	//Make sure final byte is sent (BTF = 1 & TXE = 1) - BLOCKING
	while( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE ))
		;

	while( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF ))
		;

	//Decide whether to make a repeated start or to actually release the bus
	if( Sr == I2C_DISABLE_SR )
	{
		I2C_GenerateCondition(pI2CHandle, STOP);
	}
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- I2C_MasterReceiveData

 	 * @brief  		- API that configures a device as an I2C master and receives data from a slave

 	 * @param 		- *pI2CHandle : contains I2C peripheral base address in MCU memory
 	 * @param 		- *pRxBuffer : pointer to Rx buffer of user data
 	 * @param 		- Len : Length of user receive data the master expects in bytes
 	 * @param 		- SlaveAddr : Address of slave device that the master wants to send data to
 	 * @param 		- Sr : Sr = repeated start. If set, the API will generate a repeated start instead of releasing the bus and ending communication

 	 * @retval 		- none

 	 * @Note		- none

*/
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
// Make sure peripheral is enabled before attempting communication

//1. Generate start condition
	I2C_GenerateCondition(pI2CHandle, START);

//2. Master sends address phase

	//Must first wait until the start condition has been detected - BLOCKING
	while( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB ) )
		;
	//Clear start condition detection bit in SR1 to stop clock stretching to LOW
	I2C_ExecuteAddressPhaseRead(pI2CHandle, SlaveAddr);

//3. Determine if address was sent by master. If no match, don't try sending data - BLOCKING
	while( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR ))
		;

// Two cases in master receiving that need to be handled separately
	//Case 1 - Master will be receiving only 1 byte from slave
	//Case 2 - Master will be receiving >1 bytes from slave

	if( Len == 1) 	//Case 1 - we are only receiving 1 byte from slave
	{
		//4. (Case 1) To send NACK to slave, we have to disable ACK bit before ADDR bit is cleared
		pI2CHandle->pI2Cx->CR1 &= ~( 0x1 << I2C_CR1_ACK );

		//5. Clear ADDR flag
		I2C_ClearAddrFlag(pI2CHandle);

		//6. Wait until RxNE is set
		while( ! (I2C_GetFlagStatus( pI2CHandle->pI2Cx, I2C_FLAG_RXNE) ))
			;

		//7. Decide whether to make a repeated start or to actually release the bus
		if( Sr == I2C_DISABLE_SR )
		{
			I2C_GenerateCondition(pI2CHandle, STOP);
		}

		//8. Read data into buffer
		*pRxBuffer = pI2CHandle->pI2Cx->DR;
		pRxBuffer++;
	}

	if( Len > 1)			//Case 2 - we are only receiving >1 byte from slave
	{
		//4. Clear ADDR flag
		I2C_ClearAddrFlag(pI2CHandle);

		//5. Start receiving data
		while( Len > 2 )
		{
			//Read data after RxNE is set
			while( ! (I2C_GetFlagStatus( pI2CHandle->pI2Cx, I2C_FLAG_RXNE) ))
				;

			*pRxBuffer = pI2CHandle->pI2Cx->DR;
			pRxBuffer++;
			Len--;
		}

		//6. Check if it is the 2nd to last data byte to be sent. If so, generate NACK
		if( Len == 2 )
		{
			while( ! (I2C_GetFlagStatus( pI2CHandle->pI2Cx, I2C_FLAG_RXNE) ))
				;

			//Clear second to last RxNE
			*pRxBuffer = pI2CHandle->pI2Cx->DR;
			pRxBuffer++;
			Len--;

			//Generate NACK for last data
			pI2CHandle->pI2Cx->CR1 &= ~( 0x1 << I2C_CR1_ACK );

			//Decide whether to make a repeated start or to actually release the bus
			if( Sr == I2C_DISABLE_SR )
			{
				I2C_GenerateCondition(pI2CHandle, STOP);
			}
		}

		//Read last data byte
		while( ! (I2C_GetFlagStatus( pI2CHandle->pI2Cx, I2C_FLAG_RXNE) ))
			;

		*pRxBuffer = pI2CHandle->pI2Cx->DR;
		pRxBuffer++;
	}

	//Reconfigure ACK bit now that communication successfully closed if it was originally enabled
	if( pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE )
		pI2CHandle->pI2Cx->CR1 |= ( 0x1 << I2C_CR1_ACK );
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- I2C_SlaveSendData

 	 * @brief  		- API that sends data to a master (as a slave)

 	 * @param 		- *pI2Cx : contains I2C peripheral registers' base address in MCU memory
 	 * @param 		- data : data to be sent (byte by byte)

 	 * @retval 		- none

 	 * @Note		- none
*/
void I2C_SlaveSendData(I2C_RegDef_t *pI2Cx, uint8_t data)
{
	pI2Cx->DR = data;
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- I2C_SlaveReceiveData

 	 * @brief  		- API that receives data from a master (as a slave)

 	 * @param 		- *pI2Cx : contains I2C peripheral registers' base address in MCU memory

 	 * @retval 		- data : byte of data that has been received

 	 * @Note		- none
*/
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2Cx)
{
	return (uint8_t) pI2Cx->DR;
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- I2C_MasterSendDataIT

 	 * @brief  		- API that configures a device as an I2C master and receives data from a slave using interrupts

 	 * @param 		- *pI2CHandle : contains I2C peripheral base address in MCU memory
 	 * @param 		- *pRxBuffer : pointer to Rx buffer of user data
 	 * @param 		- Len : Length of user receive data the master expects in bytes
 	 * @param 		- SlaveAddr : Address of slave device that the master wants to send data to
 	 * @param 		- Sr : Sr = repeated start. If set, the API will generate a repeated start instead of releasing the bus and ending communication

 	 * @retval 		- state : Indicates to the rest of the program that the I2C is busy in Tx and no further data should try to be transmitted

 	 * @Note		- This function call is non-blocking
*/
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	uint8_t state = pI2CHandle->RxTxState;

	//1. Check to make sure the interface isn't already busy in Tx-ing or Rx-ing something else.
		//This way, no handle structure values can be touched while Tx or Rx request is still active waiting for interrupt
	if( ( state != I2C_BUSY_IN_TX ) && ( state != I2C_BUSY_IN_RX ) )
	{
		//2. Update the global handle structure so that updated values are accessible globally (available to the ISR)
		pI2CHandle->RxTxState = I2C_BUSY_IN_TX;
		pI2CHandle->Slave_Address = SlaveAddr;
		pI2CHandle->Sr = Sr;
		pI2CHandle->TxLen = Len;
		pI2CHandle->pTxBuffer = pTxBuffer;

		//3. Generate a start condition - this will trigger event-based interrupts which drive communications forward
		I2C_GenerateCondition(pI2CHandle, START);

		//4. Configure CR bits that enable interrupts
			//ITVTEN == 1 enables most SR1 event interrupts
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN );

			//(ITVTEN == 1 && ITBUFEN == 1 ) enables TxE and RxNE interrupts
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN );

			//ITERREN == 1 enables error interrupts
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN );
	}

	return state;
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- I2C_MasterReceiveDataIT

 	 * @brief  		- API that configures a device as an I2C master and receives data from a slave using interrupts

 	 * @param 		- *pI2CHandle : contains I2C peripheral base address in MCU memory
 	 * @param 		- *pRxBuffer : pointer to Rx buffer of user data
 	 * @param 		- Len : Length of user receive data the master expects in bytes
 	 * @param 		- SlaveAddr : Address of slave device that the master wants to send data to
 	 * @param 		- Sr : Sr = repeated start. If set, the API will generate a repeated start instead of releasing the bus and ending communication

 	 * @retval 		- state : Indicates to the rest of the program that the I2C is busy in Rx and no further data should try to be received

 	 * @Note		- This function call is non-blocking
*/
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	uint8_t state = pI2CHandle->RxTxState;

	//1. Check to make sure the interface isn't already busy in Tx-ing or Rx-ing something else.
		//This way, no handle structure values can be touched while Tx or Rx request is still active waiting for interrupt
	if( ( state != I2C_BUSY_IN_TX ) && ( state != I2C_BUSY_IN_RX ) )
	{
		//2. Update the global handle structure so that updated values are accessible globally (available to the ISR)
		pI2CHandle->RxTxState = I2C_BUSY_IN_RX;
		pI2CHandle->Slave_Address = SlaveAddr;
		pI2CHandle->Sr = Sr;
		pI2CHandle->RxLen = Len;
		pI2CHandle->RxSize = Len; //Rxsize is used in the ISR code to manage the data reception
		pI2CHandle->pRxBuffer = pRxBuffer;

		//3. Generate a start condition - this will trigger event-based interrupts which drive communications forward
		I2C_GenerateCondition(pI2CHandle, START);

		//4. Configure CR bits that enable interrupts
			//ITVTEN == 1 enables most SR1 event interrupts
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN );

			//(ITVTEN == 1 && ITBUFEN == 1 ) enables TxE and RxNE interrupts
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN );

			//ITERREN == 1 enables error interrupts
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN );

	}

	return state;
}


/*********************** Function Documentation ***************************************
 *
 	 * @fn			- I2C_IRQInterruptConfig

 	 * @brief  		- API that configures interrupts generated by a I2C peripheral. This function handles the configurations needed for interrupts on the processor side
 	 * 				- Refer to processor guide here for more details: https://www.engr.scu.edu/~dlewis/book3/docs/Cortex-M4_Devices_Generic_User_Guide.pdf

 	 * @param 		- IRQNumber : Number associated with the peripheral's exception handler in the NVIC vector table
 	 * @param 		- EnOrDi : macros to enable or disable the IRQ (ENABLE or DISABLE macros in MCU specific header file)

 	 * @retval 		- none

 	 * @Note		- none

*/
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi)
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
	 * @fn			- I2C_IRQPriorityConfig

	 * @brief  		- API that configures the priority level of a given IRQ (I2C-driven interrupt).

	 * @param 		- IRQPriority : Value that contains priority level of interrupt as compared to other interrupts. This is handled on the processor side in Cortex-M4 internal peripheral registers
	 * @param		- IRQNumber : Number associated with the peripheral's exception handler in the NVIC vector table

	 * @retval 		- none

	 * @Note		- none

*/
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//1. Find out which IPR register the IRQ is in
	uint8_t Offset = IRQNumber / 4;

	//2. Clear and write by shifting the priority value into the correct position
	uint8_t temp = (IRQNumber % 4) * 8;

	uint8_t shiftAmount = temp + ( 8 - NO_PR_BITS_IMPLEMENTED );														//Note: In the Cortex-M4, only 4 most significant bits of each of the 4 "sections" in the Interrupt priority register (IPR) are implemented, not the last 4. Because if this, you are limited to only 16 priority levels instead of 256 (this was for cost savings apparently). Keeping this in mind, you must also shift whatever priority level desired over to the left by 4 bits no matter the section of the byte it is in.
	*(NVIC_IPR_BASE_ADDR + (Offset) ) &= ~( 0xFF << shiftAmount );														//Clear the existing priority level
	*(NVIC_IPR_BASE_ADDR + (Offset) ) |= ( IRQPriority << shiftAmount );												//Write the desired priority level

}


/*********************** Function Documentation ***************************************
 *
	 * @fn			- I2C_EV_IRQHandling

	 * @brief  		- API that handles interrupts caused by events that are flagged in the I2C peripheral SR registers

	 * @param 		- *pI2CHandle : contains I2C peripheral base address in MCU memory as well as data handling information

	 * @retval 		- none

	 * @Note		- 1 of 2 I2C ISR functions in this driver. I2Cx peripherals have 2 NVIC lines - one for events and one for errors

*/
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	//Interrupt handling for both master and slave mode of a device

	uint32_t temp1 = pI2CHandle->pI2Cx->CR2 & ( 1 << I2C_CR2_ITEVTEN );
	uint32_t temp2 = pI2CHandle->pI2Cx->CR2 & ( 1 << I2C_CR2_ITBUFEN );

	//Determine which event has occurred

	if( I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB) && temp1 )						//SB event - only applicable in master mode
	{
		//Clear SB bit to avoid SCL from being stretched LOW
		if( pI2CHandle->RxTxState == I2C_BUSY_IN_TX )
		{
			I2C_ExecuteAddressPhaseWrite(pI2CHandle, pI2CHandle->Slave_Address);
		}
		else if( pI2CHandle->RxTxState == I2C_BUSY_IN_RX )
		{
			I2C_ExecuteAddressPhaseRead(pI2CHandle, pI2CHandle->Slave_Address);
		}
		else
			;
	}


	if( I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR ) && temp1 )						//ADDR event
	{
		//Check if device is master
		if( pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_MSL ) )
		{
			//Master mode : Address is sent
			//2 cases: Rx and Tx
			if( pI2CHandle->RxTxState == I2C_BUSY_IN_TX ) 			//Tx
			{
				I2C_ClearAddrFlag(pI2CHandle);
			}
			else if( pI2CHandle->RxTxState == I2C_BUSY_IN_RX )		//Rx
			{
				if( pI2CHandle->RxSize == 1) 		//Case 1 - we are only receiving 1 byte from slave
				{
					//Enable NACK
					pI2CHandle->pI2Cx->CR1 &= ~( 0x1 << I2C_CR1_ACK );

					I2C_ClearAddrFlag(pI2CHandle);
				}

				if( pI2CHandle->RxSize > 1 )		//Case 2 - we are receiving >1 bytes from slave
				{
					I2C_ClearAddrFlag(pI2CHandle);
				}
			}
			else
				;
		}
		else // device is in slave mode
		{
			//Slave mode : Address matched with own address
			I2C_ClearAddrFlag(pI2CHandle);
		}
	}


	if( I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_STOPF ) && temp1 )					//STOPF event - only applicable in slave mode
	{
		//Slave mode : Stop condition detected - reset required
		I2C_ClearStopfFlag(pI2CHandle);

		//Notify application that STOP was detected by slave
		I2C_ApplicationEventCallBack(pI2CHandle, I2C_EV_STOPF);
	}



	if( I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF ) && temp1 )						//BTF event
	{
		//Master transmit mode : Both DR and shift register are empty. Close transmission

		if( pI2CHandle->RxTxState == I2C_BUSY_IN_TX )
		{
			uint8_t TxE_set = I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE );
			uint8_t BTF_set = I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF );

			if( BTF_set && TxE_set && ( pI2CHandle->TxLen == 0 ) )
			{
				if( pI2CHandle->Sr == I2C_DISABLE_SR )
				{
					I2C_GenerateCondition(pI2CHandle, STOP);
				}

				//Reset all necessary member elements of the handle structure and disable event & buffer-driver interrupts
				I2C_CloseSendData(pI2CHandle);

				//Notify application that transmission is over
				I2C_ApplicationEventCallBack(pI2CHandle, I2C_EV_TX_COMPLETE);
			}
		}
		else if( pI2CHandle->RxTxState == I2C_BUSY_IN_RX )
		{
			//Master receive mode : Both DR and shift register are full (BTF should never be set in receive mode)
			//hang program here if this happens
			while(1);
		}
	}


	if( I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE ) && temp1 && temp2 )				//TxE event
	{
		//Master transmit mode : DR empty
		if( pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_MSL ) ) 	//Check if device is master
		{
			I2C_MasterHandleTxEInterrupt(pI2CHandle);
		}
		else													//Device is slave
		{
			//Slave transmit mode : DR empty
			if( pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_TRA ) ) //make sure slave is really in transmitter mode
			{
				I2C_ApplicationEventCallBack(pI2CHandle, I2C_EV_DATA_REQUEST);
			}
		}
	}


	if( I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE ) && temp1 && temp2 )			//RxNE event
	{
		//Master receive mode : DR not empty - data needs to be read
		if( pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_MSL ) ) 	//Check if device is master
		{
			I2C_MasterHandleRxNEInterrupt(pI2CHandle);
		}
		else												 	//Device is slave
		{
			//Slave receive mode : DR not empty - data needs to be read
			if( !( pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_TRA ) )) //make sure slave is really in receiver mode
			{
				I2C_ApplicationEventCallBack(pI2CHandle, I2C_EV_DATA_RECEIVE);
			}
		}


	}

}



/*********************** Function Documentation ***************************************
 *
	 * @fn			- I2C_ER_IRQHandling

	 * @brief  		- API that handles interrupts caused by errors that are flagged in the I2C peripheral SR registers

	 * @param 		- *pI2CHandle : contains I2C peripheral base address in MCU memory as well as data handling information

	 * @retval 		- none

	 * @Note		- 1 of 2 I2C ISR functions in this driver. I2Cx peripherals have 2 NVIC lines - one for events and one for errors

*/
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	//Interrupt handler for error-driven interrupts
	uint32_t temp1;
	uint32_t temp2;

	temp1 = pI2CHandle->pI2Cx->CR2 & ( 1 << I2C_CR2_ITERREN );

	//Determine which error has occurred

	/***********************Check for Bus error******************************/

		temp2 = (pI2CHandle->pI2Cx->SR1) & ( 1<< I2C_SR1_BERR);
		if( temp1  && temp2 )
		{
			//Clear the buss error flag
			pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_BERR);

			//Notify the application about the error
			I2C_ApplicationEventCallBack(pI2CHandle,I2C_ERROR_BERR);
		}

	/***********************Check for arbitration lost error****************************/

		temp2 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_ARLO );
		if(temp1  && temp2)
		{
			//Clear the arbitration lost error flag
			pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_ARLO);

			//Notify the application about the error
			I2C_ApplicationEventCallBack(pI2CHandle,I2C_ERROR_ARLO);
		}

	/***********************Check for ACK failure  error*****************************/

		temp2 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_AF);
		if(temp1  && temp2)
		{
			//Clear the ACK failure error flag
			pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_AF);

			//Notify the application about the error
			I2C_ApplicationEventCallBack(pI2CHandle,I2C_ERROR_AF);
		}

	/***********************Check for Overrun/underrun error***************************/

		temp2 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_OVR);
		if(temp1  && temp2)
		{
		    //Clear the Overrun/underrun error flag
			pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_OVR);

			//Notify the application about the error
			I2C_ApplicationEventCallBack(pI2CHandle,I2C_ERROR_OVR);
		}

	/***********************Check for Time out error******************************/

		temp2 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_TIMEOUT);
		if(temp1  && temp2)
		{
		    //Clear the Time out error flag
			pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_TIMEOUT);

			//Notify the application about the error
			I2C_ApplicationEventCallBack(pI2CHandle,I2C_ERROR_TIMEOUT);
		}
}




/*********************** Function Documentation ***************************************
 *
 	 * @fn			- I2C_PeripheralControl

 	 * @brief  		- API that enables or disables the I2Cx peripheral

 	 * @param 		- *pI2Cx : I2C peripheral base address in MCU memory
 	 * @param 		- EnOrDi : User-inputted value to enable or disable the device's I2C peripheral

 	 * @retval 		- none

 	 * @Note		- none

*/
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if( EnOrDi == ENABLE )
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
	}
	else if( EnOrDi == DISABLE )
	{
		pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
	}
	else
		;
}

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- I2C_GetFlagStatus

 	 * @brief  		- API that returns that value of a specific flag in the I2C peripheral status registers

 	 * @param 		- *pI2Cx : I2C peripheral base address in MCU memory
 	 * @param 		- FlagName : Name of the desired flag you want to check the status of

 	 * @retval 		- data that is read from flag - can only be either 1 or 0 (set or reset)

 	 * @Note		- none

*/
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName)
{
	if(	pI2Cx->SR1 & FlagName ) return FLAG_SET;
	else return FLAG_RESET;
}






//Helper functions





void I2C_GenerateCondition(I2C_Handle_t *pI2CHandle, uint8_t StartOrStop)
{
	if( StartOrStop == START )
	{
		pI2CHandle->pI2Cx->CR1 |= ( 1 << I2C_CR1_START );
		//automatically cleared by hardware when start is sent or peripheral is not enabled
	}
	else if ( StartOrStop == STOP )
	{
		pI2CHandle->pI2Cx->CR1 |= ( 1 << I2C_CR1_STOP );
		//Stop condition is generated after the current byte transfer or after the current Start condition is sent
		//automatically cleared by hardware when stop is detected
	}
	else
		;
}


static void I2C_ExecuteAddressPhaseWrite(I2C_Handle_t *pI2CHandle, uint8_t SlaveAddr)
{
	uint8_t AddressPhasePacket_Write = ( ( SlaveAddr << 1 ) & ~( 0x1 << 0 ) );
	pI2CHandle->pI2Cx->DR = AddressPhasePacket_Write;
}


static void I2C_ExecuteAddressPhaseRead(I2C_Handle_t *pI2CHandle, uint8_t SlaveAddr)
{
	uint8_t AddressPhasePacket_Read = ( ( SlaveAddr << 1 ) | ( 0x1 << 0 ) );
	pI2CHandle->pI2Cx->DR = AddressPhasePacket_Read;
}


static void I2C_ClearAddrFlag(I2C_Handle_t *pI2CHandle)
{
	uint32_t DummyRead = pI2CHandle->pI2Cx->SR1;
	DummyRead = pI2CHandle->pI2Cx->SR2;
	(void) DummyRead;
}

static void I2C_ClearStopfFlag(I2C_Handle_t *pI2CHandle)
{
	uint32_t DummyRead = pI2CHandle->pI2Cx->SR1;
	uint32_t DummyWrite = 0x0;
	pI2CHandle->pI2Cx->CR1 |= DummyWrite;
	(void) DummyRead;
}

static void I2C_MasterHandleRxNEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if( pI2CHandle->RxTxState == I2C_BUSY_IN_RX ) //Confirm device is in Rx mode
	{
		if( pI2CHandle->RxSize == 1)
		{
			//Release bus by sending start or stop condition
			if( pI2CHandle->Sr == I2C_DISABLE_SR )
			{
				I2C_GenerateCondition(pI2CHandle, STOP);
			}

			//Read only data byte
			*(pI2CHandle->pRxBuffer) = pI2CHandle->pI2Cx->DR;
			pI2CHandle->pRxBuffer++;
			pI2CHandle->RxLen--;

			//Reset all necessary member elements of the handle structure and disable event & buffer-driver interrupts
			I2C_CloseReceiveData(pI2CHandle);

			//Notify application that reception is over
			I2C_ApplicationEventCallBack(pI2CHandle, I2C_EV_RX_COMPLETE);
		}
		else if( pI2CHandle->RxSize > 1 )
		{
			if( pI2CHandle->RxLen > 2 )
			{
				*(pI2CHandle->pRxBuffer) = pI2CHandle->pI2Cx->DR;
				pI2CHandle->pRxBuffer++;
				pI2CHandle->RxLen--;
			}
			else if( pI2CHandle->RxLen == 2 )
			{
				*(pI2CHandle->pRxBuffer) = pI2CHandle->pI2Cx->DR;
				pI2CHandle->pRxBuffer++;
				pI2CHandle->RxLen--;

				//Enable NACK
				pI2CHandle->pI2Cx->CR1 &= ~( 0x1 << I2C_CR1_ACK );


			}
			else if( pI2CHandle->RxLen == 1 )
			{
				//Release bus by sending start or stop condition before last RxNE is cleared
				if( pI2CHandle->Sr == I2C_DISABLE_SR )
				{
					I2C_GenerateCondition(pI2CHandle, STOP);
				}

				//Read last data byte
				*(pI2CHandle->pRxBuffer) = pI2CHandle->pI2Cx->DR;
				pI2CHandle->pRxBuffer++;
				pI2CHandle->RxLen--;

				//Reset all necessary member elements of the handle structure and disable event & buffer-driver interrupts
				I2C_CloseReceiveData(pI2CHandle);

				//Notify application that reception is over
				I2C_ApplicationEventCallBack(pI2CHandle, I2C_EV_RX_COMPLETE);
			}
			else;
		}
		else //RxSize is 0 or invalid
			;
	}
}


static void I2C_MasterHandleTxEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if( pI2CHandle->RxTxState == I2C_BUSY_IN_TX )
	{
		if( pI2CHandle->TxLen > 0)
		{
			pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer); 		//Clear TxE
			pI2CHandle->pTxBuffer++;
			pI2CHandle->TxLen--;
		}

	//Master transmit mode : If TxLen == 0, control will exit this handler without clearing TxE and continue to BTF handler which closes Tx sequence
	}
}

void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle)
{
	//Disable interrupts
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITEVTEN );

	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITBUFEN );

	//Reset handle structure in preparation for future data
	pI2CHandle->RxLen = 0;
	pI2CHandle->RxSize = 0;
	pI2CHandle->pRxBuffer = NULL;

	if( pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE )	//re-enable ACKing
	{
		pI2CHandle->pI2Cx->CR1 |= ( 0x1 << I2C_CR1_ACK );
	}

	//Allow other I2C interrupt requests
	pI2CHandle->RxTxState = I2C_READY;
}


void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
{
	//Disable interrupts
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITEVTEN );

	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITBUFEN );

	//Reset handle structure in preparation for future data
	pI2CHandle->TxLen = 0;
	pI2CHandle->pTxBuffer = NULL;

	//Allow other I2C interrupt requests
	pI2CHandle->RxTxState = I2C_READY;
}


void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	//Configure CR bits that enable/disable interrupts
	if( EnOrDi == ENABLE )
	{
		pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN );
		pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN );
		pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN );
	}
	else if( EnOrDi == DISABLE )
	{
		pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITEVTEN );
		pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITBUFEN );
		pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITERREN );
	}
}


__weak void I2C_ApplicationEventCallBack(I2C_Handle_t *pI2CHandle, uint8_t AppEvent)
{
	//This is a weak implementation. The application may overwrite this function.
}
