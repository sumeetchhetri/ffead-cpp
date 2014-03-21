################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../DefTemp.cpp \
../DefaultController.cpp \
../Dview.cpp \
../Expose.cpp \
../Service1.cpp \
../Service2.cpp \
../Tempo.cpp \
../Test.cpp \
../Test2.cpp \
../Test3.cpp \
../Test4.cpp \
../TestMany.cpp \
../Testing.cpp \
../DefaultOAUTHController.cpp \
../DefaultIOFilter.cpp \
../TestPage.cpp \
../TestForm.cpp \
../DefaultRestController.cpp \
../TestFormController.cpp \
../TestingWS.cpp \
../TestObject.cpp \
../TestObject1.cpp \
../TestCronBasedJob.cpp

OBJS += \
./DefTemp.o \
./DefaultController.o \
./Dview.o \
./Expose.o \
./Service1.o \
./Service2.o \
./Tempo.o \
./Test.o \
./Test2.o \
./Test3.o \
./Test4.o \
./TestMany.o \
./Testing.o \
./DefaultOAUTHController.o \
./DefaultIOFilter.o \
./TestPage.o \
./TestForm.o \
./DefaultRestController.o \
./TestFormController.o \
./TestingWS.o \
./TestObject.o \
./TestObject1.o \
./TestCronBasedJob.o

CPP_DEPS += \
./DefTemp.d \
./DefaultController.d \
./Dview.d \
./Expose.d \
./Service1.d \
./Service2.d \
./Tempo.d \
./Test.d \
./Test2.d \
./Test3.d \
./Test4.d \
./TestMany.d \
./Testing.d \
./DefaultOAUTHController.d \
./DefaultIOFilter.d \
./TestPage.d \
./TestForm.d \
./DefaultRestController.d \
./TestFormController.d \
./TestingWS.d \
./TestObject.d \
./TestObject1.d \
./TestCronBasedJob.d


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(PTHREADS) -I"../../../../include" -I"../../include" -I"/usr/local/include" -O0 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


