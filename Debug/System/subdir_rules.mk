################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
System/%.o: ../System/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/TI/CCS_2026/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"syscfg/device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"D:/CCS_workplace_2026_new/Test" -I"D:/CCS_workplace_2026_new/Test/Debug" -I"D:/TI/SDK/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"D:/TI/SDK/mspm0_sdk_2_09_00_01/source" -I"D:/CCS_workplace_2026_new/Test/System" -I"D:/CCS_workplace_2026_new/Test/Hardware" -gdwarf-3 -MMD -MP -MF"System/$(basename $(<F)).d_raw" -MT"$(@)" -I"D:/CCS_workplace_2026_new/Test/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


