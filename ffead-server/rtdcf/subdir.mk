# Add inputs and outputs from these tool invocations to the build variables 

POBJS += \
./AjaxInterface.o \
./ReflectorInterface.o \
./SerializeInterface.o \



PDOBJS += \



# Each subdirectory must supply rules for building sources it contributes
./%.o: ./%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(PTHREADS) -I/cygdrive/c/Users/sumeetc/workspace-cpp/ffead-cpp-svn-latest-src/ffead-server/web/.svn/include/ -I/cygdrive/c/Users/sumeetc/workspace-cpp/ffead-cpp-svn-latest-src/ffead-server/web/default/include/ -I/cygdrive/c/Users/sumeetc/workspace-cpp/ffead-cpp-svn-latest-src/ffead-server/web/flexApp/include/ -I/cygdrive/c/Users/sumeetc/workspace-cpp/ffead-cpp-svn-latest-src/ffead-server/web/oauthApp/include/  -I/usr/local/include -I../include -O0 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

