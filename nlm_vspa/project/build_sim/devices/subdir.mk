################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../devices/dma.c" \
"../devices/mailbox.c" \
"../devices/phy-timer.c" \

C_SRCS += \
../devices/dma.c \
../devices/mailbox.c \
../devices/phy-timer.c \

S_SRCS += \
../devices/bss.s \

S_SRCS_QUOTED += \
"../devices/bss.s" \

S_DEPS_QUOTED += \
"./devices/bss.d" \

OBJS += \
./devices/bss.eln \
./devices/dma.eln \
./devices/mailbox.eln \
./devices/phy-timer.eln \

S_DEPS += \
./devices/bss.d \

OBJS_QUOTED += \
"./devices/bss.eln" \
"./devices/dma.eln" \
"./devices/mailbox.eln" \
"./devices/phy-timer.eln" \

C_DEPS += \
./devices/dma.d \
./devices/mailbox.d \
./devices/phy-timer.d \

C_DEPS_QUOTED += \
"./devices/dma.d" \
"./devices/mailbox.d" \
"./devices/phy-timer.d" \

OBJS_OS_FORMAT += \
./devices/bss.eln \
./devices/dma.eln \
./devices/mailbox.eln \
./devices/phy-timer.eln \


# Each subdirectory must supply rules for building sources it contributes
devices/bss.eln: ../devices/bss.s
	@echo 'Building file: $<'
	@echo 'Executing target #29 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"devices/bss.args" -o "devices/bss.eln" -c "$<" -inline all -o "devices" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

devices/%.d: ../devices/%.s
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

devices/dma.eln: ../devices/dma.c
	@echo 'Building file: $<'
	@echo 'Executing target #30 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"devices/dma.args" -o "devices/dma.eln" -c "$<" -inline all -o "devices" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

devices/%.d: ../devices/%.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

devices/mailbox.eln: ../devices/mailbox.c
	@echo 'Building file: $<'
	@echo 'Executing target #31 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"devices/mailbox.args" -o "devices/mailbox.eln" -c "$<" -inline all -o "devices" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

devices/phy-timer.eln: ../devices/phy-timer.c
	@echo 'Building file: $<'
	@echo 'Executing target #32 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"devices/phy-timer.args" -o "devices/phy-timer.eln" -c "$<" -inline all -o "devices" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '


