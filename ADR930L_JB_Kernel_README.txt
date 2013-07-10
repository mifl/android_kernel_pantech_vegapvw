1. How to build Kernel source of ADR930L_JB

    - Firstly Get Toolchain from android git server and etc …
        GCC correct version is arm-eabi-4.6
    - modify build_kernel shell script.
        edit "CROSS_COMPILE" to toolchain path ( You downloaded )
        ex) CROSS_COMPILE=$(your download directory)/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-
    - run build_kernel.sh
        $./build_kernel.sh
         

2.	Output File Location
    - kernel : ./obj/KERNEL_OBJ/arch/arm/boot/zImage
    - module : ./obj/KERNEL_OBJ/drivers/*/*.ko
      
3.	How to Clean Kernel object files
    - run clean_kernel.sh
        $./clean_kernel.sh
