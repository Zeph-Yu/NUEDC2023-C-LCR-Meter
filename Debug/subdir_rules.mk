################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/TI/CCS_2026/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"syscfg/device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"D:/CCS_workplace_2026_new/Test" -I"D:/CCS_workplace_2026_new/Test/Debug" -I"D:/TI/SDK/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"D:/TI/SDK/mspm0_sdk_2_09_00_01/source" -I"D:/CCS_workplace_2026_new/Test/System" -I"D:/CCS_workplace_2026_new/Test/Hardware" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"D:/CCS_workplace_2026_new/Test/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1430597425: ../LCR2.0.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"D:/TI/Sysconfig/sysconfig_1.25.0/sysconfig_cli.bat" -s "D:/TI/SDK/mspm0_sdk_2_09_00_01/.metadata/product.json" --script "D:/CCS_workplace_2026_new/Test/LCR2.0.syscfg" -o "syscfg" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/device_linker.cmd: build-1430597425 ../LCR2.0.syscfg
syscfg/device.opt: build-1430597425
syscfg/device.cmd.genlibs: build-1430597425
syscfg/ti_msp_dl_config.c: build-1430597425
syscfg/ti_msp_dl_config.h: build-1430597425
syscfg/Event.dot: build-1430597425
syscfg: build-1430597425

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/TI/CCS_2026/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"syscfg/device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"D:/CCS_workplace_2026_new/Test" -I"D:/CCS_workplace_2026_new/Test/Debug" -I"D:/TI/SDK/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"D:/TI/SDK/mspm0_sdk_2_09_00_01/source" -I"D:/CCS_workplace_2026_new/Test/System" -I"D:/CCS_workplace_2026_new/Test/Hardware" -gdwarf-3 -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"D:/CCS_workplace_2026_new/Test/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: D:/TI/SDK/mspm0_sdk_2_09_00_01/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/TI/CCS_2026/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"syscfg/device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"D:/CCS_workplace_2026_new/Test" -I"D:/CCS_workplace_2026_new/Test/Debug" -I"D:/TI/SDK/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"D:/TI/SDK/mspm0_sdk_2_09_00_01/source" -I"D:/CCS_workplace_2026_new/Test/System" -I"D:/CCS_workplace_2026_new/Test/Hardware" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"D:/CCS_workplace_2026_new/Test/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


