################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/netutil/netutil.c 

OBJS += \
./src/netutil/netutil.o 

C_DEPS += \
./src/netutil/netutil.d 


# Each subdirectory must supply rules for building sources it contributes
src/netutil/%.o: ../src/netutil/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -DDEBUG -DUSE_FULL_ASSERT -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=12000000 -I"../include" -I"D:\WIZnet\__Purple\WIZ550WEB\workspace\WIZ550web_Boot\src\Configuration" -I"D:\WIZnet\__Purple\WIZ550WEB\workspace\WIZ550web_Boot\src\netutil" -I"D:\WIZnet\__Purple\WIZ550WEB\workspace\WIZ550web_Boot\src\Internet\TFTP" -I"D:\WIZnet\__Purple\WIZ550WEB\workspace\WIZ550web_Boot\src" -I"D:\WIZnet\__Purple\WIZ550WEB\workspace\WIZ550web_Boot\src\Ethernet" -I"D:\WIZnet\__Purple\WIZ550WEB\workspace\WIZ550web_Boot\src\PlatformHandler" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


