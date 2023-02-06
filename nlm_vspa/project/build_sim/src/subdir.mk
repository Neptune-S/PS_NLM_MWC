################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
SX_SRCS_QUOTED += \
"$(SRC)/atan_asm.sx" \
"$(SRC)/bitRev512.sx" \
"$(SRC)/compensate_cfo.sx" \
"$(SRC)/copy_vec_asm.sx" \
"$(SRC)/dfe_proc_nr.sx" \
"$(SRC)/ffo_hypo_corr_asm.sx" \
"$(SRC)/fftBR512HP.sx" \
"$(SRC)/fftDIF512_hfx_hfl.sx" \
"$(SRC)/fftDIF512_hfx_hfx.sx" \
"$(SRC)/fftDIF512_hfx_sfl.sx" \
"$(SRC)/fftDIT512_hfx_hfx.sx" \
"$(SRC)/fftDIT512_sflsfl.sx" \
"$(SRC)/nco_upd_asm.sx" \
"$(SRC)/nr_full_corr_asm_v2.sx" \
"$(SRC)/pssdet_rssi_asm.sx" \
"$(SRC)/pssdet_xcorr_fft_asm.sx" \
"$(SRC)/ptrsDecimator.sx" \
"$(SRC)/sssdet_asm.sx" \

SX_SRCS += \
$(SRC)/atan_asm.sx \
$(SRC)/bitRev512.sx \
$(SRC)/compensate_cfo.sx \
$(SRC)/copy_vec_asm.sx \
$(SRC)/dfe_proc_nr.sx \
$(SRC)/ffo_hypo_corr_asm.sx \
$(SRC)/fftBR512HP.sx \
$(SRC)/fftDIF512_hfx_hfl.sx \
$(SRC)/fftDIF512_hfx_hfx.sx \
$(SRC)/fftDIF512_hfx_sfl.sx \
$(SRC)/fftDIT512_hfx_hfx.sx \
$(SRC)/fftDIT512_sflsfl.sx \
$(SRC)/nco_upd_asm.sx \
$(SRC)/nr_full_corr_asm_v2.sx \
$(SRC)/pssdet_rssi_asm.sx \
$(SRC)/pssdet_xcorr_fft_asm.sx \
$(SRC)/ptrsDecimator.sx \
$(SRC)/sssdet_asm.sx \

C_SRCS_QUOTED += \
"$(SRC)/SSS.c" \
"$(SRC)/_cell_search.c" \
"$(SRC)/_harness.c" \
"$(SRC)/atan_alloc.c" \
"$(SRC)/bitRev512Invoke.c" \
"$(SRC)/capture.c" \
"$(SRC)/dfe_proc.c" \
"$(SRC)/nr_pssdet_xcorr_fft.c" \
"$(SRC)/system_resources.c" \

C_SRCS += \
$(SRC)/SSS.c \
$(SRC)/_cell_search.c \
$(SRC)/_harness.c \
$(SRC)/atan_alloc.c \
$(SRC)/bitRev512Invoke.c \
$(SRC)/capture.c \
$(SRC)/dfe_proc.c \
$(SRC)/nr_pssdet_xcorr_fft.c \
$(SRC)/system_resources.c \

S_SRCS += \
$(SRC)/crt0.s \

S_SRCS_QUOTED += \
"$(SRC)/crt0.s" \

S_DEPS_QUOTED += \
"./src/crt0.d" \

OBJS += \
./src/SSS.eln \
./src/_cell_search.eln \
./src/_harness.eln \
./src/atan_alloc.eln \
./src/atan_asm.eln \
./src/bitRev512.eln \
./src/bitRev512Invoke.eln \
./src/capture.eln \
./src/compensate_cfo.eln \
./src/copy_vec_asm.eln \
./src/crt0.eln \
./src/dfe_proc.eln \
./src/dfe_proc_nr.eln \
./src/ffo_hypo_corr_asm.eln \
./src/fftBR512HP.eln \
./src/fftDIF512_hfx_hfl.eln \
./src/fftDIF512_hfx_hfx.eln \
./src/fftDIF512_hfx_sfl.eln \
./src/fftDIT512_hfx_hfx.eln \
./src/fftDIT512_sflsfl.eln \
./src/nco_upd_asm.eln \
./src/nr_full_corr_asm_v2.eln \
./src/nr_pssdet_xcorr_fft.eln \
./src/pssdet_rssi_asm.eln \
./src/pssdet_xcorr_fft_asm.eln \
./src/ptrsDecimator.eln \
./src/sssdet_asm.eln \
./src/system_resources.eln \

S_DEPS += \
./src/crt0.d \

OBJS_QUOTED += \
"./src/SSS.eln" \
"./src/_cell_search.eln" \
"./src/_harness.eln" \
"./src/atan_alloc.eln" \
"./src/atan_asm.eln" \
"./src/bitRev512.eln" \
"./src/bitRev512Invoke.eln" \
"./src/capture.eln" \
"./src/compensate_cfo.eln" \
"./src/copy_vec_asm.eln" \
"./src/crt0.eln" \
"./src/dfe_proc.eln" \
"./src/dfe_proc_nr.eln" \
"./src/ffo_hypo_corr_asm.eln" \
"./src/fftBR512HP.eln" \
"./src/fftDIF512_hfx_hfl.eln" \
"./src/fftDIF512_hfx_hfx.eln" \
"./src/fftDIF512_hfx_sfl.eln" \
"./src/fftDIT512_hfx_hfx.eln" \
"./src/fftDIT512_sflsfl.eln" \
"./src/nco_upd_asm.eln" \
"./src/nr_full_corr_asm_v2.eln" \
"./src/nr_pssdet_xcorr_fft.eln" \
"./src/pssdet_rssi_asm.eln" \
"./src/pssdet_xcorr_fft_asm.eln" \
"./src/ptrsDecimator.eln" \
"./src/sssdet_asm.eln" \
"./src/system_resources.eln" \

C_DEPS += \
./src/SSS.d \
./src/_cell_search.d \
./src/_harness.d \
./src/atan_alloc.d \
./src/bitRev512Invoke.d \
./src/capture.d \
./src/dfe_proc.d \
./src/nr_pssdet_xcorr_fft.d \
./src/system_resources.d \

SX_DEPS += \
./src/atan_asm.d \
./src/bitRev512.d \
./src/compensate_cfo.d \
./src/copy_vec_asm.d \
./src/dfe_proc_nr.d \
./src/ffo_hypo_corr_asm.d \
./src/fftBR512HP.d \
./src/fftDIF512_hfx_hfl.d \
./src/fftDIF512_hfx_hfx.d \
./src/fftDIF512_hfx_sfl.d \
./src/fftDIT512_hfx_hfx.d \
./src/fftDIT512_sflsfl.d \
./src/nco_upd_asm.d \
./src/nr_full_corr_asm_v2.d \
./src/pssdet_rssi_asm.d \
./src/pssdet_xcorr_fft_asm.d \
./src/ptrsDecimator.d \
./src/sssdet_asm.d \

SX_DEPS_QUOTED += \
"./src/atan_asm.d" \
"./src/bitRev512.d" \
"./src/compensate_cfo.d" \
"./src/copy_vec_asm.d" \
"./src/dfe_proc_nr.d" \
"./src/ffo_hypo_corr_asm.d" \
"./src/fftBR512HP.d" \
"./src/fftDIF512_hfx_hfl.d" \
"./src/fftDIF512_hfx_hfx.d" \
"./src/fftDIF512_hfx_sfl.d" \
"./src/fftDIT512_hfx_hfx.d" \
"./src/fftDIT512_sflsfl.d" \
"./src/nco_upd_asm.d" \
"./src/nr_full_corr_asm_v2.d" \
"./src/pssdet_rssi_asm.d" \
"./src/pssdet_xcorr_fft_asm.d" \
"./src/ptrsDecimator.d" \
"./src/sssdet_asm.d" \

C_DEPS_QUOTED += \
"./src/SSS.d" \
"./src/_cell_search.d" \
"./src/_harness.d" \
"./src/atan_alloc.d" \
"./src/bitRev512Invoke.d" \
"./src/capture.d" \
"./src/dfe_proc.d" \
"./src/nr_pssdet_xcorr_fft.d" \
"./src/system_resources.d" \

OBJS_OS_FORMAT += \
./src/SSS.eln \
./src/_cell_search.eln \
./src/_harness.eln \
./src/atan_alloc.eln \
./src/atan_asm.eln \
./src/bitRev512.eln \
./src/bitRev512Invoke.eln \
./src/capture.eln \
./src/compensate_cfo.eln \
./src/copy_vec_asm.eln \
./src/crt0.eln \
./src/dfe_proc.eln \
./src/dfe_proc_nr.eln \
./src/ffo_hypo_corr_asm.eln \
./src/fftBR512HP.eln \
./src/fftDIF512_hfx_hfl.eln \
./src/fftDIF512_hfx_hfx.eln \
./src/fftDIF512_hfx_sfl.eln \
./src/fftDIT512_hfx_hfx.eln \
./src/fftDIT512_sflsfl.eln \
./src/nco_upd_asm.eln \
./src/nr_full_corr_asm_v2.eln \
./src/nr_pssdet_xcorr_fft.eln \
./src/pssdet_rssi_asm.eln \
./src/pssdet_xcorr_fft_asm.eln \
./src/ptrsDecimator.eln \
./src/sssdet_asm.eln \
./src/system_resources.eln \


# Each subdirectory must supply rules for building sources it contributes
src/SSS.eln: $(SRC)/SSS.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/SSS.args" -o "src/SSS.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/SSS.d: $(SRC)/SSS.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/_cell_search.eln: $(SRC)/_cell_search.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/_cell_search.args" -o "src/_cell_search.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/_cell_search.d: $(SRC)/_cell_search.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/_harness.eln: $(SRC)/_harness.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/_harness.args" -o "src/_harness.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/_harness.d: $(SRC)/_harness.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/atan_alloc.eln: $(SRC)/atan_alloc.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/atan_alloc.args" -o "src/atan_alloc.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/atan_alloc.d: $(SRC)/atan_alloc.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/atan_asm.eln: $(SRC)/atan_asm.sx
	@echo 'Building file: $<'
	@echo 'Executing target #5 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/atan_asm.args" -o "src/atan_asm.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/atan_asm.d: $(SRC)/atan_asm.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/bitRev512.eln: $(SRC)/bitRev512.sx
	@echo 'Building file: $<'
	@echo 'Executing target #6 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/bitRev512.args" -o "src/bitRev512.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/bitRev512.d: $(SRC)/bitRev512.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/bitRev512Invoke.eln: $(SRC)/bitRev512Invoke.c
	@echo 'Building file: $<'
	@echo 'Executing target #7 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/bitRev512Invoke.args" -o "src/bitRev512Invoke.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/bitRev512Invoke.d: $(SRC)/bitRev512Invoke.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/capture.eln: $(SRC)/capture.c
	@echo 'Building file: $<'
	@echo 'Executing target #8 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/capture.args" -o "src/capture.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/capture.d: $(SRC)/capture.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/compensate_cfo.eln: $(SRC)/compensate_cfo.sx
	@echo 'Building file: $<'
	@echo 'Executing target #9 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/compensate_cfo.args" -o "src/compensate_cfo.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/compensate_cfo.d: $(SRC)/compensate_cfo.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/copy_vec_asm.eln: $(SRC)/copy_vec_asm.sx
	@echo 'Building file: $<'
	@echo 'Executing target #10 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/copy_vec_asm.args" -o "src/copy_vec_asm.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/copy_vec_asm.d: $(SRC)/copy_vec_asm.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/crt0.eln: $(SRC)/crt0.s
	@echo 'Building file: $<'
	@echo 'Executing target #11 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/crt0.args" -o "src/crt0.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/crt0.d: $(SRC)/crt0.s
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/dfe_proc.eln: $(SRC)/dfe_proc.c
	@echo 'Building file: $<'
	@echo 'Executing target #12 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/dfe_proc.args" -o "src/dfe_proc.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/dfe_proc.d: $(SRC)/dfe_proc.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/dfe_proc_nr.eln: $(SRC)/dfe_proc_nr.sx
	@echo 'Building file: $<'
	@echo 'Executing target #13 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/dfe_proc_nr.args" -o "src/dfe_proc_nr.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/dfe_proc_nr.d: $(SRC)/dfe_proc_nr.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/ffo_hypo_corr_asm.eln: $(SRC)/ffo_hypo_corr_asm.sx
	@echo 'Building file: $<'
	@echo 'Executing target #14 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/ffo_hypo_corr_asm.args" -o "src/ffo_hypo_corr_asm.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/ffo_hypo_corr_asm.d: $(SRC)/ffo_hypo_corr_asm.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/fftBR512HP.eln: $(SRC)/fftBR512HP.sx
	@echo 'Building file: $<'
	@echo 'Executing target #15 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/fftBR512HP.args" -o "src/fftBR512HP.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/fftBR512HP.d: $(SRC)/fftBR512HP.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/fftDIF512_hfx_hfl.eln: $(SRC)/fftDIF512_hfx_hfl.sx
	@echo 'Building file: $<'
	@echo 'Executing target #16 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/fftDIF512_hfx_hfl.args" -o "src/fftDIF512_hfx_hfl.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/fftDIF512_hfx_hfl.d: $(SRC)/fftDIF512_hfx_hfl.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/fftDIF512_hfx_hfx.eln: $(SRC)/fftDIF512_hfx_hfx.sx
	@echo 'Building file: $<'
	@echo 'Executing target #17 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/fftDIF512_hfx_hfx.args" -o "src/fftDIF512_hfx_hfx.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/fftDIF512_hfx_hfx.d: $(SRC)/fftDIF512_hfx_hfx.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/fftDIF512_hfx_sfl.eln: $(SRC)/fftDIF512_hfx_sfl.sx
	@echo 'Building file: $<'
	@echo 'Executing target #18 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/fftDIF512_hfx_sfl.args" -o "src/fftDIF512_hfx_sfl.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/fftDIF512_hfx_sfl.d: $(SRC)/fftDIF512_hfx_sfl.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/fftDIT512_hfx_hfx.eln: $(SRC)/fftDIT512_hfx_hfx.sx
	@echo 'Building file: $<'
	@echo 'Executing target #19 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/fftDIT512_hfx_hfx.args" -o "src/fftDIT512_hfx_hfx.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/fftDIT512_hfx_hfx.d: $(SRC)/fftDIT512_hfx_hfx.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/fftDIT512_sflsfl.eln: $(SRC)/fftDIT512_sflsfl.sx
	@echo 'Building file: $<'
	@echo 'Executing target #20 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/fftDIT512_sflsfl.args" -o "src/fftDIT512_sflsfl.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/fftDIT512_sflsfl.d: $(SRC)/fftDIT512_sflsfl.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/nco_upd_asm.eln: $(SRC)/nco_upd_asm.sx
	@echo 'Building file: $<'
	@echo 'Executing target #21 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/nco_upd_asm.args" -o "src/nco_upd_asm.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/nco_upd_asm.d: $(SRC)/nco_upd_asm.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/nr_full_corr_asm_v2.eln: $(SRC)/nr_full_corr_asm_v2.sx
	@echo 'Building file: $<'
	@echo 'Executing target #22 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/nr_full_corr_asm_v2.args" -o "src/nr_full_corr_asm_v2.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/nr_full_corr_asm_v2.d: $(SRC)/nr_full_corr_asm_v2.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/nr_pssdet_xcorr_fft.eln: $(SRC)/nr_pssdet_xcorr_fft.c
	@echo 'Building file: $<'
	@echo 'Executing target #23 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/nr_pssdet_xcorr_fft.args" -o "src/nr_pssdet_xcorr_fft.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/nr_pssdet_xcorr_fft.d: $(SRC)/nr_pssdet_xcorr_fft.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/pssdet_rssi_asm.eln: $(SRC)/pssdet_rssi_asm.sx
	@echo 'Building file: $<'
	@echo 'Executing target #24 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/pssdet_rssi_asm.args" -o "src/pssdet_rssi_asm.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/pssdet_rssi_asm.d: $(SRC)/pssdet_rssi_asm.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/pssdet_xcorr_fft_asm.eln: $(SRC)/pssdet_xcorr_fft_asm.sx
	@echo 'Building file: $<'
	@echo 'Executing target #25 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/pssdet_xcorr_fft_asm.args" -o "src/pssdet_xcorr_fft_asm.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/pssdet_xcorr_fft_asm.d: $(SRC)/pssdet_xcorr_fft_asm.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/ptrsDecimator.eln: $(SRC)/ptrsDecimator.sx
	@echo 'Building file: $<'
	@echo 'Executing target #26 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/ptrsDecimator.args" -o "src/ptrsDecimator.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/ptrsDecimator.d: $(SRC)/ptrsDecimator.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/sssdet_asm.eln: $(SRC)/sssdet_asm.sx
	@echo 'Building file: $<'
	@echo 'Executing target #27 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/sssdet_asm.args" -o "src/sssdet_asm.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/sssdet_asm.d: $(SRC)/sssdet_asm.sx
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/system_resources.eln: $(SRC)/system_resources.c
	@echo 'Building file: $<'
	@echo 'Executing target #28 $<'
	@echo 'Invoking: VSPA C Compiler'
	"$(VSPAiSCAPEToolsInstallDirEnv)/fsvspacc" @@"src/system_resources.args" -o "src/system_resources.eln" -c "$<" -inline all -o "src" -MDfile "$(@:%.eln=%.d)"
	@echo 'Finished building: $<'
	@echo ' '

src/system_resources.d: $(SRC)/system_resources.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '


