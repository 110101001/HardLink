################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/wpc/UserSoftware/CCS/ccsv8/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7M3 --code_state=16 --float_support=vfplib -me --include_path="/home/wpc/projects/CCS_workspace/rfPacketTx_CC1310_LAUNCHXL_nortos_ccs" --include_path="/home/wpc/ti/simplelink_cc13x0_sdk_4_10_01_01/source" --include_path="/home/wpc/ti/simplelink_cc13x0_sdk_4_10_01_01/kernel/nortos" --include_path="/home/wpc/ti/simplelink_cc13x0_sdk_4_10_01_01/kernel/nortos/posix" --include_path="/home/wpc/UserSoftware/CCS/ccsv8/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --define=DeviceFamily_CC13X0 --define=CCFG_FORCE_VDDR_HH=0 --define=SUPPORT_PHY_CUSTOM --define=SUPPORT_PHY_50KBPS2GFSK --define=SUPPORT_PHY_625BPSLRM --define=SUPPORT_PHY_5KBPSSLLR -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


