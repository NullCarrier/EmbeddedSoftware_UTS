################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Sources/Analog.cpp \
../Sources/FIFO.cpp \
../Sources/FixPoint32.cpp \
../Sources/Flash.cpp \
../Sources/IDMT.cpp \
../Sources/LEDs.cpp \
../Sources/PIT.cpp \
../Sources/Packet.cpp \
../Sources/UART.cpp \
../Sources/main.cpp 

OBJS += \
./Sources/Analog.o \
./Sources/FIFO.o \
./Sources/FixPoint32.o \
./Sources/Flash.o \
./Sources/IDMT.o \
./Sources/LEDs.o \
./Sources/PIT.o \
./Sources/Packet.o \
./Sources/UART.o \
./Sources/main.o 

CPP_DEPS += \
./Sources/Analog.d \
./Sources/FIFO.d \
./Sources/FixPoint32.d \
./Sources/Flash.d \
./Sources/IDMT.d \
./Sources/LEDs.d \
./Sources/PIT.d \
./Sources/Packet.d \
./Sources/UART.d \
./Sources/main.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"C:/Users/12199434/My Local Data/08/2019_Autumn_Project-master/Static_Code/IO_Map" -I"C:/Users/12199434/My Local Data/08/2019_Autumn_Project-master/Sources" -I"C:/Users/12199434/My Local Data/08/2019_Autumn_Project-master/Generated_Code" -I"C:\Users\12199434\My Local Data\08\2019_Autumn_Project-master\Library" -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


