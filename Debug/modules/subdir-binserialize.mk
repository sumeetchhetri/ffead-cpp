################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
BINSER_CPP_SRCS += \
../src/modules/binserialize/BinarySerialize.cpp \
../src/modules/binserialize/AMEFResources.cpp \
../src/modules/binserialize/AMEFEncoder.cpp \
../src/modules/binserialize/AMEFDecoder.cpp \
../src/modules/binserialize/AMEFObject.cpp

BINSER_OBJS += \
./BinarySerialize.o \
./AMEFResources.o \
./AMEFEncoder.o \
./AMEFDecoder.o \
./AMEFObject.o


BINSER_CPP_DEPS += \
./BinarySerialize.d \
./AMEFResources.d \
./AMEFEncoder.d \
./AMEFDecoder.d \
./AMEFObject.d


# Each subdirectory must supply rules for building sources it contributes
%.o: ../src/modules/binserialize/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(PTHREADS) -l ../src/ -I../include -I/usr/local/include -O0 -Wall -g3 -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


