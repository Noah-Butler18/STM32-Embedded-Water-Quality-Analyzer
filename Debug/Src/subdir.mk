################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/FinalProjectSTMToArduino.c 

OBJS += \
./Src/FinalProjectSTMToArduino.o 

C_DEPS += \
./Src/FinalProjectSTMToArduino.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I"C:/Users/butle/OneDrive/Documents/MCU1-Course/MCU1/stm32f407_drivers/drivers/Inc" -I../Inc -I"C:/Users/butle/OneDrive/Documents/MCU1-Course/MCU1/stm32f407_drivers/bsp/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/FinalProjectSTMToArduino.cyclo ./Src/FinalProjectSTMToArduino.d ./Src/FinalProjectSTMToArduino.o ./Src/FinalProjectSTMToArduino.su

.PHONY: clean-Src

