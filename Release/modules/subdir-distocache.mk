################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
DSTC_CPP_SRCS += \
../src/modules/distocache/DistoCacheHandler.cpp \
../src/modules/distocache/DistoCacheServiceHandler.cpp \
../src/modules/distocache/CacheMap.cpp \
../src/modules/distocache/DistoCacheClientUtils.cpp \
../src/modules/distocache/BasicDistoCacheConnectionFactory.cpp \
../src/modules/distocache/PooledDistoCacheConnectionFactory.cpp


DSTC_OBJS += \
./DistoCacheHandler.o \
./DistoCacheServiceHandler.o \
./CacheMap.o \
./DistoCacheClientUtils.o \
./BasicDistoCacheConnectionFactory.o \
./PooledDistoCacheConnectionFactory.o


DSTC_CPP_DEPS += \
./DistoCacheHandler.d \
./DistoCacheServiceHandler.d \
./CacheMap.d \
./DistoCacheClientUtils.d \
./BasicDistoCacheConnectionFactory.d \
./PooledDistoCacheConnectionFactory.d


# Each subdirectory must supply rules for building sources it contributes
%.o: ../src/modules/distocache/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(PTHREADS) -l ../src/ -I../include -I/usr/local/include -O0 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


