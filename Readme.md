# UID1 Project for Pilot Disc

The UID1 project is a Keil microvision project for Pilot Disc.
It is based on the processor STM32F031G4 processor with 4KB RAM and 16KB Flash.

## Keil Microvision Project Settings

There are two targets in the project. They are
###	a. Debug 
	The Debug target is located at 0x0800_0000 in flash and can be used to debug the target using ST-LinkV2 debugger.
###	b. Release
	The Release target is located at 0x0800_1000 in flash and can only be used with the bootloader.

It is to be noted that both targets are having identical configurations and settings for build.

## Software Component settings are:
	a. CMSIS
		CORE Version: 5.0.2
	b. Device
		Startup Version: 2.2.3

### The Compiler details are:
	a. Code Generation (ARM Compiler) Version: V5.06 update 5 (build 528)
	b. No MicroLIB
	c. Cross Module Optimization disabled.

### Linker details are:
	a. R/O Base: 0x08000000
	b. R/W Base: 0x20000000

### Compilation Arguements:
	--c99 -c --cpu Cortex-M0 -D__EVAL -g -O0 --apcs=interwork --split_sections 
	-I./RTE/_Target_1
	-IC:/Keil_v5/ARM/PACK/ARM/CMSIS/5.2.0/CMSIS/Include
	-IC:/Keil_v5/ARM/PACK/Keil/STM32F0xx_DFP/1.5.0/Device/Include
	-D__UVISION_VERSION="524"
	-D_RTE_
	-DSTM32F031x6
	-DUSE_STDPERIPH_DRIVER
	-D__CC_ARM
	-o .\Objects\*.o --omf_browse .\Objects\*.crf --depend .\Objects\*.d 

### Assembler Arguements:
	--cpu Cortex-M0 --pd "__EVAL SETA 1" -g --apcs=interwork 
	-I .\RTE\_Target_1 
	-I C:\Keil_v5\ARM\PACK\ARM\CMSIS\5.2.0\CMSIS\Include 
	-I C:\Keil_v5\ARM\PACK\Keil\STM32F0xx_DFP\1.5.0\Device\Include 
	--pd "__UVISION_VERSION SETA 524" --pd "_RTE_ SETA 1" --pd "STM32F031x6 SETA 1" --list ".\Listings\*.lst" --xref -o "*.o" --depend "*.d" 
	-I.\RTE\_Target_1

### Linker control string:
	--cpu Cortex-M0 *.o 
	--ro-base 0x08000000 --entry 0x08000000 --rw-base 0x20000000 --entry Reset_Handler --first __Vectors --strict --summary_stderr --info summarysizes --map --xref --callgraph --symbols 
	--info sizes --info totals --info unused --info veneers 
	 --list ".\Listings\UID1.map" 
	-o .\Objects\UID1.axf 

