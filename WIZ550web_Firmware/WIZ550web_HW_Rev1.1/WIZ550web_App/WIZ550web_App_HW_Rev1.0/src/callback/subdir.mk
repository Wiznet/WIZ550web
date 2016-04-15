################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/callback/dhcp_cb.c 

OBJS += \
./src/callback/dhcp_cb.o 

C_DEPS += \
./src/callback/dhcp_cb.d 


# Each subdirectory must supply rules for building sources it contributes
src/callback/%.o: ../src/callback/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_ITM -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=12000000 -DHW_MAJOR_VER=1 -DHW_MINOR_VER=0 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I../src -I../src/ATcmd -I../src/ATcmd/library -I../src/callback -I../src/Configuration -I../src/Ethernet -I../src/Ethernet/W5500 -I../src/Internet/DHCP -I../src/Internet/FTPServer -I../src/Internet/httpServer -I../src/netutil -I../src/PlatformHandler -I../src/PlatformHandler/FatFs -I../src/util -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


