################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../integration/acc_hal_integration_stm32cube_xc111_r4a.c \
../integration/acc_integration_stm32.c 

OBJS += \
./integration/acc_hal_integration_stm32cube_xc111_r4a.o \
./integration/acc_integration_stm32.o 

C_DEPS += \
./integration/acc_hal_integration_stm32cube_xc111_r4a.d \
./integration/acc_integration_stm32.d 


# Each subdirectory must supply rules for building sources it contributes
integration/%.o: ../integration/%.c integration/subdir.mk
	arm-none-eabi-gcc -c "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/examples" -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/integration" -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/rss/include" -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/rss/lib" -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/Src" -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/rss" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

