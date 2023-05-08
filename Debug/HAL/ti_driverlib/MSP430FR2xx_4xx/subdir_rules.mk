################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
HAL/ti_driverlib/MSP430FR2xx_4xx/%.obj: ../HAL/ti_driverlib/MSP430FR2xx_4xx/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ccstudio/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/bin/cl430" -vmspx --use_hw_mpy=none --include_path="/opt/ccstudio/ccs/ccs_base/msp430/include" --include_path="/home/silas/.ccstudio_workspace_v12/ECE342Microcontroller/HAL" --include_path="/home/silas/.ccstudio_workspace_v12/ECE342Microcontroller/HAL/ti_driverlib/MSP430FR2xx_4xx" --include_path="/home/silas/.ccstudio_workspace_v12/ECE342Microcontroller" --include_path="/opt/ccstudio/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR4133__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="HAL/ti_driverlib/MSP430FR2xx_4xx/$(basename $(<F)).d_raw" --obj_directory="HAL/ti_driverlib/MSP430FR2xx_4xx" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


