################################################################################
# Automatically-generated file. Do not edit!
################################################################################

#Progress monitor hint: 62
first : all
-include ../makefile.init

# This file contains definitions of environment variables used in the makefiles and .args files if exist.
-include makefile.local

RM := "$(GNU_Make_Install_DirEnv)/rm" -f

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
-include devices/subdir.mk
-include build_test/src/subdir.mk
-include build_test/devices/subdir.mk
-include build_app/src/subdir.mk
-include build_app/devices/subdir.mk
-include LCF/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(SL_DEPS)),)
-include $(SL_DEPS)
endif
ifneq ($(strip $(SX_UPPER_DEPS)),)
-include $(SX_UPPER_DEPS)
endif
ifneq ($(strip $(SX_DEPS)),)
-include $(SX_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SL_UPPER_DEPS)),)
-include $(SL_UPPER_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
VSPA_EXECUTABLE_FILE_OUTPUTS += \
nmm_vspa_sp_HARNESS_sim.eld \

VSPA_EXECUTABLE_FILE_OUTPUTS_QUOTED += \
"nmm_vspa_sp_HARNESS_sim.eld" \

VSPA_EXECUTABLE_FILE_OUTPUTS_OS_FORMAT += \
nmm_vspa_sp_HARNESS_sim.eld \


# All Target
all: warning-messages explicit-dependencies nmm_vspa_sp_HARNESS_sim.eld

# Tool invocations
nmm_vspa_sp_HARNESS_sim.eld: $(OBJS) $(LCF_BUILD_DEPENCENCY) $(ELB_BUILD_DEPENCENCY) $(USER_OBJS) $(LCF_SRCS) $(ELB_SRCS) ../LCF/la9310_nmm_sim.lcf
	@echo 'Building target: $@'
	@echo 'Executing target #63 $@'
	@echo 'Invoking: VSPA C Linker'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"nmm_vspa_sp_HARNESS_sim.args" -o "nmm_vspa_sp_HARNESS_sim.eld"
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) "./*/*.d"  $(C_UPPER_DEPS_QUOTED) "./*/*.d"  $(VSPA_EXECUTABLE_FILE_OUTPUTS_QUOTED) "./*/*.d"  "./*/*/*.d"  $(CC_DEPS_QUOTED) $(CPP_DEPS_QUOTED) "./*/*/*.eln" "./*/*.eln"  $(S_UPPER_DEPS_QUOTED) $(SX_UPPER_DEPS_QUOTED) $(CXX_DEPS_QUOTED) $(ASM_DEPS_QUOTED) $(ASM_UPPER_DEPS_QUOTED) $(C++_DEPS_QUOTED) $(SL_UPPER_DEPS_QUOTED) $(ProjDirPath)/build_sim/nmm_vspa_sp_HARNESS_sim.eld
	-@echo ' '

.PHONY: all clean dependents explicit-dependencies warning-messages
.SECONDARY:

-include ../makefile.targets

explicit-dependencies: ../LCF/la9310_nmm_sim.lcf 

warning-messages: 

