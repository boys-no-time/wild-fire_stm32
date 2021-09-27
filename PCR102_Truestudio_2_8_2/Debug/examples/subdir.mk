################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../examples/example_assembly_test.c \
../examples/example_detector_distance.c \
../examples/example_detector_distance_recorded.c \
../examples/example_detector_presence.c \
../examples/example_get_next_by_reference.c \
../examples/example_multiple_service_usage.c \
../examples/example_service_envelope.c \
../examples/example_service_iq.c \
../examples/example_service_power_bins.c \
../examples/example_service_sparse.c \
../examples/ref_app_parking.c \
../examples/ref_app_rf_certification_test.c \
../examples/ref_app_smart_presence.c \
../examples/ref_app_tank_level.c 

OBJS += \
./examples/example_assembly_test.o \
./examples/example_detector_distance.o \
./examples/example_detector_distance_recorded.o \
./examples/example_detector_presence.o \
./examples/example_get_next_by_reference.o \
./examples/example_multiple_service_usage.o \
./examples/example_service_envelope.o \
./examples/example_service_iq.o \
./examples/example_service_power_bins.o \
./examples/example_service_sparse.o \
./examples/ref_app_parking.o \
./examples/ref_app_rf_certification_test.o \
./examples/ref_app_smart_presence.o \
./examples/ref_app_tank_level.o 

C_DEPS += \
./examples/example_assembly_test.d \
./examples/example_detector_distance.d \
./examples/example_detector_distance_recorded.d \
./examples/example_detector_presence.d \
./examples/example_get_next_by_reference.d \
./examples/example_multiple_service_usage.d \
./examples/example_service_envelope.d \
./examples/example_service_iq.d \
./examples/example_service_power_bins.d \
./examples/example_service_sparse.d \
./examples/ref_app_parking.d \
./examples/ref_app_rf_certification_test.d \
./examples/ref_app_smart_presence.d \
./examples/ref_app_tank_level.d 


# Each subdirectory must supply rules for building sources it contributes
examples/%.o: ../examples/%.c examples/subdir.mk
	arm-none-eabi-gcc -c "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/examples" -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/integration" -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/rss/include" -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/rss/lib" -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/Src" -I"E:/workspace/PCB_project/Smart_outlets/PCR102_Truestudio_2_8_2/rss" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

