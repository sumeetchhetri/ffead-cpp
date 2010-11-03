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
	rm -Rf ffead-server
	mkdir ffead-server
	mkdir ffead-server/resources
	mkdir ffead-server/lib
	mkdir ffead-server/public
	mkdir ffead-server/include
	mkdir ffead-server/rtdcf
	mkdir ffead-server/web
	mkdir ffead-server/web/default/public
	cp -Rf ../resources/* ffead-server/resources/
	cp -Rf ../lib/* ffead-server/lib/
	cp -Rf ../public/* ffead-server/public/
	cp -Rf ../include/* ffead-server/include/
	cp -Rf ../rtdcf/* ffead-server/rtdcf/
	cp -Rf ../web/* ffead-server/web
	cp -Rf ../public/* ffead-server/web/default/public/
	cp -Rf ../script/* ffead-server/
	cp -Rf CHS ffead-server/
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) CHS
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
