RM := rm -rf

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
all: libinter libdinter

# Tool invocations
libinter: $(POBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ $(PTHREADS) -L"/usr/local/lib" -L"../lib" -shared -o"libinter.$(RES_LIB_EXT)" $(POBJS) $(USER_OBJS) $(LIBS) $(EX_LIBS)
	@echo 'Finished building target: $@'
	cp -f libinter.$(RES_LIB_EXT) ../lib/
	@echo ' '
	
libdinter: $(PDOBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ $(PTHREADS) -L"/usr/local/lib" -L"../lib" -shared -o"libdinter.$(RES_LIB_EXT)" $(PDOBJS) $(LIBS) $(EX_LIBS)
	@echo 'Finished building target: $@'
	cp -f libdinter.$(RES_LIB_EXT) ../lib/
	@echo ' '

# Other Targets
clean:
	-$(RM) *.o $(C++_DEPS) $(C_DEPS) $(CC_DEPS) *.d $(EXECUTABLES) $(CXX_DEPS) $(C_UPPER_DEPS) libinter.$(RES_LIB_EXT) libdinter.$(RES_LIB_EXT)
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
libinter:
