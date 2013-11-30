################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CIB_CPP_SRCS += \
../src/modules/cibernate/Cibernate.cpp \
../src/modules/cibernate/CibernateConnPools.cpp \
../src/modules/cibernate/CibernateConnectionPool.cpp \
../src/modules/cibernate/CibernateQuery.cpp \
../src/modules/cibernate/DialectHelper.cpp


CIB_OBJS += \
./Cibernate.o \
./CibernateConnPools.o \
./CibernateConnectionPool.o \
./CibernateQuery.o \
./DialectHelper.o



CIB_CPP_DEPS += \
./Cibernate.d \
./CibernateConnPools.d \
./CibernateConnectionPool.d \
./CibernateQuery.d \
./DialectHelper.o



# Each subdirectory must supply rules for building sources it contributes
%.o: ../src/modules/cibernate/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(PTHREADS) -l ../src/ -I../include -I/usr/local/include -O0 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


