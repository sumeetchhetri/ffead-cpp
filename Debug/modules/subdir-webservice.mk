################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
WEBSVCS_CPP_SRCS += \
../src/modules/webservice/SoapHandler.cpp \
../src/modules/webservice/SOAPRequest.cpp \
../src/modules/webservice/SOAPResponse.cpp \
../src/modules/webservice/SOAPUtil.cpp \
../src/modules/webservice/WsUtil.cpp



WEBSVCS_OBJS += \
./SoapHandler.o \
./SOAPRequest.o \
./SOAPResponse.o \
./SOAPUtil.o \
./WsUtil.o



WEBSVCS_CPP_DEPS += \
./SoapHandler.d \
./SOAPRequest.d \
./SOAPResponse.d \
./SOAPUtil.d \
./WsUtil.d


# Each subdirectory must supply rules for building sources it contributes
%.o: ../src/modules/webservice/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(PTHREADS) -l ../src/ -I../include -I/usr/local/include -O0 -Wall -g3 -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


