################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bsp/Src/ds18b20_temp_sensor.c 

OBJS += \
./bsp/Src/ds18b20_temp_sensor.o 

C_DEPS += \
./bsp/Src/ds18b20_temp_sensor.d 


# Each subdirectory must supply rules for building sources it contributes
bsp/Src/%.o bsp/Src/%.su bsp/Src/%.cyclo: ../bsp/Src/%.c bsp/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I"C:/Users/butle/OneDrive/Documents/MCU1-Course/MCU1/stm32f407_drivers/drivers/Inc" -I../Inc -I"C:/Users/butle/OneDrive/Documents/MCU1-Course/MCU1/stm32f407_drivers/bsp/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=soft -mthumb -o "$@"

clean: clean-bsp-2f-Src

clean-bsp-2f-Src:
	-$(RM) ./bsp/Src/ds18b20_temp_sensor.cyclo ./bsp/Src/ds18b20_temp_sensor.d ./bsp/Src/ds18b20_temp_sensor.o ./bsp/Src/ds18b20_temp_sensor.su

.PHONY: clean-bsp-2f-Src

