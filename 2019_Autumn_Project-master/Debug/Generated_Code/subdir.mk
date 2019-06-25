################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Generated_Code/Vectors.cpp 

C_SRCS += \
../Generated_Code/Cpu.c 

OBJS += \
./Generated_Code/Cpu.o \
./Generated_Code/Vectors.o 

C_DEPS += \
./Generated_Code/Cpu.d 

CPP_DEPS += \
./Generated_Code/Vectors.d 


# Each subdirectory must supply rules for building sources it contributes
Generated_Code/%.o: ../Generated_Code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"C:\Users\12199434\My Local Data\08-b4bb57118f8777b56d792d1026c605e44fbec244\2019_Autumn_Project-master\Library" -I"C:/Users/12199434/My Local Data/08-b4bb57118f8777b56d792d1026c605e44fbec244/2019_Autumn_Project-master/Static_Code/IO_Map" -I"C:/Users/12199434/My Local Data/08-b4bb57118f8777b56d792d1026c605e44fbec244/2019_Autumn_Project-master/Sources" -I"C:/Users/12199434/My Local Data/08-b4bb57118f8777b56d792d1026c605e44fbec244/2019_Autumn_Project-master/Generated_Code" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/%.o: ../Generated_Code/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"C:/Users/12199434/My Local Data/08-b4bb57118f8777b56d792d1026c605e44fbec244/2019_Autumn_Project-master/Static_Code/IO_Map" -I"C:/Users/12199434/My Local Data/08-b4bb57118f8777b56d792d1026c605e44fbec244/2019_Autumn_Project-master/Sources" -I"C:/Users/12199434/My Local Data/08-b4bb57118f8777b56d792d1026c605e44fbec244/2019_Autumn_Project-master/Generated_Code" -I"C:\Users\12199434\My Local Data\08-b4bb57118f8777b56d792d1026c605e44fbec244\2019_Autumn_Project-master\Library" -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


