################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf
modules := all

# All of the sources participating in the build are defined here
-include subdir.mk
-include objects.mk
-include envvars.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: init CHS

init:
	chmod 755 chkIncludes.sh
	sh chkIncludes.sh

# Tool invocations
CHS: init $(OBJS) $(USER_OBJS)
	$(MAKE_EXE) modules=$(modules) -f makefilel all
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ $(PTHREADS) -L../lib -L/usr/local/lib -rdynamic -o"CHS" $(OBJS) $(USER_OBJS) $(LIBS) $(EX_LIBS)
	@echo 'Finished building target: $@'	
	rm -Rf ffead-server
	mkdir ffead-server
	mkdir ffead-server/resources
	mkdir ffead-server/lib
	mkdir ffead-server/public
	mkdir ffead-server/include
	mkdir ffead-server/rtdcf
	mkdir ffead-server/web
	mkdir ffead-server/tmp
	mkdir ffead-server/logs
	cp -Rf ../resources/* ffead-server/resources/
	cp -Rf ../lib/* ffead-server/lib/
	cp -Rf ../public/* ffead-server/public/
	cp -Rf ../include/* ffead-server/include/
	cp -Rf ../rtdcf/* ffead-server/rtdcf/
	cp -Rf ../web/* ffead-server/web
	cp -Rf ../script/* ffead-server/
	cp -Rf CHS ffead-server/
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS) $(C++_DEPS) $(C_DEPS) $(CC_DEPS) $(CPP_DEPS) $(EXECUTABLES) $(CXX_DEPS) $(C_UPPER_DEPS) CHS
	$(MAKE_EXE) -f makefilel clean
	rm -rf ffead-server
	-@echo ' '
	
build-apps:
	chmod 755 buildApps.sh
	sh buildApps.sh
	
clean-apps:
	chmod 755 cleanApps.sh
	sh cleanApps.sh

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
