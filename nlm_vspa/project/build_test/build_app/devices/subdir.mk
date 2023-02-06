################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
SL_SRCS_QUOTED += \
"../build_app/devices/dma.sl" \
"../build_app/devices/mailbox.sl" \
"../build_app/devices/phy-timer.sl" \

S1_SRCS_QUOTED += \
"../build_app/devices/dma.s1" \
"../build_app/devices/mailbox.s1" \
"../build_app/devices/phy-timer.s1" \

S1_SRCS += \
../build_app/devices/dma.s1 \
../build_app/devices/mailbox.s1 \
../build_app/devices/phy-timer.s1 \

SL_SRCS += \
../build_app/devices/dma.sl \
../build_app/devices/mailbox.sl \
../build_app/devices/phy-timer.sl \

ELN_SRCS_QUOTED += \
"../build_app/devices/bss.eln" \
"../build_app/devices/dma.eln" \
"../build_app/devices/mailbox.eln" \
"../build_app/devices/phy-timer.eln" \

ELN_SRCS += \
../build_app/devices/bss.eln \
../build_app/devices/dma.eln \
../build_app/devices/mailbox.eln \
../build_app/devices/phy-timer.eln \

OBJS += \
./build_app/devices/dma.eln \
./build_app/devices/mailbox.eln \
./build_app/devices/phy-timer.eln \

SL_DEPS_QUOTED += \
"./build_app/devices/dma.d" \
"./build_app/devices/mailbox.d" \
"./build_app/devices/phy-timer.d" \

OBJS_QUOTED += \
"./build_app/devices/dma.eln" \
"./build_app/devices/mailbox.eln" \
"./build_app/devices/phy-timer.eln" \

SL_DEPS += \
./build_app/devices/dma.d \
./build_app/devices/mailbox.d \
./build_app/devices/phy-timer.d \

OBJS_OS_FORMAT += \
./build_app/devices/dma.eln \
./build_app/devices/mailbox.eln \
./build_app/devices/phy-timer.eln \


# Each subdirectory must supply rules for building sources it contributes
build_app/devices/dma.eln: ../build_app/devices/dma.sl
	@echo 'Building file: $<'
	@echo 'Executing target #60 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/devices/dma.args" -o "build_app/devices/dma.eln" -c "$<" -inline all -o "build_app/devices" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/devices/dma.d: ../build_app/devices/dma.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/devices/mailbox.eln: ../build_app/devices/mailbox.sl
	@echo 'Building file: $<'
	@echo 'Executing target #61 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/devices/mailbox.args" -o "build_app/devices/mailbox.eln" -c "$<" -inline all -o "build_app/devices" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/devices/mailbox.d: ../build_app/devices/mailbox.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/devices/phy-timer.eln: ../build_app/devices/phy-timer.sl
	@echo 'Building file: $<'
	@echo 'Executing target #62 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/devices/phy-timer.args" -o "build_app/devices/phy-timer.eln" -c "$<" -inline all -o "build_app/devices" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/devices/phy-timer.d: ../build_app/devices/phy-timer.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '


