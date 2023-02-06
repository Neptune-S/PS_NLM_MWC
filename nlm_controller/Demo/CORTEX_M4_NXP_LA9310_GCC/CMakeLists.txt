#Copyright 2017-2022 NXP

INCLUDE(CMakeForceCompiler)

# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

# THE VERSION NUMBER
SET (Tutorial_VERSION_MAJOR 1)
SET (Tutorial_VERSION_MINOR 0)

# ENABLE ASM
ENABLE_LANGUAGE(ASM)

SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)

SET(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)

# CURRENT DIRECTORY
SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})

# RELEASE LINK FILE
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${ProjDirPath}/platform/ARM_CM4/la9310.ld  -static")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -g  -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g  -fno-strict-aliasing  -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99 -Werror -Wno-address-of-packed-member")

# RELEASE LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -g  -mcpu=cortex-m4  -mfloat-abi=soft  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -D__DEBUG")

#Enable/Disable command line
SET(LA9310_TURN_ON_COMMAND_LINE ON)

# Enable/Disable RFIC driver
SET(RFIC ON)

# Enable/Disable DCS driver
SET(DCS ON)

# Enable/Disable DAC
SET(DAC ON)

# Set logging mode in NMM app:
# 0 / DEFAULT: high-importance messages are printed on the console, all messages logged in memory
# 1 / DEBUG: All messages are printed on the console and logged in memory
# 2 / SILENT: nothing on console, high-importance messages written to memory log
SET(NMM_APP_LOGGING 0)

# Enable/Disable VSPA timer in NMM app
SET(NMM_APP_VSPA_TIMER ON)

# C MACRO
if(LA9310_TURN_ON_COMMAND_LINE)
    SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -D__NDEBUG -DLA9310_ENABLE_COMMAND_LINE")
else()
    SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -D__NDEBUG")
endif()

if(RFIC)
    SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -D__RFIC")
endif()

if(DCS)
    SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -D__DCS")
endif()

if(DAC)
    SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DDAC")
endif()

if(NMM_APP_LOGGING)
    SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNMM_APP_LOGGING=${NMM_APP_LOGGING}")
endif()

if(NMM_APP_VSPA_TIMER)
    SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNMM_APP_VSPA_TIMER")
endif()

# CXX MACRO

# INCLUDE_DIRECTORIES
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../Source/portable/GCC/ARM_CM4)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../..//Source/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/platform/ARM_CM4/include/)
    INCLUDE_DIRECTORIES(${ProjDirPath}/platform/boards/la9310/)
    INCLUDE_DIRECTORIES(${ProjDirPath}/drivers/misc/)
    INCLUDE_DIRECTORIES(${ProjDirPath}/drivers/edma/)
    INCLUDE_DIRECTORIES(${ProjDirPath}/drivers/watchdog/)
    INCLUDE_DIRECTORIES(${ProjDirPath}/drivers/i2c/)
    INCLUDE_DIRECTORIES(${ProjDirPath}/drivers/dspi)
    INCLUDE_DIRECTORIES(${ProjDirPath}/drivers/phy_timer)
    INCLUDE_DIRECTORIES(${ProjDirPath}/drivers/timesync)
    INCLUDE_DIRECTORIES(${ProjDirPath}/sw_cmd_engine)
    INCLUDE_DIRECTORIES($ENV{LA9310_COMMON_HEADERS}/)
if(LA9310_TURN_ON_COMMAND_LINE)
    INCLUDE_DIRECTORIES(${ProjDirPath}/utils/)
    INCLUDE_DIRECTORIES(${ProjDirPath}/utils/FreeRTOS-Plus-CLI-Source/)
endif()
if(RFIC)
    INCLUDE_DIRECTORIES(${ProjDirPath}/drivers/rfic/)
endif()
if(DCS)
    INCLUDE_DIRECTORIES(${ProjDirPath}/drivers/dcs)
endif()

SET(LA9310_SOURCES
    "${ProjDirPath}/../../Source/portable/GCC/ARM_CM4/port.c"
    "${ProjDirPath}/../../Source/portable/MemMang/heap_2.c"
    "${ProjDirPath}/../../Source/croutine.c"
    "${ProjDirPath}/../../Source/event_groups.c"
    "${ProjDirPath}/../../Source/list.c"
    "${ProjDirPath}/../../Source/queue.c"
    "${ProjDirPath}/../../Source/tasks.c"
    "${ProjDirPath}/../../Source/timers.c"
    "${ProjDirPath}/drivers/serial/debug_console.c"
    "${ProjDirPath}/drivers/serial/print_scan.c"
    "${ProjDirPath}/drivers/serial/serial_ns16550.c"
    "${ProjDirPath}/drivers/serial/nmm_logging.c"
    "${ProjDirPath}/drivers/edma/la9310_edma.c"
    "${ProjDirPath}/drivers/misc/mem_log.c"
    "${ProjDirPath}/drivers/watchdog/la9310_watchdog.c"
    "${ProjDirPath}/platform/ARM_CM4/soc.c"
    "${ProjDirPath}/platform/ARM_CM4/start.S"
    "${ProjDirPath}/platform/boards/la9310/board.c"
    "${ProjDirPath}/platform/ARM_CM4/exceptions.c"
    "${ProjDirPath}/platform/boards/la9310/hardware_init.c"
    "${ProjDirPath}/main.c"
    "${ProjDirPath}/cmd_ctrl_vspa_intfce.c"
    "${ProjDirPath}/ps_nr_pseudo_random_generator.c"
	"${ProjDirPath}/ps_nr_pbch_sym_descrambling.c"
	"${ProjDirPath}/ps_nr_pbch_bit_deinterleaver.c"
    "${ProjDirPath}/ps_nr_pbch_dmrs_gen.c"
	"${ProjDirPath}/ps_nr_pbch_deratematching.c"
	"${ProjDirPath}/ps_nr_pbch_bit_descrambling.c"
    "${ProjDirPath}/ps_nr_pbch_re_demapper.c"
	"${ProjDirPath}/ps_nr_pbch_crc.c"
	"${ProjDirPath}/ps_nr_pbch_polar_decoder.c"
	"${ProjDirPath}/ps_nr_mib_parsing.c"	
    "${ProjDirPath}/nmm_avi.c"
    "${ProjDirPath}/nmm_app.c"
    "${ProjDirPath}/nmm_app_utilities.c"
    "${ProjDirPath}/la9310_irq.c"
    "${ProjDirPath}/la9310_v2h_test.c"
    "${ProjDirPath}/drivers/misc/bbdev_ipc.c"
    "${ProjDirPath}/drivers/avi/la9310_avi.c"
    "${ProjDirPath}/drivers/misc/la9310_gpio.c"
    "${ProjDirPath}/drivers/misc/la9310_pinmux.c"
    "${ProjDirPath}/drivers/i2c/la9310_i2c.c"
    "${ProjDirPath}/drivers/pcie/la9310_pcie.c"
    "${ProjDirPath}/drivers/dspi/fsl_dspi.c"
    "${ProjDirPath}/drivers/dspi/dspi_test.c"
    "${ProjDirPath}/drivers/phy_timer/phytimer.c"
    "${ProjDirPath}/drivers/timesync/sync_timing_device.c"
    "${ProjDirPath}/sw_cmd_engine/sw_cmd_engine.c"
    "${ProjDirPath}/utils/delay.c"
  )

if(DCS)
    SET(LA9310_SOURCES ${LA9310_SOURCES}
    	"${ProjDirPath}/drivers/dcs/la9310_dcs.c"
      )
endif()

if(LA9310_TURN_ON_COMMAND_LINE)
    SET(LA9310_SOURCES ${LA9310_SOURCES}
        "${ProjDirPath}/utils/FreeRTOS-Plus-CLI-Source/FreeRTOS_CLI.c"
        "${ProjDirPath}/utils/command_from_shell.c"
        "${ProjDirPath}/utils/la9310_nlm_test_command.c"
        "${ProjDirPath}/utils/UARTCommandConsole.c"
        "${ProjDirPath}/drivers/timesync/sync_timing_device_cli.c"
      )
endif()

if(RFIC)
    SET(LA9310_SOURCES ${LA9310_SOURCES}
        "${ProjDirPath}/drivers/rfic/rfic_init.c"
        "${ProjDirPath}/drivers/rfic/rfic_core.c"
        "${ProjDirPath}/drivers/rfic/rfic_avi_ctrl.c"
        "${ProjDirPath}/drivers/rfic/rfic_api.c"
        "${ProjDirPath}/drivers/rfic/rfic_vga.c"
        "${ProjDirPath}/drivers/rfic/rfic_synth.c"
        "${ProjDirPath}/drivers/rfic/rfic_demod.c"
        "${ProjDirPath}/drivers/rfic/rfic_cmd.c"
        "${ProjDirPath}/drivers/rfic/rfic_dac.c"
        )
endif()

ADD_EXECUTABLE(la9310 ${LA9310_SOURCES})

SET_TARGET_PROPERTIES(la9310 PROPERTIES OUTPUT_NAME "la9310.elf")

TARGET_LINK_LIBRARIES(la9310 -Wl,--start-group)
# LIBRARIES
#IF(CMAKE_BUILD_TYPE MATCHES Debug)
#ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
#ENDIF()

# SYSTEM LIBRARIES
TARGET_LINK_LIBRARIES(la9310 m)
TARGET_LINK_LIBRARIES(la9310 c)
TARGET_LINK_LIBRARIES(la9310 gcc)
TARGET_LINK_LIBRARIES(la9310 nosys)
TARGET_LINK_LIBRARIES(la9310 -Wl,--end-group)

# MAP FILE
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  -Xlinker -Map=debug/la9310.map")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  -Xlinker -Map=release/la9310.map")

# BIN AND HEX
ADD_CUSTOM_COMMAND(TARGET la9310 POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/la9310.elf ${EXECUTABLE_OUTPUT_PATH}/la9310.hex)
ADD_CUSTOM_COMMAND(TARGET la9310 POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE_OUTPUT_PATH}/la9310.elf ${EXECUTABLE_OUTPUT_PATH}/la9310.bin)
