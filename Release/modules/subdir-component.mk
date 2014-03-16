################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
COMP_CPP_SRCS += \
../src/modules/component/Component.cpp \
../src/modules/component/ComponentHandler.cpp \
../src/modules/component/RemoteComponent.cpp \
../src/modules/component/ComponentGen.cpp \
../src/modules/component/BeanContext.cpp \
../src/modules/component/ServiceInt.cpp \
../src/modules/component/AppContext.cpp

COMP_OBJS += \
./Component.o \
./ComponentHandler.o \
./RemoteComponent.o \
./ComponentGen.o \
./BeanContext.o \
./ServiceInt.o \
./AppContext.o


COMP_CPP_DEPS += \
./Component.d \
./ComponentHandler.d \
./RemoteComponent.d \
./ComponentGen.d \
./BeanContext.d \
./ServiceInt.d \
./AppContext.d


# Each subdirectory must supply rules for building sources it contributes
%.o: ../src/modules/component/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(PTHREADS) -l ../src/ -I../include -I/usr/local/include -O0 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
	
