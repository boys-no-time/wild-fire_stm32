################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/gpio.c \
../Src/iwdg.c \
../Src/main.c \
../Src/spi.c \
../Src/stm32l4xx_hal_msp.c \
../Src/stm32l4xx_it.c \
../Src/syscalls.c \
../Src/system_stm32l4xx.c \
../Src/usart.c 

OBJS += \
./Src/gpio.o \
./Src/iwdg.o \
./Src/main.o \
./Src/spi.o \
./Src/stm32l4xx_hal_msp.o \
./Src/stm32l4xx_it.o \
./Src/syscalls.o \
./Src/system_stm32l4xx.o \
./Src/usart.o 

C_DEPS += \
./Src/gpio.d \
./Src/iwdg.d \
./Src/main.d \
./Src/spi.d \
./Src/stm32l4xx_hal_msp.d \
./Src/stm32l4xx_it.d \
./Src/syscalls.d \
./Src/system_stm32l4xx.d \
./Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc -c "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/examples" -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/integration" -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/rss/include" -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/rss/lib" -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/Src" -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/rss" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

