################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/PlatformHandler/flashHandler.c \
../src/PlatformHandler/gpioHandler.c \
../src/PlatformHandler/rccHandler.c \
../src/PlatformHandler/ring_buffer.c \
../src/PlatformHandler/spiHandler.c \
../src/PlatformHandler/storageHandler.c \
../src/PlatformHandler/timerHandler.c \
../src/PlatformHandler/uartHandler.c 

OBJS += \
./src/PlatformHandler/flashHandler.o \
./src/PlatformHandler/gpioHandler.o \
./src/PlatformHandler/rccHandler.o \
./src/PlatformHandler/ring_buffer.o \
./src/PlatformHandler/spiHandler.o \
./src/PlatformHandler/storageHandler.o \
./src/PlatformHandler/timerHandler.o \
./src/PlatformHandler/uartHandler.o 

C_DEPS += \
./src/PlatformHandler/flashHandler.d \
./src/PlatformHandler/gpioHandler.d \
./src/PlatformHandler/rccHandler.d \
./src/PlatformHandler/ring_buffer.d \
./src/PlatformHandler/spiHandler.d \
./src/PlatformHandler/storageHandler.d \
./src/PlatformHandler/timerHandler.d \
./src/PlatformHandler/uartHandler.d 


# Each subdirectory must supply rules for building sources it contributes
src/PlatformHandler/%.o: ../src/PlatformHandler/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -DDEBUG -DUSE_FULL_ASSERT -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=12000000 -I"../include" -I"D:\Wiznet\Firmware\WIZ550web_workspace_20151209\WIZ550web_Boot\src\Configuration" -I"D:\Wiznet\Firmware\WIZ550web_workspace_20151209\WIZ550web_Boot\src\netutil" -I"D:\Wiznet\Firmware\WIZ550web_workspace_20151209\WIZ550web_Boot\src\Internet\TFTP" -I"D:\Wiznet\Firmware\WIZ550web_workspace_20151209\WIZ550web_Boot\src" -I"D:\Wiznet\Firmware\WIZ550web_workspace_20151209\WIZ550web_Boot\src\Ethernet" -I"D:\Wiznet\Firmware\WIZ550web_workspace_20151209\WIZ550web_Boot\src\PlatformHandler" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


