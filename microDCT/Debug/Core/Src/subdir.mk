################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/executable.c \
../Core/Src/main.c \
../Core/Src/mcp23017.c \
../Core/Src/misc.c \
../Core/Src/pp.c \
../Core/Src/psu.c \
../Core/Src/rng.c \
../Core/Src/soft_i2c.c \
../Core/Src/stm32g4xx_hal_msp.c \
../Core/Src/stm32g4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32g4xx.c \
../Core/Src/uart.c \
../Core/Src/vm.c 

OBJS += \
./Core/Src/executable.o \
./Core/Src/main.o \
./Core/Src/mcp23017.o \
./Core/Src/misc.o \
./Core/Src/pp.o \
./Core/Src/psu.o \
./Core/Src/rng.o \
./Core/Src/soft_i2c.o \
./Core/Src/stm32g4xx_hal_msp.o \
./Core/Src/stm32g4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32g4xx.o \
./Core/Src/uart.o \
./Core/Src/vm.o 

C_DEPS += \
./Core/Src/executable.d \
./Core/Src/main.d \
./Core/Src/mcp23017.d \
./Core/Src/misc.d \
./Core/Src/pp.d \
./Core/Src/psu.d \
./Core/Src/rng.d \
./Core/Src/soft_i2c.d \
./Core/Src/stm32g4xx_hal_msp.d \
./Core/Src/stm32g4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32g4xx.d \
./Core/Src/uart.d \
./Core/Src/vm.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/executable.cyclo ./Core/Src/executable.d ./Core/Src/executable.o ./Core/Src/executable.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mcp23017.cyclo ./Core/Src/mcp23017.d ./Core/Src/mcp23017.o ./Core/Src/mcp23017.su ./Core/Src/misc.cyclo ./Core/Src/misc.d ./Core/Src/misc.o ./Core/Src/misc.su ./Core/Src/pp.cyclo ./Core/Src/pp.d ./Core/Src/pp.o ./Core/Src/pp.su ./Core/Src/psu.cyclo ./Core/Src/psu.d ./Core/Src/psu.o ./Core/Src/psu.su ./Core/Src/rng.cyclo ./Core/Src/rng.d ./Core/Src/rng.o ./Core/Src/rng.su ./Core/Src/soft_i2c.cyclo ./Core/Src/soft_i2c.d ./Core/Src/soft_i2c.o ./Core/Src/soft_i2c.su ./Core/Src/stm32g4xx_hal_msp.cyclo ./Core/Src/stm32g4xx_hal_msp.d ./Core/Src/stm32g4xx_hal_msp.o ./Core/Src/stm32g4xx_hal_msp.su ./Core/Src/stm32g4xx_it.cyclo ./Core/Src/stm32g4xx_it.d ./Core/Src/stm32g4xx_it.o ./Core/Src/stm32g4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32g4xx.cyclo ./Core/Src/system_stm32g4xx.d ./Core/Src/system_stm32g4xx.o ./Core/Src/system_stm32g4xx.su ./Core/Src/uart.cyclo ./Core/Src/uart.d ./Core/Src/uart.o ./Core/Src/uart.su ./Core/Src/vm.cyclo ./Core/Src/vm.d ./Core/Src/vm.o ./Core/Src/vm.su

.PHONY: clean-Core-2f-Src

