################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: PL_ARM_SDRAM_READ_WRITE.axf

# Tool invocations
PL_ARM_SDRAM_READ_WRITE.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="PL_ARM_SDRAM_READ_WRITE.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=arm7tdmi -T "PL_ARM_SDRAM_READ_WRITE_Debug.ld" -o "PL_ARM_SDRAM_READ_WRITE.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) PL_ARM_SDRAM_READ_WRITE.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-objcopy -v -O ihex "PL_ARM_SDRAM_READ_WRITE.axf" "PL_ARM_SDRAM_READ_WRITE.hex" ; # checksum -p LPC2478 -d "PL_ARM_SDRAM_READ_WRITE.bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
