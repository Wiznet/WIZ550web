################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/PlatformHandler/adcHandler.c \
../src/PlatformHandler/extiHandler.c \
../src/PlatformHandler/flashHandler.c \
../src/PlatformHandler/gpioHandler.c \
../src/PlatformHandler/mmcHandler.c \
../src/PlatformHandler/rccHandler.c \
../src/PlatformHandler/ring_buffer.c \
../src/PlatformHandler/spiHandler.c \
../src/PlatformHandler/storageHandler.c \
../src/PlatformHandler/timerHandler.c \
../src/PlatformHandler/uartHandler.c \
../src/PlatformHandler/userHandler.c 

OBJS += \
./src/PlatformHandler/adcHandler.o \
./src/PlatformHandler/extiHandler.o \
./src/PlatformHandler/flashHandler.o \
./src/PlatformHandler/gpioHandler.o \
./src/PlatformHandler/mmcHandler.o \
./src/PlatformHandler/rccHandler.o \
./src/PlatformHandler/ring_buffer.o \
./src/PlatformHandler/spiHandler.o \
./src/PlatformHandler/storageHandler.o \
./src/PlatformHandler/timerHandler.o \
./src/PlatformHandler/uartHandler.o \
./src/PlatformHandler/userHandler.o 

C_DEPS += \
./src/PlatformHandler/adcHandler.d \
./src/PlatformHandler/extiHandler.d \
./src/PlatformHandler/flashHandler.d \
./src/PlatformHandler/gpioHandler.d \
./src/PlatformHandler/mmcHandler.d \
./src/PlatformHandler/rccHandler.d \
./src/PlatformHandler/ring_buffer.d \
./src/PlatformHandler/spiHandler.d \
./src/PlatformHandler/storageHandler.d \
./src/PlatformHandler/timerHandler.d \
./src/PlatformHandler/uartHandler.d \
./src/PlatformHandler/userHandler.d 


# Each subdirectory must supply rules for building sources it contributes
src/PlatformHandler/%.o: ../src/PlatformHandler/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_ITM -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=12000000 -DHW_MAJOR_VER=1 -DHW_MINOR_VER=0 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I../src -I../src/ATcmd -I../src/ATcmd/library -I../src/callback -I../src/Configuration -I../src/Ethernet -I../src/Ethernet/W5500 -I../src/Internet/DHCP -I../src/Internet/FTPServer -I../src/Internet/httpServer -I../src/netutil -I../src/PlatformHandler -I../src/PlatformHandler/FatFs -I../src/util -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


