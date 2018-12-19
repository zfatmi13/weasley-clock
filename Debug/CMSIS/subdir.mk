################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/system_MKL43Z4.c 

OBJS += \
./CMSIS/system_MKL43Z4.o 

C_DEPS += \
./CMSIS/system_MKL43Z4.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/%.o: ../CMSIS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DPRINTF_FLOAT_ENABLE=0 -D__USE_CMSIS -DCR_INTEGER_PRINTF -DSDK_DEBUGCONSOLE=0 -D__MCUXPRESSO -DDEBUG -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DCPU_MKL43Z256VLH4_cm0plus -DCPU_MKL43Z256VLH4 -I"C:\Users\zaina\Documents\MCUXpressoIDE_10.1.1_606\workspace\Project_Complete\board" -I"C:\Users\zaina\Documents\MCUXpressoIDE_10.1.1_606\workspace\Project_Complete\source" -I"C:\Users\zaina\Documents\MCUXpressoIDE_10.1.1_606\workspace\Project_Complete" -I"C:\Users\zaina\Documents\MCUXpressoIDE_10.1.1_606\workspace\Project_Complete\drivers" -I"C:\Users\zaina\Documents\MCUXpressoIDE_10.1.1_606\workspace\Project_Complete\CMSIS" -I"C:\Users\zaina\Documents\MCUXpressoIDE_10.1.1_606\workspace\Project_Complete\utilities" -I"C:\Users\zaina\Documents\MCUXpressoIDE_10.1.1_606\workspace\Project_Complete\startup" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


