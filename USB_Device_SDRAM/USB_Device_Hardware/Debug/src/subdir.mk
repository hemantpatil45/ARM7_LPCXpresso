################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../src/cr_startup_lpc24.s 

C_SRCS += \
../src/SDRAM.c \
../src/crp.c \
../src/glcd.c \
../src/main.c \
../src/sdcard.c \
../src/sys_calls.c \
../src/system_init.c \
../src/upper_sdram.c \
../src/usbcore.c \
../src/usbdesc.c \
../src/usbhw.c 

OBJS += \
./src/SDRAM.o \
./src/cr_startup_lpc24.o \
./src/crp.o \
./src/glcd.o \
./src/main.o \
./src/sdcard.o \
./src/sys_calls.o \
./src/system_init.o \
./src/upper_sdram.o \
./src/usbcore.o \
./src/usbdesc.o \
./src/usbhw.o 

C_DEPS += \
./src/SDRAM.d \
./src/crp.d \
./src/glcd.d \
./src/main.d \
./src/sdcard.d \
./src/sys_calls.d \
./src/system_init.d \
./src/upper_sdram.d \
./src/usbcore.d \
./src/usbdesc.d \
./src/usbhw.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -D__REDLIB__ -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=arm7tdmi -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -DDEBUG -D__CODE_RED -D__REDLIB__ -g3 -mcpu=arm7tdmi -specs=redlib.specs -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


