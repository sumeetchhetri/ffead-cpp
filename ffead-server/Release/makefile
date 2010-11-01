################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
-include objects.mk

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
all: CHS

# Tool invocations
CHS: $(OBJS) $(USER_OBJS)
	make -f makefilel all
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -L../lib -rdynamic -o"CHS" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'	
	rm -Rf server
	mkdir server
	mkdir server/resources
	mkdir server/lib
	mkdir server/public
	mkdir server/include
	mkdir server/rtdcf
	mkdir server/web
	cp -Rf ../resources/* server/resources/
	cp -Rf ../lib/* server/lib/
	cp -Rf ../public/* server/public/
	cp -Rf ../include/* server/include/
	cp -Rf ../rtdcf/* server/rtdcf/
	cp -Rf ../web/* server/web
	cp -Rf ../script/* server/
	cp -Rf CHS server/
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) CHS
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
