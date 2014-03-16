################################################################################
# Automatically-generated file. Do not edit!
################################################################################

DEFCOMP :=
DEFMI :=
DEFMSGH :=
DEFWEBSVC :=
DEFCIB :=
DEFDSTC :=
DEFBINSER :=
DEFXMLSER :=
DEFDCP :=
DEFDVIEW :=
DEFTPE :=
DEFINTP :=
DEFAPPF :=
DEFJOBS :=
DEFSCRH :=
modules := all

ifneq (,$(findstring component,$(modules)))
DEFCOMP := -DINC_COMP
DEFXMLSER := -DINC_XMLSER
DEFCIB := -DINC_CIB
endif
ifneq (,$(findstring methinvoker,$(modules)))
DEFMI := -DINC_MI
DEFXMLSER := -DINC_XMLSER
endif
ifneq (,$(findstring msghandler,$(modules)))
DEFMSGH := -DINC_MSGH
DEFBINSER := -DINC_BINSER
endif
ifneq (,$(findstring webservice,$(modules)))
DEFWEBSVC := -DINC_WEBSVC
endif
ifneq (,$(findstring cibernate,$(modules)))
DEFCIB := -DINC_CIB
endif
ifneq (,$(findstring distocache,$(modules)))
DEFDIST := -DINC_DSTC
DEFBINSER := -DINC_BINSER
endif
ifneq (,$(findstring xmlserialize,$(modules)))
DEFXMLSER := -DINC_XMLSER
endif
ifneq (,$(findstring binserialize,$(modules)))
DEFBINSER := -DINC_BINSER
endif
ifneq (,$(findstring dcp,$(modules)))
DEFDCP := -DINC_DCP
endif
ifneq (,$(findstring dview,$(modules)))
DEFDVIEW := -DINC_DVIEW
endif
ifneq (,$(findstring tpe,$(modules)))
DEFTPE := -DINC_TPE
endif
ifneq (,$(findstring interpreter,$(modules)))
DEFINTP := -DINC_INTP
endif
ifneq (,$(findstring appflow,$(modules)))
DEFAPPF := -DINC_APPFLOW
endif
ifneq (,$(findstring jobs,$(modules)))
DEFJOBS := -DINC_JOBS
endif
ifneq (,$(findstring scripthandler,$(modules)))
DEFSCRH := -DINC_SCRH
endif
ifneq (,$(findstring all,$(modules)))
DEFCOMP := -DINC_COMP
DEFMI := -DINC_MI
DEFMSGH := -DINC_MSGH
DEFWEBSVC := -DINC_WEBSVC
DEFCIB := -DINC_CIB
DEFDSTC := -DINC_DSTC
DEFXMLSER := -DINC_XMLSER
DEFBINSER := -DINC_BINSER
DEFDCP := -DINC_DCP
DEFDVIEW := -DINC_DVIEW
DEFINTP := -DINC_INTP
DEFAPPF := -DINC_APPFLOW
DEFJOBS := -DINC_JOBS
DEFSCRH := -DINC_SCRH
DEFTPE := -DINC_TPE
endif

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CHServer.cpp	

OBJS += \
./CHServer.o	
	
CPP_DEPS += \
./CHServer.d	
	
# Each subdirectory must supply rules for building sources it contributes
%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(PTHREADS) -I../include -I/usr/local/include -O0 -g3 \
		$(DEFCOMP) $(DEFMI) $(DEFMSGH) $(DEFWEBSVC) $(DEFDSTC) $(DEFCIB) $(DEFBINSER) $(DEFXMLSER) \
		$(DEFDCP) $(DEFDVIEW) $(DEFTPE) $(DEFINTP) $(DEFAPPF) $(DEFJOBS) $(DEFSCRH) -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
