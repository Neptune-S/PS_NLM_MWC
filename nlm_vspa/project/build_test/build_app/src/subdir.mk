################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
SL_SRCS_QUOTED += \
"../build_app/src/SSS.sl" \
"../build_app/src/_cell_search.sl" \
"../build_app/src/_harness.sl" \
"../build_app/src/atan_alloc.sl" \
"../build_app/src/atan_asm.sl" \
"../build_app/src/bitRev512.sl" \
"../build_app/src/bitRev512Invoke.sl" \
"../build_app/src/capture.sl" \
"../build_app/src/compensate_cfo.sl" \
"../build_app/src/copy_vec_asm.sl" \
"../build_app/src/dfe_proc.sl" \
"../build_app/src/dfe_proc_nr.sl" \
"../build_app/src/ffo_hypo_corr_asm.sl" \
"../build_app/src/fftBR512HP.sl" \
"../build_app/src/fftDIF512_hfx_hfl.sl" \
"../build_app/src/fftDIF512_hfx_hfx.sl" \
"../build_app/src/fftDIF512_hfx_sfl.sl" \
"../build_app/src/fftDIT512_hfx_hfx.sl" \
"../build_app/src/fftDIT512_sflsfl.sl" \
"../build_app/src/nco_upd_asm.sl" \
"../build_app/src/nr_full_corr_asm_v2.sl" \
"../build_app/src/nr_pssdet_xcorr_fft.sl" \
"../build_app/src/pssdet_rssi_asm.sl" \
"../build_app/src/pssdet_xcorr_fft_asm.sl" \
"../build_app/src/ptrsDecimator.sl" \
"../build_app/src/sssdet_asm.sl" \
"../build_app/src/system_resources.sl" \

S1_SRCS_QUOTED += \
"../build_app/src/SSS.s1" \
"../build_app/src/_cell_search.s1" \
"../build_app/src/_harness.s1" \
"../build_app/src/atan_alloc.s1" \
"../build_app/src/bitRev512Invoke.s1" \
"../build_app/src/capture.s1" \
"../build_app/src/dfe_proc.s1" \
"../build_app/src/nr_pssdet_xcorr_fft.s1" \
"../build_app/src/system_resources.s1" \

S1_SRCS += \
../build_app/src/SSS.s1 \
../build_app/src/_cell_search.s1 \
../build_app/src/_harness.s1 \
../build_app/src/atan_alloc.s1 \
../build_app/src/bitRev512Invoke.s1 \
../build_app/src/capture.s1 \
../build_app/src/dfe_proc.s1 \
../build_app/src/nr_pssdet_xcorr_fft.s1 \
../build_app/src/system_resources.s1 \

SL_SRCS += \
../build_app/src/SSS.sl \
../build_app/src/_cell_search.sl \
../build_app/src/_harness.sl \
../build_app/src/atan_alloc.sl \
../build_app/src/atan_asm.sl \
../build_app/src/bitRev512.sl \
../build_app/src/bitRev512Invoke.sl \
../build_app/src/capture.sl \
../build_app/src/compensate_cfo.sl \
../build_app/src/copy_vec_asm.sl \
../build_app/src/dfe_proc.sl \
../build_app/src/dfe_proc_nr.sl \
../build_app/src/ffo_hypo_corr_asm.sl \
../build_app/src/fftBR512HP.sl \
../build_app/src/fftDIF512_hfx_hfl.sl \
../build_app/src/fftDIF512_hfx_hfx.sl \
../build_app/src/fftDIF512_hfx_sfl.sl \
../build_app/src/fftDIT512_hfx_hfx.sl \
../build_app/src/fftDIT512_sflsfl.sl \
../build_app/src/nco_upd_asm.sl \
../build_app/src/nr_full_corr_asm_v2.sl \
../build_app/src/nr_pssdet_xcorr_fft.sl \
../build_app/src/pssdet_rssi_asm.sl \
../build_app/src/pssdet_xcorr_fft_asm.sl \
../build_app/src/ptrsDecimator.sl \
../build_app/src/sssdet_asm.sl \
../build_app/src/system_resources.sl \

ELN_SRCS_QUOTED += \
"../build_app/src/SSS.eln" \
"../build_app/src/_cell_search.eln" \
"../build_app/src/_harness.eln" \
"../build_app/src/atan_alloc.eln" \
"../build_app/src/atan_asm.eln" \
"../build_app/src/bitRev512.eln" \
"../build_app/src/bitRev512Invoke.eln" \
"../build_app/src/capture.eln" \
"../build_app/src/compensate_cfo.eln" \
"../build_app/src/copy_vec_asm.eln" \
"../build_app/src/crt0.eln" \
"../build_app/src/dfe_proc.eln" \
"../build_app/src/dfe_proc_nr.eln" \
"../build_app/src/ffo_hypo_corr_asm.eln" \
"../build_app/src/fftBR512HP.eln" \
"../build_app/src/fftDIF512_hfx_hfl.eln" \
"../build_app/src/fftDIF512_hfx_hfx.eln" \
"../build_app/src/fftDIF512_hfx_sfl.eln" \
"../build_app/src/fftDIT512_hfx_hfx.eln" \
"../build_app/src/fftDIT512_sflsfl.eln" \
"../build_app/src/nco_upd_asm.eln" \
"../build_app/src/nr_full_corr_asm_v2.eln" \
"../build_app/src/nr_pssdet_xcorr_fft.eln" \
"../build_app/src/pssdet_rssi_asm.eln" \
"../build_app/src/pssdet_xcorr_fft_asm.eln" \
"../build_app/src/ptrsDecimator.eln" \
"../build_app/src/sssdet_asm.eln" \
"../build_app/src/system_resources.eln" \

ELN_SRCS += \
../build_app/src/SSS.eln \
../build_app/src/_cell_search.eln \
../build_app/src/_harness.eln \
../build_app/src/atan_alloc.eln \
../build_app/src/atan_asm.eln \
../build_app/src/bitRev512.eln \
../build_app/src/bitRev512Invoke.eln \
../build_app/src/capture.eln \
../build_app/src/compensate_cfo.eln \
../build_app/src/copy_vec_asm.eln \
../build_app/src/crt0.eln \
../build_app/src/dfe_proc.eln \
../build_app/src/dfe_proc_nr.eln \
../build_app/src/ffo_hypo_corr_asm.eln \
../build_app/src/fftBR512HP.eln \
../build_app/src/fftDIF512_hfx_hfl.eln \
../build_app/src/fftDIF512_hfx_hfx.eln \
../build_app/src/fftDIF512_hfx_sfl.eln \
../build_app/src/fftDIT512_hfx_hfx.eln \
../build_app/src/fftDIT512_sflsfl.eln \
../build_app/src/nco_upd_asm.eln \
../build_app/src/nr_full_corr_asm_v2.eln \
../build_app/src/nr_pssdet_xcorr_fft.eln \
../build_app/src/pssdet_rssi_asm.eln \
../build_app/src/pssdet_xcorr_fft_asm.eln \
../build_app/src/ptrsDecimator.eln \
../build_app/src/sssdet_asm.eln \
../build_app/src/system_resources.eln \

OBJS += \
./build_app/src/SSS.eln \
./build_app/src/_cell_search.eln \
./build_app/src/_harness.eln \
./build_app/src/atan_alloc.eln \
./build_app/src/atan_asm.eln \
./build_app/src/bitRev512.eln \
./build_app/src/bitRev512Invoke.eln \
./build_app/src/capture.eln \
./build_app/src/compensate_cfo.eln \
./build_app/src/copy_vec_asm.eln \
./build_app/src/dfe_proc.eln \
./build_app/src/dfe_proc_nr.eln \
./build_app/src/ffo_hypo_corr_asm.eln \
./build_app/src/fftBR512HP.eln \
./build_app/src/fftDIF512_hfx_hfl.eln \
./build_app/src/fftDIF512_hfx_hfx.eln \
./build_app/src/fftDIF512_hfx_sfl.eln \
./build_app/src/fftDIT512_hfx_hfx.eln \
./build_app/src/fftDIT512_sflsfl.eln \
./build_app/src/nco_upd_asm.eln \
./build_app/src/nr_full_corr_asm_v2.eln \
./build_app/src/nr_pssdet_xcorr_fft.eln \
./build_app/src/pssdet_rssi_asm.eln \
./build_app/src/pssdet_xcorr_fft_asm.eln \
./build_app/src/ptrsDecimator.eln \
./build_app/src/sssdet_asm.eln \
./build_app/src/system_resources.eln \

SL_DEPS_QUOTED += \
"./build_app/src/SSS.d" \
"./build_app/src/_cell_search.d" \
"./build_app/src/_harness.d" \
"./build_app/src/atan_alloc.d" \
"./build_app/src/atan_asm.d" \
"./build_app/src/bitRev512.d" \
"./build_app/src/bitRev512Invoke.d" \
"./build_app/src/capture.d" \
"./build_app/src/compensate_cfo.d" \
"./build_app/src/copy_vec_asm.d" \
"./build_app/src/dfe_proc.d" \
"./build_app/src/dfe_proc_nr.d" \
"./build_app/src/ffo_hypo_corr_asm.d" \
"./build_app/src/fftBR512HP.d" \
"./build_app/src/fftDIF512_hfx_hfl.d" \
"./build_app/src/fftDIF512_hfx_hfx.d" \
"./build_app/src/fftDIF512_hfx_sfl.d" \
"./build_app/src/fftDIT512_hfx_hfx.d" \
"./build_app/src/fftDIT512_sflsfl.d" \
"./build_app/src/nco_upd_asm.d" \
"./build_app/src/nr_full_corr_asm_v2.d" \
"./build_app/src/nr_pssdet_xcorr_fft.d" \
"./build_app/src/pssdet_rssi_asm.d" \
"./build_app/src/pssdet_xcorr_fft_asm.d" \
"./build_app/src/ptrsDecimator.d" \
"./build_app/src/sssdet_asm.d" \
"./build_app/src/system_resources.d" \

OBJS_QUOTED += \
"./build_app/src/SSS.eln" \
"./build_app/src/_cell_search.eln" \
"./build_app/src/_harness.eln" \
"./build_app/src/atan_alloc.eln" \
"./build_app/src/atan_asm.eln" \
"./build_app/src/bitRev512.eln" \
"./build_app/src/bitRev512Invoke.eln" \
"./build_app/src/capture.eln" \
"./build_app/src/compensate_cfo.eln" \
"./build_app/src/copy_vec_asm.eln" \
"./build_app/src/dfe_proc.eln" \
"./build_app/src/dfe_proc_nr.eln" \
"./build_app/src/ffo_hypo_corr_asm.eln" \
"./build_app/src/fftBR512HP.eln" \
"./build_app/src/fftDIF512_hfx_hfl.eln" \
"./build_app/src/fftDIF512_hfx_hfx.eln" \
"./build_app/src/fftDIF512_hfx_sfl.eln" \
"./build_app/src/fftDIT512_hfx_hfx.eln" \
"./build_app/src/fftDIT512_sflsfl.eln" \
"./build_app/src/nco_upd_asm.eln" \
"./build_app/src/nr_full_corr_asm_v2.eln" \
"./build_app/src/nr_pssdet_xcorr_fft.eln" \
"./build_app/src/pssdet_rssi_asm.eln" \
"./build_app/src/pssdet_xcorr_fft_asm.eln" \
"./build_app/src/ptrsDecimator.eln" \
"./build_app/src/sssdet_asm.eln" \
"./build_app/src/system_resources.eln" \

SL_DEPS += \
./build_app/src/SSS.d \
./build_app/src/_cell_search.d \
./build_app/src/_harness.d \
./build_app/src/atan_alloc.d \
./build_app/src/atan_asm.d \
./build_app/src/bitRev512.d \
./build_app/src/bitRev512Invoke.d \
./build_app/src/capture.d \
./build_app/src/compensate_cfo.d \
./build_app/src/copy_vec_asm.d \
./build_app/src/dfe_proc.d \
./build_app/src/dfe_proc_nr.d \
./build_app/src/ffo_hypo_corr_asm.d \
./build_app/src/fftBR512HP.d \
./build_app/src/fftDIF512_hfx_hfl.d \
./build_app/src/fftDIF512_hfx_hfx.d \
./build_app/src/fftDIF512_hfx_sfl.d \
./build_app/src/fftDIT512_hfx_hfx.d \
./build_app/src/fftDIT512_sflsfl.d \
./build_app/src/nco_upd_asm.d \
./build_app/src/nr_full_corr_asm_v2.d \
./build_app/src/nr_pssdet_xcorr_fft.d \
./build_app/src/pssdet_rssi_asm.d \
./build_app/src/pssdet_xcorr_fft_asm.d \
./build_app/src/ptrsDecimator.d \
./build_app/src/sssdet_asm.d \
./build_app/src/system_resources.d \

OBJS_OS_FORMAT += \
./build_app/src/SSS.eln \
./build_app/src/_cell_search.eln \
./build_app/src/_harness.eln \
./build_app/src/atan_alloc.eln \
./build_app/src/atan_asm.eln \
./build_app/src/bitRev512.eln \
./build_app/src/bitRev512Invoke.eln \
./build_app/src/capture.eln \
./build_app/src/compensate_cfo.eln \
./build_app/src/copy_vec_asm.eln \
./build_app/src/dfe_proc.eln \
./build_app/src/dfe_proc_nr.eln \
./build_app/src/ffo_hypo_corr_asm.eln \
./build_app/src/fftBR512HP.eln \
./build_app/src/fftDIF512_hfx_hfl.eln \
./build_app/src/fftDIF512_hfx_hfx.eln \
./build_app/src/fftDIF512_hfx_sfl.eln \
./build_app/src/fftDIT512_hfx_hfx.eln \
./build_app/src/fftDIT512_sflsfl.eln \
./build_app/src/nco_upd_asm.eln \
./build_app/src/nr_full_corr_asm_v2.eln \
./build_app/src/nr_pssdet_xcorr_fft.eln \
./build_app/src/pssdet_rssi_asm.eln \
./build_app/src/pssdet_xcorr_fft_asm.eln \
./build_app/src/ptrsDecimator.eln \
./build_app/src/sssdet_asm.eln \
./build_app/src/system_resources.eln \


# Each subdirectory must supply rules for building sources it contributes
build_app/src/SSS.eln: ../build_app/src/SSS.sl
	@echo 'Building file: $<'
	@echo 'Executing target #33 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/SSS.args" -o "build_app/src/SSS.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/SSS.d: ../build_app/src/SSS.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/_cell_search.eln: ../build_app/src/_cell_search.sl
	@echo 'Building file: $<'
	@echo 'Executing target #34 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/_cell_search.args" -o "build_app/src/_cell_search.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/_cell_search.d: ../build_app/src/_cell_search.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/_harness.eln: ../build_app/src/_harness.sl
	@echo 'Building file: $<'
	@echo 'Executing target #35 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/_harness.args" -o "build_app/src/_harness.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/_harness.d: ../build_app/src/_harness.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/atan_alloc.eln: ../build_app/src/atan_alloc.sl
	@echo 'Building file: $<'
	@echo 'Executing target #36 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/atan_alloc.args" -o "build_app/src/atan_alloc.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/atan_alloc.d: ../build_app/src/atan_alloc.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/atan_asm.eln: ../build_app/src/atan_asm.sl
	@echo 'Building file: $<'
	@echo 'Executing target #37 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/atan_asm.args" -o "build_app/src/atan_asm.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/atan_asm.d: ../build_app/src/atan_asm.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/bitRev512.eln: ../build_app/src/bitRev512.sl
	@echo 'Building file: $<'
	@echo 'Executing target #38 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/bitRev512.args" -o "build_app/src/bitRev512.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/bitRev512.d: ../build_app/src/bitRev512.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/bitRev512Invoke.eln: ../build_app/src/bitRev512Invoke.sl
	@echo 'Building file: $<'
	@echo 'Executing target #39 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/bitRev512Invoke.args" -o "build_app/src/bitRev512Invoke.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/bitRev512Invoke.d: ../build_app/src/bitRev512Invoke.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/capture.eln: ../build_app/src/capture.sl
	@echo 'Building file: $<'
	@echo 'Executing target #40 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/capture.args" -o "build_app/src/capture.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/capture.d: ../build_app/src/capture.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/compensate_cfo.eln: ../build_app/src/compensate_cfo.sl
	@echo 'Building file: $<'
	@echo 'Executing target #41 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/compensate_cfo.args" -o "build_app/src/compensate_cfo.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/compensate_cfo.d: ../build_app/src/compensate_cfo.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/copy_vec_asm.eln: ../build_app/src/copy_vec_asm.sl
	@echo 'Building file: $<'
	@echo 'Executing target #42 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/copy_vec_asm.args" -o "build_app/src/copy_vec_asm.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/copy_vec_asm.d: ../build_app/src/copy_vec_asm.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/dfe_proc.eln: ../build_app/src/dfe_proc.sl
	@echo 'Building file: $<'
	@echo 'Executing target #43 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/dfe_proc.args" -o "build_app/src/dfe_proc.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/dfe_proc.d: ../build_app/src/dfe_proc.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/dfe_proc_nr.eln: ../build_app/src/dfe_proc_nr.sl
	@echo 'Building file: $<'
	@echo 'Executing target #44 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/dfe_proc_nr.args" -o "build_app/src/dfe_proc_nr.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/dfe_proc_nr.d: ../build_app/src/dfe_proc_nr.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/ffo_hypo_corr_asm.eln: ../build_app/src/ffo_hypo_corr_asm.sl
	@echo 'Building file: $<'
	@echo 'Executing target #45 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/ffo_hypo_corr_asm.args" -o "build_app/src/ffo_hypo_corr_asm.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/ffo_hypo_corr_asm.d: ../build_app/src/ffo_hypo_corr_asm.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/fftBR512HP.eln: ../build_app/src/fftBR512HP.sl
	@echo 'Building file: $<'
	@echo 'Executing target #46 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/fftBR512HP.args" -o "build_app/src/fftBR512HP.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/fftBR512HP.d: ../build_app/src/fftBR512HP.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/fftDIF512_hfx_hfl.eln: ../build_app/src/fftDIF512_hfx_hfl.sl
	@echo 'Building file: $<'
	@echo 'Executing target #47 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/fftDIF512_hfx_hfl.args" -o "build_app/src/fftDIF512_hfx_hfl.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/fftDIF512_hfx_hfl.d: ../build_app/src/fftDIF512_hfx_hfl.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/fftDIF512_hfx_hfx.eln: ../build_app/src/fftDIF512_hfx_hfx.sl
	@echo 'Building file: $<'
	@echo 'Executing target #48 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/fftDIF512_hfx_hfx.args" -o "build_app/src/fftDIF512_hfx_hfx.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/fftDIF512_hfx_hfx.d: ../build_app/src/fftDIF512_hfx_hfx.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/fftDIF512_hfx_sfl.eln: ../build_app/src/fftDIF512_hfx_sfl.sl
	@echo 'Building file: $<'
	@echo 'Executing target #49 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/fftDIF512_hfx_sfl.args" -o "build_app/src/fftDIF512_hfx_sfl.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/fftDIF512_hfx_sfl.d: ../build_app/src/fftDIF512_hfx_sfl.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/fftDIT512_hfx_hfx.eln: ../build_app/src/fftDIT512_hfx_hfx.sl
	@echo 'Building file: $<'
	@echo 'Executing target #50 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/fftDIT512_hfx_hfx.args" -o "build_app/src/fftDIT512_hfx_hfx.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/fftDIT512_hfx_hfx.d: ../build_app/src/fftDIT512_hfx_hfx.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/fftDIT512_sflsfl.eln: ../build_app/src/fftDIT512_sflsfl.sl
	@echo 'Building file: $<'
	@echo 'Executing target #51 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/fftDIT512_sflsfl.args" -o "build_app/src/fftDIT512_sflsfl.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/fftDIT512_sflsfl.d: ../build_app/src/fftDIT512_sflsfl.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/nco_upd_asm.eln: ../build_app/src/nco_upd_asm.sl
	@echo 'Building file: $<'
	@echo 'Executing target #52 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/nco_upd_asm.args" -o "build_app/src/nco_upd_asm.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/nco_upd_asm.d: ../build_app/src/nco_upd_asm.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/nr_full_corr_asm_v2.eln: ../build_app/src/nr_full_corr_asm_v2.sl
	@echo 'Building file: $<'
	@echo 'Executing target #53 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/nr_full_corr_asm_v2.args" -o "build_app/src/nr_full_corr_asm_v2.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/nr_full_corr_asm_v2.d: ../build_app/src/nr_full_corr_asm_v2.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/nr_pssdet_xcorr_fft.eln: ../build_app/src/nr_pssdet_xcorr_fft.sl
	@echo 'Building file: $<'
	@echo 'Executing target #54 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/nr_pssdet_xcorr_fft.args" -o "build_app/src/nr_pssdet_xcorr_fft.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/nr_pssdet_xcorr_fft.d: ../build_app/src/nr_pssdet_xcorr_fft.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/pssdet_rssi_asm.eln: ../build_app/src/pssdet_rssi_asm.sl
	@echo 'Building file: $<'
	@echo 'Executing target #55 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/pssdet_rssi_asm.args" -o "build_app/src/pssdet_rssi_asm.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/pssdet_rssi_asm.d: ../build_app/src/pssdet_rssi_asm.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/pssdet_xcorr_fft_asm.eln: ../build_app/src/pssdet_xcorr_fft_asm.sl
	@echo 'Building file: $<'
	@echo 'Executing target #56 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/pssdet_xcorr_fft_asm.args" -o "build_app/src/pssdet_xcorr_fft_asm.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/pssdet_xcorr_fft_asm.d: ../build_app/src/pssdet_xcorr_fft_asm.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/ptrsDecimator.eln: ../build_app/src/ptrsDecimator.sl
	@echo 'Building file: $<'
	@echo 'Executing target #57 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/ptrsDecimator.args" -o "build_app/src/ptrsDecimator.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/ptrsDecimator.d: ../build_app/src/ptrsDecimator.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/sssdet_asm.eln: ../build_app/src/sssdet_asm.sl
	@echo 'Building file: $<'
	@echo 'Executing target #58 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/sssdet_asm.args" -o "build_app/src/sssdet_asm.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/sssdet_asm.d: ../build_app/src/sssdet_asm.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

build_app/src/system_resources.eln: ../build_app/src/system_resources.sl
	@echo 'Building file: $<'
	@echo 'Executing target #59 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"build_app/src/system_resources.args" -o "build_app/src/system_resources.eln" -c "$<" -inline all -o "build_app/src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

build_app/src/system_resources.d: ../build_app/src/system_resources.sl
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '


