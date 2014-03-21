################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
MSGH_CPP_SRCS += \
../src/modules/msghandler/InvalidMessageException.cpp \
../src/modules/msghandler/Message.cpp \
../src/modules/msghandler/MessageHandler.cpp \
../src/modules/msghandler/MessageUtil.cpp \
../src/modules/msghandler/Destination.cpp


MSGH_OBJS += \
./InvalidMessageException.o \
./Message.o \
./MessageHandler.o \
./MessageUtil.o \
./Destination.o



MSGH_CPP_DEPS += \
./InvalidMessageException.d \
./Message.d \
./MessageHandler.d \
./MessageUtil.d \
./Destination.d


# Each subdirectory must supply rules for building sources it contributes
%.o: ../src/modules/msghandler/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(PTHREADS) -l ../src/ -I../include -I/usr/local/include -O0 -Wall -g3 -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


