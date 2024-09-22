/*
 * stm32f407vg.h
 *
 *  Created on: Jan 8, 2024
 *      Author: butle
 */

#ifndef INC_STM32F407VG_H_
#define INC_STM32F407VG_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define __vo 									volatile
#define __weak									__attribute__ ((weak))



/*		-----------------------------------		START: Processor Specific Details		-----------------------------------		*/

/*
 * ARM Cortex M4 processor NVIC interrupt set-enable register addresses
 */

#define NVIC_ISER0								( (__vo uint32_t*) 0xE000E100 )
#define NVIC_ISER1								( (__vo uint32_t*) 0xE000E104 )
#define NVIC_ISER2								( (__vo uint32_t*) 0xE000E108 )

/*
 * ARM Cortex M4 processor NVIC interrupt clear-enable register addresses
 */

#define NVIC_ICER0								( (__vo uint32_t*) 0xE000E180 )
#define NVIC_ICER1								( (__vo uint32_t*) 0xE000E104 )
#define NVIC_ICER2								( (__vo uint32_t*) 0xE000E108 )

/*
 * ARM Cortex M4 processor NVIC interrupt priority registers base address
 */

#define NVIC_IPR_BASE_ADDR						( (__vo uint32_t*) 0xE000E400 )

#define NO_PR_BITS_IMPLEMENTED					4


/*		-----------------------------------		END: Processor Specific Details		-----------------------------------		*/




/*
 * Base addresses of Flash, SRAM, and ROM memories
 */

#define FLASH_BASE_ADDR 						0x08000000U							//Base address of FLASH memory (code space)
#define SRAM1_BASE_ADDR 						0x20000000U 						//Base address of SRAM1
#define SRAM2_BASE_ADDR							0x2001C000U 						//Base address of SRAM1
#define ROM_BASE_ADDR							0x1FFF0000U							//Base address of ROM (system memory)
#define SRAM_BASE_ADDR							SRAM1_BASE_ADDR						//SRAM = SRAM1 for the purposes of the drivers

/*
 * AHBx and APBx bus peripheral base addresses
 */

#define PERIPH_BASE_ADDR 						0x40000000U							//Base address of all peripherals supported by STM32F407
#define APB1PERIPH_BASE_ADDR 					PERIPH_BASE_ADDR 					//Base address of APB1 peripheral base
#define APB2PERIPH_BASE_ADDR					0x40010000U 						//Base address of APB2 peripheral base
#define AHB1PERIPH_BASE_ADDR					0x40020000U 						//Base address of AHB1 peripheral base
#define AHB2PERIPH_BASE_ADDR 					0x50000000U							//Base address of AHB2 peripheral base

/*
 * Base addresses of peripherals which are hanging on AHB1 bus (peripherals listed are only those that used in this project)
 */

#define GPIOA_BASE_ADDR	 						(AHB1PERIPH_BASE_ADDR + 0x0000) 	//Base address of GPIOA port peripheral
#define GPIOB_BASE_ADDR 						(AHB1PERIPH_BASE_ADDR + 0x0400) 	//Base address of GPIOB port peripheral
#define GPIOC_BASE_ADDR							(AHB1PERIPH_BASE_ADDR + 0x0800) 	//Base address of GPIOC port peripheral
#define GPIOD_BASE_ADDR							(AHB1PERIPH_BASE_ADDR + 0x0C00) 	//Base address of GPIOD port peripheral
#define GPIOE_BASE_ADDR 						(AHB1PERIPH_BASE_ADDR + 0x1000)		//Base address of GPIOE port peripheral
#define GPIOF_BASE_ADDR 						(AHB1PERIPH_BASE_ADDR + 0x1400) 	//Base address of GPIOF port peripheral
#define GPIOG_BASE_ADDR							(AHB1PERIPH_BASE_ADDR + 0x1800) 	//Base address of GPIOG port peripheral
#define GPIOH_BASE_ADDR							(AHB1PERIPH_BASE_ADDR + 0x1C00) 	//Base address of GPIOH port peripheral
#define GPIOI_BASE_ADDR 						(AHB1PERIPH_BASE_ADDR + 0x2000)		//Base address of GPIOI port peripheral

#define RCC_BASE_ADDR							(AHB1PERIPH_BASE_ADDR + 0x3800)		//Base address of RCC peripheral

/*
 * Base addresses of peripherals which are hanging on APB1 bus (peripherals listed are only those that used in this project)
 */

#define I2C1_BASE_ADDR	 						(APB1PERIPH_BASE_ADDR + 0x5400) 	//Base address of I2C1 peripheral
#define I2C2_BASE_ADDR 							(APB1PERIPH_BASE_ADDR + 0x5800) 	//Base address of I2C2 peripheral
#define I2C3_BASE_ADDR							(APB1PERIPH_BASE_ADDR + 0x5C00) 	//Base address of I2C3 peripheral

#define SPI2_BASE_ADDR							(APB1PERIPH_BASE_ADDR + 0x3800) 	//Base address of SPI2 peripheral
#define SPI3_BASE_ADDR 							(APB1PERIPH_BASE_ADDR + 0x3C00)		//Base address of SPI3 peripheral

#define USART2_BASE_ADDR 						(APB1PERIPH_BASE_ADDR + 0x4400) 	//Base address of USART2 peripheral
#define USART3_BASE_ADDR						(APB1PERIPH_BASE_ADDR + 0x4800) 	//Base address of USART3 peripheral
#define UART4_BASE_ADDR							(APB1PERIPH_BASE_ADDR + 0x4C00) 	//Base address of UART4 peripheral
#define UART5_BASE_ADDR 						(APB1PERIPH_BASE_ADDR + 0x5000)		//Base address of UART5 peripheral

#define TIM2_BASE_ADDR							(APB1PERIPH_BASE_ADDR)				//Base address of TIM2 peripheral
#define TIM5_BASE_ADDR							(APB1PERIPH_BASE_ADDR + 0x0C00)		//Base address of TIM5 peripheral

/*
 * Base addresses of peripherals which are hanging on APB2 bus (peripherals listed are only those that used in this project)
 */

#define EXTI_BASE_ADDR 							(APB2PERIPH_BASE_ADDR + 0x3C00)		//Base address of EXTI peripheral

#define SPI1_BASE_ADDR	 						(APB2PERIPH_BASE_ADDR + 0x3000) 	//Base address of SPI1 peripheral

#define SYSCFG_BASE_ADDR						(APB2PERIPH_BASE_ADDR + 0x3800) 	//Base address of SYSCFG peripheral

#define USART1_BASE_ADDR 						(APB2PERIPH_BASE_ADDR + 0x1000) 	//Base address of USART1 peripheral
#define USART6_BASE_ADDR						(APB2PERIPH_BASE_ADDR + 0x1400) 	//Base address of USART6 peripheral

#define ADC1_BASE_ADDR							(APB2PERIPH_BASE_ADDR + 0x2000) 	//Base address of ADC1 peripheral
#define ADC2_BASE_ADDR							(APB2PERIPH_BASE_ADDR + 0x2100) 	//Base address of ADC2 peripheral
#define ADC3_BASE_ADDR							(APB2PERIPH_BASE_ADDR + 0x2200) 	//Base address of ADC3 peripheral
#define ADC_COMMON_REG_BASE_ADDR				(ADC1_BASE_ADDR + 0x300) 			//Base address of ADC common registers


/*		-----------------------------------		Peripheral register structure definitions		-----------------------------------		*/

/*
 * Accomplished by creating generic structs that can be used by each peripheral that describes its registers
 * Macros for the pointer variables that point to the memory location at which each peripheral is located at
 */

/*
 * Generic GPIO peripheral registers structure definition
 */

typedef struct
{
	__vo uint32_t	MODER;				/* Mode register												Address Offset: 0x00 */
	__vo uint32_t 	OTYPER;				/* Output type register											Address Offset: 0x04 */
	__vo uint32_t 	OSPEEDR;			/* Output speed register										Address Offset: 0x08 */
	__vo uint32_t 	PUPDR;				/* Pull-up/pull-down register									Address Offset: 0x0C */
	__vo uint32_t 	IDR;				/* Input data register											Address Offset: 0x10 */
	__vo uint32_t 	ODR;				/* Output data register											Address Offset: 0x14 */
	__vo uint32_t 	BSRR;				/* Bit set/reset register										Address Offset: 0x18 */
	__vo uint32_t 	LCKR;				/* Configuration lock register									Address Offset: 0x1C */
	__vo uint32_t 	AFR[2];				/* Alternate function mode registers (high and low)				Address Offset: 0x20 			AFR[0]: GPIO alternate function low register, AFR[1]: GPIO alternate function high register */
}GPIO_RegDef_t;

/*
 * RCC peripheral registers structure definition
 */

typedef struct
{
	__vo uint32_t 	CR;					/* RCC clock control register										Address Offset: 0x00 */
	__vo uint32_t 	PLLCFGR;			/* RCC PLL configuration register									Address Offset: 0x04 */
	__vo uint32_t 	CFGR;				/* RCC clock configuration register									Address Offset: 0x08 */
	__vo uint32_t 	CIR;				/* RCC clock interrupt register										Address Offset: 0x0C */
	__vo uint32_t 	AHB1RSTR;			/* RCC AHB1 peripheral reset register								Address Offset: 0x10 */
	__vo uint32_t 	AHB2RSTR;			/* RCC AHB2 peripheral reset register								Address Offset: 0x14 */
	__vo uint32_t 	AHB3RSTR;			/* RCC AHB3 peripheral reset register								Address Offset: 0x18 */
	uint32_t 		RESERVED0;			/* Reserved memory space											Address Offset: 0x1C */
	__vo uint32_t 	APB1RSTR;			/* RCC APB1 peripheral reset register								Address Offset: 0x20 */
	__vo uint32_t 	APB2RSTR;			/* RCC APB2 peripheral reset register								Address Offset: 0x24 */
	uint32_t 		RESERVED1[2];		/* Reserved memory space											Address Offset: 0x28 & 0x2C */
	__vo uint32_t 	AHB1ENR;			/* RCC AHB1 peripheral clock enable register						Address Offset: 0x30 */
	__vo uint32_t 	AHB2ENR;			/* RCC AHB2 peripheral clock enable register						Address Offset: 0x34 */
	__vo uint32_t 	AHB3ENR;			/* RCC AHB3 peripheral clock enable register						Address Offset: 0x38 */
	uint32_t 		RESERVED2;			/* Reserved memory space											Address Offset: 0x3C */
	__vo uint32_t 	APB1ENR;			/* RCC APB1 peripheral clock enable register						Address Offset: 0x40 */
	__vo uint32_t 	APB2ENR;			/* RCC APB2 peripheral clock enable register						Address Offset: 0x44 */
	uint32_t		RESERVED3[2];		/* Reserved memory space											Address Offset: 0x48 & 0x4C */
	__vo uint32_t	AHB1LPENR;			/* RCC AHB1 peripheral clock enable in low power mode register		Address Offset: 0x50 */
	__vo uint32_t 	AHB2LPENR;			/* RCC AHB2 peripheral clock enable in low power mode register		Address Offset: 0x54 */
	__vo uint32_t 	AHB3LPENR;			/* RCC AHB3 peripheral clock enable in low power mode register		Address Offset: 0x58 */
	uint32_t 		RESERVED4;			/* Reserved memory space											Address Offset: 0x5C */
	__vo uint32_t 	APB1LPENR;			/* RCC APB1 peripheral clock enable in low power mode register		Address Offset: 0x60 */
	__vo uint32_t 	APB2LPENR;			/* RCC APB2 peripheral clock enable in low power mode register		Address Offset: 0x64 */
	uint32_t 		RESERVED5[2];		/* Reserved memory space											Address Offset: 0x68 & 0x6C */
	__vo uint32_t 	BDCR;				/* RCC Backup domain control register								Address Offset: 0x70 */
	__vo uint32_t 	CSR;				/* RCC clock control & status register								Address Offset: 0x74 */
	uint32_t 		RESERVED6[2];		/* Reserved memory space											Address Offset: 0x78 & 0x7C */
	__vo uint32_t 	SSCGR;				/* RCC spread spectrum clock generation register					Address Offset: 0x80 */
	__vo uint32_t 	PLLI2SCFGR;			/* RCC PLLI2S configuration register								Address Offset: 0x84 */
}RCC_RegDef_t;

/*
 * EXTI peripheral registers structure definition
 */

typedef struct
{
	__vo uint32_t	IMR;				/* EXTI interrupt mask register										Address Offset: 0x00 */
	__vo uint32_t 	EMR;				/* EXTI event mask register											Address Offset: 0x04 */
	__vo uint32_t 	RTSR;				/* EXTI rising trigger selection register							Address Offset: 0x08 */
	__vo uint32_t 	FTSR;				/* EXTI falling trigger selection register							Address Offset: 0x0C */
	__vo uint32_t 	SWIER;				/* EXTI software interrupt event register							Address Offset: 0x10 */
	__vo uint32_t 	PR;					/* EXTI pending register											Address Offset: 0x14 */
}EXTI_RegDef_t;

/*
 * SYSCFG peripheral registers structure definition
 */

typedef struct
{
	__vo uint32_t 	MEMRMP;				/* SYSCFG memory re-map register 									Address Offset: 0x00 */
	__vo uint32_t	PMC;				/* SYSCFG peripheral mode configuration register					Address Offset: 0x04 */
	__vo uint32_t 	EXTICR[4];			/* SYSCFG external interrupt configuration registers				Address Offset: 0x08 - 0x14 */
	uint32_t 		RESERVED1[2];		/* Reserved memory space											Address Offset: 0x1C & 0x18 */
	__vo uint32_t	CMPCR;				/* Compensation cell control register								Address Offset: 0x20 */
}SYSCFG_RegDef_t;

/*
 * SPI peripheral registers structure definition
 */

typedef struct
{
	__vo uint32_t 	CR[2];				/* SPI control registers (not used in I2S mode)						Address Offset: 0x00 - 0x04 */
	__vo uint32_t 	SR;					/* SPI status register												Address Offset: 0x08 */
	__vo uint32_t 	DR;					/* SPI data register												Address Offset: 0x0C */
	__vo uint32_t 	CRCPR;				/* SPI CRC polynomial register(not used in I2S mode)				Address Offset: 0x10 */
	__vo uint32_t 	RXCRCR;				/* SPI RX CRC register (not used in I2S mode)						Address Offset: 0x14 */
	__vo uint32_t 	TXCRCR;				/* SPI TX CRC register (not used in I2S mode)						Address Offset: 0x18 */
	__vo uint32_t 	I2SCFGR;			/* SPI_I2S configuration register									Address Offset: 0x1C */
	__vo uint32_t 	I2SPR;				/* SPI_I2S prescaler register										Address Offset: 0x20 */
}SPI_RegDef_t;

/*
 * I2C peripheral registers structure definition
 */

typedef struct
{
	__vo uint32_t 	CR1;				/* I2C Control register 1											Address Offset: 0x00 */
	__vo uint32_t 	CR2;				/* I2C Control register 2											Address Offset: 0x04 */
	__vo uint32_t 	OAR1;				/* I2C Own address register 1										Address Offset: 0x08 */
	__vo uint32_t 	OAR2;				/* I2C Own address register 2										Address Offset: 0x0C */
	__vo uint32_t 	DR;					/* I2C Data register												Address Offset: 0x10 */
	__vo uint32_t 	SR1;				/* I2C Status register 1 											Address Offset: 0x14 */
	__vo uint32_t 	SR2;				/* I2C Status register 2											Address Offset: 0x18 */
	__vo uint32_t 	CCR;				/* I2C Clock control register										Address Offset: 0x1C */
	__vo uint32_t 	TRISE;				/* I2C TRISE register												Address Offset: 0x20 */
	__vo uint32_t 	FLTR;				/* I2C FLTR register												Address Offset: 0x24 */
}I2C_RegDef_t;

/*
 * USART peripheral registers structure definition
 */

typedef struct
{
	__vo uint32_t 	SR;					/* USART Status register 											Address Offset: 0x00 */
	__vo uint32_t 	DR;					/* USART Data register 												Address Offset: 0x04 */
	__vo uint32_t 	BRR;				/* USART Baud rate register											Address Offset: 0x08 */
	__vo uint32_t 	CR1;				/* USART Control register 1											Address Offset: 0x0C */
	__vo uint32_t 	CR2;				/* USART Control register 2											Address Offset: 0x10 */
	__vo uint32_t 	CR3;				/* USART Control register 3 										Address Offset: 0x14 */
	__vo uint32_t 	GTPR;				/* USART Guard time and prescaler register							Address Offset: 0x18 */
}USART_RegDef_t;

/*
 * ADC specific peripheral registers structure definition
 */

typedef struct
{
	__vo uint32_t 	SR;					/* ADC status register												Address Offset: 0x00 */
	__vo uint32_t 	CR1;				/* ADC control register 1 											Address Offset: 0x04 */
	__vo uint32_t 	CR2;				/* ADC control register 2											Address Offset: 0x08 */
	__vo uint32_t 	SMPR1;				/* ADC sample time register 1										Address Offset: 0x0C */
	__vo uint32_t 	SMPR2;				/* ADC sample time register 2										Address Offset: 0x10 */
	__vo uint32_t 	JOFR[4];			/* ADC injected channel data offset registers 						Address Offset: 0x14 - 0x20 */
	__vo uint32_t 	HTR;				/* ADC watchdog higher threshold register							Address Offset: 0x24 */
	__vo uint32_t 	LTR;				/* ADC watchdog lower threshold register							Address Offset: 0x28 */
	__vo uint32_t 	SQR1;				/* ADC regular sequence register 1									Address Offset: 0x2C */
	__vo uint32_t 	SQR2;				/* ADC regular sequence register 2									Address Offset: 0x30 */
	__vo uint32_t 	SQR3;				/* ADC regular sequence register 3									Address Offset: 0x34 */
	__vo uint32_t 	JSQR;				/* ADC injected sequence register									Address Offset: 0x38 */
	__vo uint32_t 	JDR[4];				/* ADC injected data registers										Address Offset: 0x3C - 0x48 */
	__vo uint32_t 	DR;					/* ADC regular data register										Address Offset: 0x4C */
}ADC_RegDef_t;

/*
 * ADC common peripheral registers structure definition
 */

typedef struct
{
	__vo uint32_t 	CSR;				/* ADC common status register										Address Offset: 0x00 */
	__vo uint32_t 	CCR;				/* ADC common control register 										Address Offset: 0x04 */
	__vo uint32_t 	CDR;				/* ADC common data register											Address Offset: 0x08 */
}ADC_Common_RegDef_t;

/*
 * TIM2 to TIM5 specific peripheral registers structure definition
 */

typedef struct
{
	__vo uint32_t 	CR1;				/* TIM control register 1											Address Offset: 0x00 */
	__vo uint32_t 	CR2;				/* TIM control register 2											Address Offset: 0x04 */
	__vo uint32_t 	SMCR;				/* TIM slave mode control register									Address Offset: 0x08 */
	__vo uint32_t 	DIER;				/* TIM DMA/Interrupt enable register								Address Offset: 0x0C */
	__vo uint32_t 	SR;					/* TIM status register												Address Offset: 0x10 */
	__vo uint32_t 	EGR;				/* TIM event generation register 									Address Offset: 0x14 */
	__vo uint32_t 	CCMR1;				/* TIM capture/compare mode register 1								Address Offset: 0x18 */
	__vo uint32_t 	CCMR2;				/* TIM capture/compare mode register 2								Address Offset: 0x1C */
	__vo uint32_t 	CCER;				/* TIM capture/compare enable register								Address Offset: 0x20 */
	__vo uint32_t 	CNT;				/* TIM counter														Address Offset: 0x24 */
	__vo uint32_t 	PSC;				/* TIM pre-scaler													Address Offset: 0x28 */
	__vo uint32_t 	ARR;				/* TIM auto-reload register											Address Offset: 0x2C */
	__vo uint32_t 	RESERVED1;			/* Reserved memory space											Address Offset: 0x30 */
	__vo uint32_t 	CCR1;				/* TIM capture/compare register 1									Address Offset: 0x34 */
	__vo uint32_t 	CCR2;				/* TIM capture/compare register 2									Address Offset: 0x38 */
	__vo uint32_t 	CCR3;				/* TIM capture/compare register 3									Address Offset: 0x3C */
	__vo uint32_t 	CCR4;				/* TIM capture/compare register 4									Address Offset: 0x40 */
	__vo uint32_t 	RESERVED2;			/* Reserved memory space											Address Offset: 0x44 */
	__vo uint32_t 	DCR;				/* TIM DMA control register											Address Offset: 0x48 */
	__vo uint32_t 	DMAR;				/* TIM DMA address for full transfer								Address Offset: 0x4C */
	__vo uint32_t 	TIM_OR;				/* TIM option register												Address Offset: 0x50 */
}TIM2_5_RegDef_t;


/*
 * Peripheral definitions (peripheral base addresses type-casted to the appropriate register structure)
 */

#define GPIOA									( (GPIO_RegDef_t* )GPIOA_BASE_ADDR )
#define GPIOB									( (GPIO_RegDef_t* )GPIOB_BASE_ADDR )
#define GPIOC									( (GPIO_RegDef_t* )GPIOC_BASE_ADDR )
#define GPIOD									( (GPIO_RegDef_t* )GPIOD_BASE_ADDR )
#define GPIOE									( (GPIO_RegDef_t* )GPIOE_BASE_ADDR )
#define GPIOF									( (GPIO_RegDef_t* )GPIOF_BASE_ADDR )
#define GPIOG									( (GPIO_RegDef_t* )GPIOG_BASE_ADDR )
#define GPIOH									( (GPIO_RegDef_t* )GPIOH_BASE_ADDR )
#define GPIOI									( (GPIO_RegDef_t* )GPIOI_BASE_ADDR )

#define RCC										( (RCC_RegDef_t* ) RCC_BASE_ADDR )

#define EXTI									( (EXTI_RegDef_t* ) EXTI_BASE_ADDR )

#define SYSCFG									( (SYSCFG_RegDef_t* ) SYSCFG_BASE_ADDR )

#define SPI1									( (SPI_RegDef_t* ) SPI1_BASE_ADDR )
#define SPI2									( (SPI_RegDef_t* ) SPI2_BASE_ADDR )
#define SPI3									( (SPI_RegDef_t* ) SPI3_BASE_ADDR )

#define I2C1									( (I2C_RegDef_t* ) I2C1_BASE_ADDR )
#define I2C2									( (I2C_RegDef_t* ) I2C2_BASE_ADDR )
#define I2C3									( (I2C_RegDef_t* ) I2C3_BASE_ADDR )

#define USART1									( (USART_RegDef_t* ) USART1_BASE_ADDR )
#define USART2									( (USART_RegDef_t* ) USART2_BASE_ADDR )
#define USART3									( (USART_RegDef_t* ) USART3_BASE_ADDR )
#define UART4									( (USART_RegDef_t* ) UART4_BASE_ADDR )
#define UART5									( (USART_RegDef_t* ) UART5_BASE_ADDR )
#define USART6									( (USART_RegDef_t* ) USART6_BASE_ADDR )

#define ADC1									( (ADC_RegDef_t* ) ADC1_BASE_ADDR )
#define ADC2									( (ADC_RegDef_t* ) ADC2_BASE_ADDR )
#define ADC3									( (ADC_RegDef_t* ) ADC3_BASE_ADDR )
#define ADCCOMMON								( (ADC_Common_RegDef_t* ) ADC_COMMON_REG_BASE_ADDR )

#define TIM2 									( ( TIM2_5_RegDef_t *) TIM2_BASE_ADDR )
#define TIM5 									( ( TIM2_5_RegDef_t *) TIM5_BASE_ADDR )

/*
 * Clock enable macros for GPIOx peripherals
 */

#define GPIOA_PCLK_EN()							( RCC -> AHB1ENR |= ( 1 << 0 ) )			//Enabling clock to GPIOA peripheral
#define GPIOB_PCLK_EN()							( RCC -> AHB1ENR |= ( 1 << 1 ) )			//Enabling clock to GPIOB peripheral
#define GPIOC_PCLK_EN()							( RCC -> AHB1ENR |= ( 1 << 2 ) )			//Enabling clock to GPIOC peripheral
#define GPIOD_PCLK_EN()							( RCC -> AHB1ENR |= ( 1 << 3 ) )			//Enabling clock to GPIOD peripheral
#define GPIOE_PCLK_EN()							( RCC -> AHB1ENR |= ( 1 << 4 ) )			//Enabling clock to GPIOE peripheral
#define GPIOF_PCLK_EN()							( RCC -> AHB1ENR |= ( 1 << 5 ) )			//Enabling clock to GPIOF peripheral
#define GPIOG_PCLK_EN()							( RCC -> AHB1ENR |= ( 1 << 6 ) )			//Enabling clock to GPIOG peripheral
#define GPIOH_PCLK_EN()							( RCC -> AHB1ENR |= ( 1 << 7 ) )			//Enabling clock to GPIOH peripheral
#define GPIOI_PCLK_EN()							( RCC -> AHB1ENR |= ( 1 << 8 ) )			//Enabling clock to GPIOI peripheral

/*
 * Clock enable macros for I2Cx peripherals
 */

#define I2C1_PCLK_EN()							( RCC -> APB1ENR |= ( 1 << 21 ) )			//Enabling clock to I2C1 peripheral
#define I2C2_PCLK_EN()							( RCC -> APB1ENR |= ( 1 << 22 ) )			//Enabling clock to I2C2 peripheral
#define I2C3_PCLK_EN()							( RCC -> APB1ENR |= ( 1 << 22 ) )			//Enabling clock to I2C3 peripheral

/*
 * Clock enable macros for SPIx peripherals
 */

#define SPI1_PCLK_EN()							( RCC -> APB2ENR |= ( 1 << 12 ) )			//Enabling clock to SPI1 peripheral

#define SPI2_PCLK_EN()							( RCC -> APB1ENR |= ( 1 << 14 ) )			//Enabling clock to SPI2 peripheral
#define SPI3_PCLK_EN()							( RCC -> APB1ENR |= ( 1 << 15 ) )			//Enabling clock to SPI3 peripheral

/*
 * Clock enable macros for USARTx peripherals
 */

#define USART1_PCLK_EN()						( RCC -> APB2ENR |= ( 1 << 4 ) )			//Enabling clock to USART1 peripheral

#define USART2_PCLK_EN()						( RCC -> APB1ENR |= ( 1 << 17 ) )			//Enabling clock to USART2 peripheral
#define USART3_PCLK_EN()						( RCC -> APB1ENR |= ( 1 << 18 ) )			//Enabling clock to USART3 peripheral
#define UART4_PCLK_EN()							( RCC -> APB1ENR |= ( 1 << 19 ) )			//Enabling clock to UART4 peripheral
#define UART5_PCLK_EN()							( RCC -> APB1ENR |= ( 1 << 20 ) )			//Enabling clock to UART5 peripheral

#define USART6_PCLK_EN()						( RCC -> APB2ENR |= ( 1 << 5 ) )			//Enabling clock to USART6 peripheral

/*
 * Clock enable macros for ADCx peripherals
 */

#define ADC1_PCLK_EN()							( RCC -> APB2ENR |= ( 1 << 8 ) )			//Enabling clock to ADC1 peripheral
#define ADC2_PCLK_EN()							( RCC -> APB2ENR |= ( 1 << 9 ) )			//Enabling clock to ADC2 peripheral
#define ADC3_PCLK_EN()							( RCC -> APB2ENR |= ( 1 << 10 ) )			//Enabling clock to ADC3 peripheral


/*
 * Clock enable macro for SYSCFG peripheral
 */

#define SYSCFG_PCLK_EN()						( RCC -> APB2ENR |= ( 1 << 14 ) )			//Enabling clock to SYSCFG peripheral


/*
 * Clock enable macro for TIMx peripherals
 */

#define TIM2_PCLK_EN()							( RCC -> APB1ENR |= ( 1 << 0 ) )			//Enabling clock to TIM2 peripheral
#define TIM5_PCLK_EN()							( RCC -> APB1ENR |= ( 1 << 3 ) )			//Enabling clock to TIM5 peripheral


/*
 * Clock disable macros for GPIOx peripherals
 */

#define GPIOA_PCLK_DI()							( RCC -> AHB1ENR &= ~( 1 << 0 ) )			//Disabling clock to GPIOA peripheral
#define GPIOB_PCLK_DI()							( RCC -> AHB1ENR &= ~( 1 << 1 ) )			//Disabling clock to GPIOB peripheral
#define GPIOC_PCLK_DI()							( RCC -> AHB1ENR &= ~( 1 << 2 ) )			//Disabling clock to GPIOC peripheral
#define GPIOD_PCLK_DI()							( RCC -> AHB1ENR &= ~( 1 << 3 ) )			//Disabling clock to GPIOD peripheral
#define GPIOE_PCLK_DI()							( RCC -> AHB1ENR &= ~( 1 << 4 ) )			//Disabling clock to GPIOE peripheral
#define GPIOF_PCLK_DI()							( RCC -> AHB1ENR &= ~( 1 << 5 ) )			//Disabling clock to GPIOF peripheral
#define GPIOG_PCLK_DI()							( RCC -> AHB1ENR &= ~( 1 << 6 ) )			//Disabling clock to GPIOG peripheral
#define GPIOH_PCLK_DI()							( RCC -> AHB1ENR &= ~( 1 << 7 ) )			//Disabling clock to GPIOH peripheral
#define GPIOI_PCLK_DI()							( RCC -> AHB1ENR &= ~( 1 << 8 ) )			//Disabling clock to GPIOI peripheral

/*
 * Clock disable macros for I2Cx peripherals
 */

#define I2C1_PCLK_DI()							( RCC -> APB1ENR &= ~( 1 << 21 ) )			//Disabling clock to I2C1 peripheral
#define I2C2_PCLK_DI()							( RCC -> APB1ENR &= ~( 1 << 22 ) )			//Disabling clock to I2C2 peripheral
#define I2C3_PCLK_DI()							( RCC -> APB1ENR &= ~( 1 << 22 ) )			//Disabling clock to I2C3 peripheral

/*
 * Clock disable macros for SPIx peripherals
 */

#define SPI1_PCLK_DI()							( RCC -> APB2ENR &= ~( 1 << 12 ) )			//Disabling clock to SPI1 peripheral

#define SPI2_PCLK_DI()							( RCC -> APB1ENR &= ~( 1 << 14 ) )			//Disabling clock to SPI2 peripheral
#define SPI3_PCLK_DI()							( RCC -> APB1ENR &= ~( 1 << 15 ) )			//Disabling clock to SPI3 peripheral

/*
 * Clock disable macros for USARTx peripherals
 */

#define USART1_PCLK_DI()						( RCC -> APB2ENR &= ~( 1 << 4 ) )			//Disabling clock to USART1 peripheral

#define USART2_PCLK_DI()						( RCC -> APB1ENR &= ~( 1 << 17 ) )			//Disabling clock to USART2 peripheral
#define USART3_PCLK_DI()						( RCC -> APB1ENR &= ~( 1 << 18 ) )			//Disabling clock to USART3 peripheral
#define UART4_PCLK_DI()							( RCC -> APB1ENR &= ~( 1 << 19 ) )			//Disabling clock to UART4 peripheral
#define UART5_PCLK_DI()							( RCC -> APB1ENR &= ~( 1 << 20 ) )			//Disabling clock to UART5 peripheral

#define USART6_PCLK_DI()						( RCC -> APB2ENR &= ~( 1 << 5 ) )			//Disabling clock to USART6 peripheral

/*
 * Clock disable macros for ADCx peripherals
 */

#define ADC1_PCLK_DI()							( RCC -> APB2ENR &= ~( 1 << 8 ) )			//Disabling clock to ADC1 peripheral
#define ADC2_PCLK_DI()							( RCC -> APB2ENR &= ~( 1 << 9 ) )			//Disabling clock to ADC2 peripheral
#define ADC3_PCLK_DI()							( RCC -> APB2ENR &= ~( 1 << 10 ) )			//Disabling clock to ADC3 peripheral

/*
 * Clock disable macro for SYSCFG peripheral
 */

#define SYSCFG_PCLK_DI()						( RCC -> APB2ENR &= ~( 1 << 14 ) )			//Disabling clock to SYSCFG peripheral


/*
 * Clock Disable macro for TIMx peripherals
 */

#define TIM2_PCLK_DI()							( RCC -> APB1ENR &= ~( 1 << 0 ) )			//Disable clock to TIM2 peripheral
#define TIM5_PCLK_DI()							( RCC -> APB1ENR &= ~( 1 << 3 ) )			//Disable clock to TIM2 peripheral


/*
 * Register reset macros for PGIOx peripherals
 */

#define GPIOA_REG_RESET()						do{ ( RCC -> AHB1RSTR |= ( 1 << 0) );		( RCC -> AHB1RSTR &= ~( 1 << 0) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for GPIOA peripheral.
#define GPIOB_REG_RESET()						do{ ( RCC -> AHB1RSTR |= ( 1 << 1) );		( RCC -> AHB1RSTR &= ~( 1 << 1) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for GPIOB peripheral.
#define GPIOC_REG_RESET()						do{ ( RCC -> AHB1RSTR |= ( 1 << 2) );		( RCC -> AHB1RSTR &= ~( 1 << 2) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for GPIOC peripheral.
#define GPIOD_REG_RESET()						do{ ( RCC -> AHB1RSTR |= ( 1 << 3) );		( RCC -> AHB1RSTR &= ~( 1 << 3) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for GPIOD peripheral.
#define GPIOE_REG_RESET()						do{ ( RCC -> AHB1RSTR |= ( 1 << 4) );		( RCC -> AHB1RSTR &= ~( 1 << 4) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for GPIOE peripheral.
#define GPIOF_REG_RESET()						do{ ( RCC -> AHB1RSTR |= ( 1 << 5) );		( RCC -> AHB1RSTR &= ~( 1 << 5) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for GPIOF peripheral.
#define GPIOG_REG_RESET()						do{ ( RCC -> AHB1RSTR |= ( 1 << 6) );		( RCC -> AHB1RSTR &= ~( 1 << 6) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for GPIOG peripheral.
#define GPIOH_REG_RESET()						do{ ( RCC -> AHB1RSTR |= ( 1 << 7) );		( RCC -> AHB1RSTR &= ~( 1 << 7) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for GPIOH peripheral.
#define GPIOI_REG_RESET()						do{ ( RCC -> AHB1RSTR |= ( 1 << 8) );		( RCC -> AHB1RSTR &= ~( 1 << 8) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for GPIOI peripheral.

/*
 * Register reset macros for SPI peripherals
 */

#define SPI1_REG_RESET()						do{ ( RCC -> APB2RSTR |= ( 1 << 12) );		( RCC -> APB2RSTR &= ~( 1 << 12) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for SPI1 peripheral.
#define SPI2_REG_RESET()						do{ ( RCC -> APB1RSTR |= ( 1 << 14) );		( RCC -> APB1RSTR &= ~( 1 << 14) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for SPI2 peripheral.
#define SPI3_REG_RESET()						do{ ( RCC -> APB1RSTR |= ( 1 << 15) );		( RCC -> APB1RSTR &= ~( 1 << 15) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for SPI3 peripheral.

/*
 * Register reset macros for I2C peripherals
 */

#define I2C1_REG_RESET()						do{ ( RCC -> APB1RSTR |= ( 1 << 21) );		( RCC -> APB1RSTR &= ~( 1 << 21) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for I2C1 peripheral.
#define I2C2_REG_RESET()						do{ ( RCC -> APB1RSTR |= ( 1 << 22) );		( RCC -> APB1RSTR &= ~( 1 << 22) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for I2C2 peripheral.
#define I2C3_REG_RESET()						do{ ( RCC -> APB1RSTR |= ( 1 << 23) );		( RCC -> APB1RSTR &= ~( 1 << 23) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for I2C3 peripheral.

/*
 * Register reset macros for I2C peripherals
 */

#define USART1_REG_RESET()						do{ ( RCC -> APB2RSTR |= ( 1 << 4) );		( RCC -> APB2RSTR &= ~( 1 << 4) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for USART1 peripheral.
#define USART2_REG_RESET()						do{ ( RCC -> APB1RSTR |= ( 1 << 17) );		( RCC -> APB1RSTR &= ~( 1 << 17) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for USART2 peripheral.
#define USART3_REG_RESET()						do{ ( RCC -> APB1RSTR |= ( 1 << 18) );		( RCC -> APB1RSTR &= ~( 1 << 18) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for USART3 peripheral.
#define UART4_REG_RESET()						do{ ( RCC -> APB1RSTR |= ( 1 << 19) );		( RCC -> APB1RSTR &= ~( 1 << 19) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for UART4 peripheral.
#define UART5_REG_RESET()						do{ ( RCC -> APB1RSTR |= ( 1 << 20) );		( RCC -> APB1RSTR &= ~( 1 << 20) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for UART5 peripheral.
#define USART6_REG_RESET()						do{ ( RCC -> APB2RSTR |= ( 1 << 5) );		( RCC -> APB2RSTR &= ~( 1 << 5) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for USART6 peripheral.

/*
 * Register reset macros for ADC peripherals
 */

#define ADC_REG_RESET()							do{ ( RCC -> APB2RSTR |= ( 1 << 8 ) );		( RCC -> APB2RSTR &= ~( 1 << 8 ) ); }while(0)		//Setting and clearing the reset bit of the RCC peripheral reset register for ADC peripheral interface. (COMMON TO ALL ADCs)

/*
 * Register reset macros for TIM2 peripheral
 */

#define TIM2_REG_RESET()						( do{ ( RCC -> APB1RSTR |= ( 1 << 0 ) );		( RCC -> APB1RSTR &= ~( 1 << 0 ) ); }while(0) )	//Setting and clearing the reset bit of the RCC peripheral reset register for TIM2 peripheral interface.
#define TIM5_REG_RESET()						( do{ ( RCC -> APB1RSTR |= ( 1 << 3 ) );		( RCC -> APB1RSTR &= ~( 1 << 3 ) ); }while(0) )	//Setting and clearing the reset bit of the RCC peripheral reset register for TIM5 peripheral interface.


/*
 * Macros used to generate configuration values for SYSCFG_EXTICRx registers (see RM 9.2.3 - 9.2.6)
 */

#define GPIO_BASEADDR_TO_CODE(x)				( (x == GPIOA) ? 0 :\
												(x == GPIOB) ? 1 :\
												(x == GPIOC) ? 2 :\
												(x == GPIOD) ? 3 :\
												(x == GPIOE) ? 4 :\
												(x == GPIOF) ? 5 :\
												(x == GPIOG) ? 6 :\
												(x == GPIOH) ? 7 :\
												(x == GPIOI) ? 8 : 0 )						// backslashes are used here to splice the proceeding line of code to the original line - it makes it more readable instead of one giant line of code for this macro

/*
 * IRQ number definition macros
 * Corresponds to IRQ (interrupt request) number of STM32F407 MCU NVIC (Nested Vectored Interrupt Controller) vector table
 * TODO: Complete this list for the rest of the peripherals
 */

#define IRQ_NO_WWDG								0											//Window watch-dog interrupt
#define IRQ_NO_PVD								1											//PVD through EXTI line detection interrupt
#define IRQ_NO_TAMP_STAMP						2											//Tamper and TimeStamp interrupts through the EXTI line
#define IRQ_NO_RTC_WKUP							3											//RTC Wake-up interrupt through the EXTI line
#define IRQ_NO_FLASH							4											//Flash global interrupt
#define IRQ_NO_RCC								5											//RCC global interrupt
#define IRQ_NO_EXTI0							6											//EXTI Line0 interrupt
#define IRQ_NO_EXTI1							7											//EXTI Line1 interrupt
#define IRQ_NO_EXTI2							8											//EXTI Line2 interrupt
#define IRQ_NO_EXTI3							9											//EXTI Line3 interrupt
#define IRQ_NO_EXTI4							10											//EXTI Line4 interrupt
#define IRQ_NO_ADC								18											//ADC1, ADC2 and ADC3 global interrupts
#define IRQ_NO_EXTI9_5							23											//EXTI Line[9:5] interrupts
#define IRQ_NO_TIM2								28											//TIM2 global interrupt
#define IRQ_NO_I2C1_EV							31											//I2C1 event interrupt
#define IRQ_NO_I2C1_ER							32											//I2C1 error interrupt
#define IRQ_NO_I2C2_EV							33											//I2C2 event interrupt
#define IRQ_NO_I2C2_ER							34											//I2C2 error interrupt
#define IRQ_NO_SPI1								35											//SPI1 global interrupt
#define IRQ_NO_SPI2								36											//SPI2 global interrupt
#define IRQ_NO_USART1							37											//USART1 global interrupt
#define IRQ_NO_USART2							38											//USART2 global interrupt
#define IRQ_NO_USART3							39											//USART3 global interrupt
#define IRQ_NO_EXTI15_10						40											//EXTI Line[15:10] interrupts
#define IRQ_NO_TIM5								50											//TIM5 global interrupt
#define IRQ_NO_SPI3								51											//SPI3 global interrupt
#define IRQ_NO_UART4							52											//UART4 global interrupt
#define IRQ_NO_UART5							53											//UART5 global interrupt
#define IRQ_NO_USART6							71											//USART6 global interrupt
#define IRQ_NO_I2C3_EV							72											//I2C3 event interrupt
#define IRQ_NO_I2C3_ER							73											//I2C3 error interrupt

/*
 * IRQ priority macros
 * Total of 16 possible priority levels for interrupts in the STM32 ARM Cortex M4 IPR configuration registers (they only implement 4 bits)
 */

#define NVIC_IRQ_PRIO_0							0
#define NVIC_IRQ_PRIO_1							1
#define NVIC_IRQ_PRIO_2							2
#define NVIC_IRQ_PRIO_3							3
#define NVIC_IRQ_PRIO_4							4
#define NVIC_IRQ_PRIO_5							5
#define NVIC_IRQ_PRIO_6							6
#define NVIC_IRQ_PRIO_7							7
#define NVIC_IRQ_PRIO_8							8
#define NVIC_IRQ_PRIO_9							9
#define NVIC_IRQ_PRIO_10						10
#define NVIC_IRQ_PRIO_11						11
#define NVIC_IRQ_PRIO_12						12
#define NVIC_IRQ_PRIO_13						13
#define NVIC_IRQ_PRIO_14						14
#define NVIC_IRQ_PRIO_15						15

//Some generic macros

#define ENABLE 									1
#define DISABLE 								0
#define SET 									ENABLE
#define RESET 									DISABLE
#define START 									ENABLE
#define STOP 									DISABLE

#define GPIO_PIN_SET 							SET
#define GPIO_PIN_RESET 							RESET

#define FLAG_SET 								SET
#define FLAG_RESET 								RESET

#define MAX_UINT32_VAL							4294967295


/*		-----------------------------------		Bit Position Definitions of the SPI Peripheral Registers		-----------------------------------		*/

//Register: SPI_CR1
#define SPI_CR1_CPHA							0
#define SPI_CR1_CPOL							1
#define SPI_CR1_MSTR							2
#define SPI_CR1_BR								3
#define SPI_CR1_SPE								6
#define SPI_CR1_LSB_FIRST						7
#define SPI_CR1_SSI								8
#define SPI_CR1_SSM								9
#define SPI_CR1_RX_ONLY							10
#define SPI_CR1_DFF								11
#define SPI_CR1_CRC_NEXT						12
#define SPI_CR1_CRC_EN							13
#define SPI_CR1_BIDI_OE							14
#define SPI_CR1_BIDI_MODE						15

//Register: SPI_CR2
#define SPI_CR2_RXDMAEN							0
#define SPI_CR2_TXDMAEN							1
#define SPI_CR2_SSOE							2
#define SPI_CR2_FRF								4
#define SPI_CR2_ERRIE							5
#define SPI_CR2_RXNEIE							6
#define SPI_CR2_TXEIE							7

//Register: SPI_SR
#define SPI_SR_RXNE								0
#define SPI_SR_TXE								1
#define SPI_SR_CHSIDE							2
#define SPI_SR_UDR								3
#define SPI_SR_CRC_ERR							4
#define SPI_SR_MODF								5
#define SPI_SR_OVR								6
#define SPI_SR_BSY								7
#define SPI_SR_FRE								8

/*		-----------------------------------		Bit Position Definitions of the I2C Peripheral Registers		-----------------------------------		*/

//Register: I2C_CR1
#define I2C_CR1_PE								0
#define I2C_CR1_SMBUS							1
#define I2C_CR1_SMBTYPE							3
#define I2C_CR1_ENARP							4
#define I2C_CR1_ENPEC							5
#define I2C_CR1_ENGC							6
#define I2C_CR1_NOSTRETCH						7
#define I2C_CR1_START							8
#define I2C_CR1_STOP							9
#define I2C_CR1_ACK								10
#define I2C_CR1_POS								11
#define I2C_CR1_PEC								12
#define I2C_CR1_ALERT							13
#define I2C_CR1_SWRST							15

//Register: I2C_CR2
#define I2C_CR2_FREQ							0
#define I2C_CR2_ITERREN							8
#define I2C_CR2_ITEVTEN							9
#define I2C_CR2_ITBUFEN							10
#define I2C_CR2_DMAEN							11
#define I2C_CR2_LAST							12

//Register: I2C_OAR1
#define I2C_OAR1_ADD0							0
#define I2C_OAR1_ADD7_1							1
#define I2C_OAR1_ADD9_8							8
#define I2C_OAR1_ADDMODE						15

//Register: I2C_OAR2
#define I2C_OAR2_ENDUAL							0
#define I2C_OAR2_ADD2							1

//Register: I2C_SR1
#define I2C_SR1_SB								0
#define I2C_SR1_ADDR							1
#define I2C_SR1_BTF								2
#define I2C_SR1_ADD10							3
#define I2C_SR1_STOPF							4
#define I2C_SR1_RXNE							6
#define I2C_SR1_TXE								7
#define I2C_SR1_BERR							8
#define I2C_SR1_ARLO							9
#define I2C_SR1_AF								10
#define I2C_SR1_OVR								11
#define I2C_SR1_PECERR							12
#define I2C_SR1_TIMEOUT							14
#define I2C_SR1_SMBALERT						15

//Register: I2C_SR2
#define I2C_SR2_MSL								0
#define I2C_SR2_BUSY							1
#define I2C_SR2_TRA								2
#define I2C_SR2_GENCALL							4
#define I2C_SR2_SMBDEFAULT						5
#define I2C_SR2_SMBHOST							6
#define I2C_SR2_DUALF							7
#define I2C_SR2_PEC7_0							8

//Register: I2C_CCR
#define I2C_CCR_CCR								0
#define I2C_CCR_DUTY							14
#define I2C_CCR_FAST_SLOW						15

/*		-----------------------------------		Bit Position Definitions of the USART Peripheral Registers		-----------------------------------		*/

//Register: USART_SR
#define USART_SR_PE								0
#define USART_SR_FE								1
#define USART_SR_NF								2
#define USART_SR_ORE							3
#define USART_SR_IDLE							4
#define USART_SR_RXNE							5
#define USART_SR_TC								6
#define USART_SR_TXE							7
#define USART_SR_LBD							8
#define USART_SR_CTS							9

//Register: USART_CR1
#define USART_CR1_SBK							0
#define USART_CR1_RWU							1
#define USART_CR1_RE							2
#define USART_CR1_TE							3
#define USART_CR1_IDLEIE						4
#define USART_CR1_RXNEIE						5
#define USART_CR1_TCIE							6
#define USART_CR1_TXEIE							7
#define USART_CR1_PEIE							8
#define USART_CR1_PS							9
#define USART_CR1_PCE							10
#define USART_CR1_WAKE							11
#define USART_CR1_M								12
#define USART_CR1_UE							13
#define USART_CR1_OVER8							15

//Register: USART_CR2
#define USART_CR2_ADD							0
#define USART_CR2_LBDL							5
#define USART_CR2_LBDIE							6
#define USART_CR2_LBCL							8
#define USART_CR2_CPHA							9
#define USART_CR2_CPOL							10
#define USART_CR2_CLKEN							11
#define USART_CR2_STOP							12
#define USART_CR2_LINEN							14

//Register: USART_CR3
#define USART_CR3_EIE							0
#define USART_CR3_IREN							1
#define USART_CR3_IRLP							2
#define USART_CR3_HDSEL							3
#define USART_CR3_NACK							4
#define USART_CR3_SCEN							5
#define USART_CR3_DMAR							6
#define USART_CR3_DMAT							7
#define USART_CR3_RTSE							8
#define USART_CR3_CTSE							9
#define USART_CR3_CTSIE							10
#define USART_CR3_ONEBIT						11

/*		-----------------------------------		Bit Position Definitions of the ADC Peripheral Registers		-----------------------------------		*/

//Register: ADC_SR
#define ADC_SR_AWD								0
#define ADC_SR_EOC								1
#define ADC_SR_JEOC								2
#define ADC_SR_JSTRT							3
#define ADC_SR_STRT								4
#define ADC_SR_OVR								5


//Register: ADC_CR1
#define ADC_CR1_AWDCH_4_0						0
#define ADC_CR1_EOCIE							5
#define ADC_CR1_AWDIE							6
#define ADC_CR1_JEOCIE							7
#define ADC_CR1_SCAN							8
#define ADC_CR1_AWDSGL							9
#define ADC_CR1_JAUTO							10
#define ADC_CR1_DISCEN							11
#define ADC_CR1_JDISCEN							12
#define ADC_CR1_DISCNUM_2_0						13
#define ADC_CR1_JAWDEN							22
#define ADC_CR1_AWDEN							23
#define ADC_CR1_RES_1_0							24
#define ADC_CR1_OVRIE							26


//Register: ADC_CR2
#define ADC_CR2_ADON							0
#define ADC_CR2_CONT							1
#define ADC_CR2_DMA								8
#define ADC_CR2_DDS								9
#define ADC_CR2_EOCS							10
#define ADC_CR2_ALIGN							11
#define ADC_CR2_JEXTSEL_3_0						16
#define ADC_CR2_JEXTEN_1_0						20
#define ADC_CR2_JSWSTART						22
#define ADC_CR2_EXTSEL_3_0						24
#define ADC_CR2_EXTEN_1_0						28
#define ADC_CR2_SWSTART							30

//Register: ADC_SQR1
#define ADC_SQR1_SQ13_4_0						0
#define ADC_SQR1_SQ14_4_0						5
#define ADC_SQR1_SQ15_4_0						10
#define ADC_SQR1_SQ16_4_0						15
#define ADC_SQR1_L_3_0							20

//Register: ADC_SQR2
#define ADC_SQR2_SQ7_4_0						0
#define ADC_SQR2_SQ8_4_0						5
#define ADC_SQR2_SQ9_4_0						10
#define ADC_SQR2_SQ10_4_0						15
#define ADC_SQR2_SQ11_4_0						20
#define ADC_SQR2_SQ12_4_0						25

//Register: ADC_SQR3
#define ADC_SQR3_SQ1_4_0						0
#define ADC_SQR3_SQ2_4_0						5
#define ADC_SQR3_SQ3_4_0						10
#define ADC_SQR3_SQ4_4_0						15
#define ADC_SQR3_SQ5_4_0						20
#define ADC_SQR3_SQ6_4_0						25

//Register: ADC_CCR
#define ADC_CCR_MULTI_4_0						0
#define ADC_CCR_DELAY_3_0						8
#define ADC_CCR_DDS								13
#define ADC_CCR_DMA_1_0							14
#define ADC_CCR_ADCPRE_1_0						16
#define ADC_CCR_VBATE							22
#define ADC_CCR_TSVREFE							23


/*		-----------------------------------		Bit Position Definitions of the TIM2 to TIM5 Peripheral Registers		-----------------------------------		*/

//Register: TIM2_5_CR1
#define TIM2_5_CR1_CEN							0
#define TIM2_5_CR1_UDIS							1
#define TIM2_5_CR1_URS							2
#define TIM2_5_CR1_OPM							3
#define TIM2_5_CR1_DIR							4
#define TIM2_5_CR1_CMS_1_0						5
#define TIM2_5_CR1_ARPE							7
#define TIM2_5_CR1_CKD_1_0						8

//Register: TIM2_5_CR2
#define TIM2_5_CR2_CCDS							3
#define TIM2_5_CR2_MMS_2_0						4
#define TIM2_5_CR2_TI1S							7

//Register: TIM2_5_DIER
#define TIM2_5_DIER_UIE							0
#define TIM2_5_DIER_CC1IE						1
#define TIM2_5_DIER_CC2IE						2
#define TIM2_5_DIER_CC3IE						3
#define TIM2_5_DIER_CC4IE						4
#define TIM2_5_DIER_TIE							6
#define TIM2_5_DIER_UDE							8
#define TIM2_5_DIER_CC1DE						9
#define TIM2_5_DIER_CC2DE						10
#define TIM2_5_DIER_CC3DE						11
#define TIM2_5_DIER_CC4DE						12
#define TIM2_5_DIER_TDE							14

//Register: TIM2_5_SR
#define TIM2_5_SR_UIF							0
#define TIM2_5_SR_CC1IF							1
#define TIM2_5_SR_CC2IF							2
#define TIM2_5_SR_CC3IF							3
#define TIM2_5_SR_CC4IF							4
#define TIM2_5_SR_TIF							6
#define TIM2_5_SR_CC1OF							9
#define TIM2_5_SR_CC2OF							10
#define TIM2_5_SR_CC3OF							11
#define TIM2_5_SR_CC4OF							12

//Register: TIM2_5_EGR
#define TIM2_5_EGR_UG							0
#define TIM2_5_EGR_CC1G							1
#define TIM2_5_EGR_CC2G							2
#define TIM2_5_EGR_CC3G							3
#define TIM2_5_EGR_CC4G							4
#define TIM2_5_EGR_TG							6



#include "stm32f407vg_gpio_driver.h"
#include "stm32f407vg_spi_driver.h"
#include "stm32f407vg_i2c_driver.h"
#include "stm32f407vg_usart_driver.h"
#include "stm32f407vg_rcc_driver.h"
#include "stm32f407vg_adc_driver.h"
#include "stm32f407vg_tim_driver.h"

#endif /* INC_STM32F407VG_H_ */




















