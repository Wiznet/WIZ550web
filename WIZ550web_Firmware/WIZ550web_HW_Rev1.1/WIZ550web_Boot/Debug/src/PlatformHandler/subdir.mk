################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/PlatformHandler/eepromHandler.c \
../src/PlatformHandler/flashHandler.c \
../src/PlatformHandler/gpioHandler.c \
../src/PlatformHandler/i2cHandler.c \
../src/PlatformHandler/rccHandler.c \
../src/PlatformHandler/ring_buffer.c \
../src/PlatformHandler/spiHandler.c \
../src/PlatformHandler/storageHandler.c \
../src/PlatformHandler/timerHandler.c \
../src/PlatformHandler/uartHandler.c 

OBJS += \
./src/PlatformHandler/eepromHandler.o \
./src/PlatformHandler/flashHandler.o \
./src/PlatformHandler/gpioHandler.o \
./src/PlatformHandler/i2cHandler.o \
./src/PlatformHandler/rccHandler.o \
./src/PlatformHandler/ring_buffer.o \
./src/PlatformHandler/spiHandler.o \
./src/PlatformHandler/storageHandler.o \
./src/PlatformHandler/timerHandler.o \
./src/PlatformHandler/uartHandler.o 

C_DEPS += \
./src/PlatformHandler/eepromHandler.d \
./src/PlatformHandler/flashHandler.d \
./src/PlatformHandler/gpioHandler.d \
./src/PlatformHandler/i2cHandler.d \
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
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -DDEBUG -DUSE_FULL_ASSERT -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=12000000 -I"../include" -I"D:\Wiznet\Firmware\WIZ550web_workspace_rev1.1_FW2.0.0\WIZ550web-WIZ550web_Rev1.1\WIZ550web-WIZ550web_Rev1.1\WIZ550web_Firmware\WIZ550web_HW_Rev1.1\WIZ550web_Boot\src\Configuration" -I"D:\Wiznet\Firmware\WIZ550web_workspace_rev1.1_FW2.0.0\WIZ550web-WIZ550web_Rev1.1\WIZ550web-WIZ550web_Rev1.1\WIZ550web_Firmware\WIZ550web_HW_Rev1.1\WIZ550web_Boot\src\netutil" -I"D:\Wiznet\Firmware\WIZ550web_workspace_rev1.1_FW2.0.0\WIZ550web-WIZ550web_Rev1.1\WIZ550web-WIZ550web_Rev1.1\WIZ550web_Firmware\WIZ550web_HW_Rev1.1\WIZ550web_Boot\src\Internet\TFTP" -I"D:\Wiznet\Firmware\WIZ550web_workspace_rev1.1_FW2.0.0\WIZ550web-WIZ550web_Rev1.1\WIZ550web-WIZ550web_Rev1.1\WIZ550web_Firmware\WIZ550web_HW_Rev1.1\WIZ550web_Boot\src" -I"D:\Wiznet\Firmware\WIZ550web_workspace_rev1.1_FW2.0.0\WIZ550web-WIZ550web_Rev1.1\WIZ550web-WIZ550web_Rev1.1\WIZ550web_Firmware\WIZ550web_HW_Rev1.1\WIZ550web_Boot\src\Ethernet" -I"D:\Wiznet\Firmware\WIZ550web_workspace_rev1.1_FW2.0.0\WIZ550web-WIZ550web_Rev1.1\WIZ550web-WIZ550web_Rev1.1\WIZ550web_Firmware\WIZ550web_HW_Rev1.1\WIZ550web_Boot\src\PlatformHandler" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


