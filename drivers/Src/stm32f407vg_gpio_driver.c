/*
 * stm32f407vg_gpio_driver.c
 *
 *  Created on: Jan 15, 2024
 *      Author: butle
 */

#include "stm32f407vg.h"

/*
 * Peripheral clock setup
 */

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- GPIO_PeriClockControl

 	 * @brief  		- enables or disables the clock of a GPIO peripheral port

 	 * @param 		- *pGPIOx : GPIO Port Base address
 	 * @param  		- EnOrDi : macros to enable or disable the clock (ENABLE or DISABLE macros in MCU specific header file

 	 * @retval 		- none

 	 * @Note		- none

*/
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnOrDi)
{
	if( EnOrDi == ENABLE )
	{
		if( (uint32_t) pGPIOx == GPIOA_BASE_ADDR )
		{
			GPIOA_PCLK_EN();
		}
		else if ( (uint32_t) pGPIOx == GPIOB_BASE_ADDR )
		{
			GPIOB_PCLK_EN();
		}
		else if ( (uint32_t) pGPIOx == GPIOC_BASE_ADDR )
		{
			GPIOC_PCLK_EN();
		}
		else if ( (uint32_t) pGPIOx == GPIOD_BASE_ADDR )
		{
			GPIOD_PCLK_EN();
		}
		else if ( (uint32_t) pGPIOx == GPIOE_BASE_ADDR )
		{
			GPIOE_PCLK_EN();
		}
		else if ( (uint32_t) pGPIOx == GPIOF_BASE_ADDR )
		{
			GPIOF_PCLK_EN();
		}
		else if ( (uint32_t) pGPIOx == GPIOG_BASE_ADDR )
		{
			GPIOG_PCLK_EN();
		}
		else if ( (uint32_t) pGPIOx == GPIOH_BASE_ADDR )
		{
			GPIOH_PCLK_EN();
		}
		else if ( (uint32_t) pGPIOx == GPIOI_BASE_ADDR )
		{
			GPIOI_PCLK_EN();
		}
		else
			;
	}

	else
	{
		if( (uint32_t) pGPIOx == GPIOA_BASE_ADDR )
		{
			GPIOA_PCLK_DI();
		}
		else if ( (uint32_t) pGPIOx == GPIOB_BASE_ADDR )
		{
			GPIOB_PCLK_DI();
		}
		else if ( (uint32_t) pGPIOx == GPIOC_BASE_ADDR )
		{
			GPIOC_PCLK_DI();
		}
		else if ( (uint32_t) pGPIOx == GPIOD_BASE_ADDR )
		{
			GPIOD_PCLK_DI();
		}
		else if ( (uint32_t) pGPIOx == GPIOE_BASE_ADDR )
		{
			GPIOE_PCLK_DI();
		}
		else if ( (uint32_t) pGPIOx == GPIOF_BASE_ADDR )
		{
			GPIOF_PCLK_DI();
		}
		else if ( (uint32_t) pGPIOx == GPIOG_BASE_ADDR )
		{
			GPIOG_PCLK_DI();
		}
		else if ( (uint32_t) pGPIOx == GPIOH_BASE_ADDR )
		{
			GPIOH_PCLK_DI();
		}
		else if ( (uint32_t) pGPIOx == GPIOI_BASE_ADDR )
		{
			GPIOI_PCLK_DI();
		}
		else
			;
	}
}

/*
 * GPIO Initialization and De-initialization (setting register back to reset state)
 */

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- GPIO_Init

 	 * @brief  		- API that initializes a specific GPIO peripheral with programmer-defined values

 	 * @param 		- *pGPIOHandle : Structure that contains GPIO Port Base address and desired configuration values

 	 * @retval 		- none

 	 * @Note		- none

*/
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	uint32_t temp = 0;

	// enable the peripheral clock

	GPIO_PeriClockControl(pGPIOHandle->pGPIOx, ENABLE);

	//Configure the alternate function of the pin

	if(pGPIOHandle -> GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN) //Only execute the following lines of code if the pin mode is set to alternate function
	{
		if(pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber <= 7) //Store configuration values in AFR LOW register for pin numbers less than or equal to 7
		{
			temp = (pGPIOHandle -> GPIO_PinConfig.GPIO_PinAltFunMode << (pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber * 4));
			pGPIOHandle -> pGPIOx -> AFR[0] &= ~( 0xF << ( pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber * 4) ); //clearing bits
			pGPIOHandle -> pGPIOx -> AFR[0] |= temp; //setting bits

			temp = 0;
		}
		else //Store configuration values in AFR HIGH register for pin numbers greater than than 7
		{
			temp = (pGPIOHandle -> GPIO_PinConfig.GPIO_PinAltFunMode << ((pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber - 8) * 4));
			pGPIOHandle -> pGPIOx -> AFR[1] &= ~( 0xF << ( (pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber - 8) * 4) ); //clearing bits
			pGPIOHandle -> pGPIOx -> AFR[1] |= temp; //setting bits

			temp = 0;
		}
	}
	else;

	// 1. Configure the mode of the pin

	if( pGPIOHandle -> GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG )
	{
		//Non interrupt pin modes
		temp = (pGPIOHandle -> GPIO_PinConfig.GPIO_PinMode << (pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber * 2));
		pGPIOHandle -> pGPIOx -> MODER &= ~( 0x3 << ( pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber * 2) ); 				//clearing bits
		pGPIOHandle -> pGPIOx -> MODER |= temp; 																		//setting bits

		temp = 0;
	}

	else
	{
		if( pGPIOHandle -> GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT )
		{
			//If the mode of the GPIO pin is selected as an interrupt on the falling edge, configure the falling trigger selection register (FTSR)
			EXTI -> FTSR |= (0x1 << pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber);

			//Clear the RTSR bit since we only want falling edge detection
			EXTI -> RTSR &= ~( 0x1 << pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber);
		}
		else if(  pGPIOHandle -> GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT )
		{
			//If the mode of the GPIO pin is selected as an interrupt on the rising edge, configure the rising trigger selection register (RTSR)
			EXTI -> RTSR |= (0x1 << pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber);

			//Clear the FTSR bit since we only want rising edge detection
			EXTI -> FTSR &= ~( 0x1 << pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber);
		}
		else if ( pGPIOHandle -> GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT )
		{
			//If the mode of the GPIO pin is selected as an interrupt on the rising or falling edge, configure both the rising trigger selection register (RTSR) and the falling trigger selection register (FTSR)
			EXTI -> FTSR |= (0x1 << pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber);
			EXTI -> RTSR |= (0x1 << pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber);
		}

		//Configure the GPIO port selection in SYSCFG_EXTICRx. I.e., the GPIO port that has the pin that you would like to generate an interrupt on
		uint8_t temp1 = pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber / 4; 									//variable that determines which SYSCFG_EXTICRx register to index into
		uint8_t temp2 = (pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber % 4) * 4; 							//Mask shift variable determines what section of register to shift masking bits into
		uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle -> pGPIOx);									//Macro implemented in MCU-specific header file

		SYSCFG_PCLK_EN(); 																					//enable the clock to the SYSCFG peripheral
		SYSCFG -> EXTICR[temp1] &= ~(0xF << temp2); 														//reset that EXTI line
		SYSCFG -> EXTICR[temp1] |= (portcode << temp2); 													//configure that EXTI line with the valid GPIO port


		//Enable the EXTI interrupt delivery using the EXTI_IMR register (interrupt mask register)
		EXTI -> IMR |= (0x1 << pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber);
	}

	// 2. Configure the speed of the pin

	temp = (pGPIOHandle -> GPIO_PinConfig.GPIO_PinSpeed << (pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber * 2));
	pGPIOHandle -> pGPIOx -> OSPEEDR &= ~( 0x3 << ( pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber * 2) ); //clearing bits
	pGPIOHandle -> pGPIOx -> OSPEEDR |= temp; //setting bits

	temp = 0;

	// 3. Configure the pull-up or pull-down resistor of the pin

	temp = (pGPIOHandle -> GPIO_PinConfig.GPIO_PinPuPdControl << (pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber * 2));
	pGPIOHandle -> pGPIOx -> PUPDR &= ~( 0x3 << ( pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber * 2) ); //clearing bits
	pGPIOHandle -> pGPIOx -> PUPDR |= temp; //setting bits

	temp = 0;

	// 4. Configure the output type of the pin (push-pull or open drain)

	temp = (pGPIOHandle -> GPIO_PinConfig.GPIO_PinOPType << (pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle -> pGPIOx -> OTYPER &= ~( 0x1 << (pGPIOHandle -> GPIO_PinConfig.GPIO_PinNumber) ); //clearing bits
	pGPIOHandle -> pGPIOx -> OTYPER |= temp; //setting bits

	temp = 0;
}

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- GPIO_DeInit

 	 * @brief  		- API that writes reset values into all of a GPIO peripheral's registers

 	 * @param 		- *pGPIOx : GPIO Port Base address

 	 * @retval 		- none

 	 * @Note		- This function uses peripheral reset registers in RCC. This reset a peripheral in one swoop instead of manually having to reset registers

*/
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
	if( (uint32_t) pGPIOx == GPIOA_BASE_ADDR )
	{
		GPIOA_REG_RESET();
	}
	else if ( (uint32_t) pGPIOx == GPIOB_BASE_ADDR )
	{
		GPIOB_REG_RESET();
	}
	else if ( (uint32_t) pGPIOx == GPIOC_BASE_ADDR )
	{
		GPIOC_REG_RESET();
	}
	else if ( (uint32_t) pGPIOx == GPIOD_BASE_ADDR )
	{
		GPIOD_REG_RESET();
	}
	else if ( (uint32_t) pGPIOx == GPIOE_BASE_ADDR )
	{
		GPIOE_REG_RESET();
	}
	else if ( (uint32_t) pGPIOx == GPIOF_BASE_ADDR )
	{
		GPIOF_REG_RESET();
	}
	else if ( (uint32_t) pGPIOx == GPIOG_BASE_ADDR )
	{
		GPIOG_REG_RESET();
	}
	else if ( (uint32_t) pGPIOx == GPIOH_BASE_ADDR )
	{
		GPIOH_REG_RESET();
	}
	else if ( (uint32_t) pGPIOx == GPIOI_BASE_ADDR )
	{
		GPIOI_REG_RESET();
	}
	else
		;
}

/*
 * Data Read and Write
 */

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- GPIO_ReadFromInputPin

 	 * @brief  		- API that reads data from a GPIO pin

 	 * @param 		- *pGPIOx : GPIO Port Base address
 	 * @param 		- pinNumber : Number of the pin to read from

 	 * @retval 		- data that is read from pin - can only be either 1 or 0

 	 * @Note		- none

*/
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber)
{
	uint8_t value;
	value = (uint8_t) ( (pGPIOx -> IDR >> pinNumber) & 0x00000001 ); //right shifting the pin we want to read so that it becomes the LSB; then we mask the rest of the register values

	return value;
}

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- GPIO_ReadFromInputPort

 	 * @brief  		- API that reads data from a GPIO port

 	 * @param 		- *pGPIOx : GPIO Port Base address

 	 * @retval 		- data that is read from entire port (there are 16 pins in a port)

 	 * @Note		- none

*/
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
	uint16_t value;
	value = (uint16_t) pGPIOx -> IDR; //store whole register into a single value

	return value;
}

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- GPIO_WriteToOutputPin

 	 * @brief  		- API that writes data to a GPIO pin

 	 * @param 		- *pGPIOx : GPIO Port Base address
 	 * @param 		- pinNumber : Number of pin to write data to
 	 * @param 		- Value : data to be written - can only be either 1 or 0

 	 * @retval 		- none

 	 * @Note		- none

*/
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber, uint8_t Value)
{
	if(Value == GPIO_PIN_SET)
	{
		pGPIOx -> ODR |= (1 << pinNumber); //write 1 to desired pin
	}
	else
	{
		pGPIOx -> ODR &= ~(1 << pinNumber); //write 0 to desired pin
	}
}

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- GPIO_WriteToOutputPort

 	 * @brief  		- API that writes data to a GPIO port

 	 * @param 		- *pGPIOx : GPIO Port Base address
 	 * @param 		- Value : data to be written - there are 16 total pins to write to on a port

 	 * @retval 		- none

 	 * @Note		- none

*/
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value)
{
	pGPIOx -> ODR = Value; //Write value given in parameter to all the pins of the port. No need to set and clear pins individually - assignment operator is sufficient
}

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- GPIO_ToggleOutputPin

 	 * @brief  		- API that toggles data on a GPIO pin

 	 * @param 		- *pGPIOx : GPIO Port Base address
 	 * @param 		- pinNumber : Number of pin to toggle value on

 	 * @retval 		- none

 	 * @Note		- none

*/
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber)
{
	pGPIOx -> ODR ^= ( 1 << pinNumber ); //XOR bitwise operator - ODR is XOR'd with zeros except in the bit position desired which is XOR'd with a 1. Anything XOR'd with a 0 will keep its original value. If the bit in the ODR is a 1, it will become a zero when XOR'd with a 1. If the bit in the ODR is a 0, it will become a 1 when XOR'd with a 1
}




/*
 * IRQ configuration and ISR handling
 */

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- GPIO_IRQConfig

 	 * @brief  		- API that configures interrupts generated by a GPIO. This function handles the configurations needed for interrupts on the processor side
 	 * 				- Refer to processor guide here for more details: https://www.engr.scu.edu/~dlewis/book3/docs/Cortex-M4_Devices_Generic_User_Guide.pdf

 	 * @param 		- IRQNumber : Number associated with the peripheral's exception handler in the NVIC vector table
 	 * @param 		- EnOrDi : macros to enable or disable the IRQ (ENABLE or DISABLE macros in MCU specific header file

 	 * @retval 		- none

 	 * @Note		- none

*/
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi)
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
	 * @fn			- GPIO_IRQPriorityConfig

	 * @brief  		- API that configures the priority level of a given IRQ (GPIO-driven interrupt).

	 * @param 		- IRQPriority : Value that contains priority level of interrupt as compared to other interrupts. Similar to GPIO_IRQInterruptConfig, this is also handled on the processor side in Cortex-M4 internal peripheral registers
	 * @param		- IRQNumber : Number associated with the peripheral's exception handler in the NVIC vector table

	 * @retval 		- none

	 * @Note		- none

*/
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//1. Find out which IPR register the IRQ is in
	uint8_t Offset = IRQNumber / 4;

	//2. Clear and write by shifting the priority value into the correct position
	uint8_t temp = (IRQNumber % 4) * 8;

	uint8_t shiftAmount = temp + ( 8 - NO_PR_BITS_IMPLEMENTED );															//Note: In the Cortex-M4, only 4 most significant bits of each of the 4 "sections" in the Interrupt priority register (IPR) are implemented, not the last 4. Because if this, you are limited to only 16 priority levels instead of 256 (this was for cost savings apparently). Keeping this in mind, you must also shift whatever priority level desired over to the left by 4 bits no matter the section of the byte it is in.
	*(NVIC_IPR_BASE_ADDR + (Offset) ) &= ~( 0xFF << shiftAmount );														//Clear the existing priority level
	*(NVIC_IPR_BASE_ADDR + (Offset) ) |= ( IRQPriority << shiftAmount );												//Write the desired priority level
}

/*********************** Function Documentation ***************************************
 *
 	 * @fn			- GPIO_IRQHandling

 	 * @brief  		- API that handles interrupts generated by GPIO pin.

 	 * @param 		- pinNumber : Number of pin that generates interrupt

 	 * @retval 		- none

 	 * @Note		- none

*/
void GPIO_IRQHandling(uint8_t pinNumber)
{
	//Clear the EXTI PR register corresponding to the pin number
	if( EXTI -> PR & ( 1 << pinNumber) )
	{
		//Clear the bit - you have to write a 1 to the desired EXTI line in the PR register. Refer to RM 12.3.6 for details.
		EXTI -> PR |= ( 1 << pinNumber );
	}

}
