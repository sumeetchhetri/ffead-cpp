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
../src/ServiceTask.cpp \
../src/SSLHandler.cpp \
../src/ConfigurationHandler.cpp \
../src/ConfigurationData.cpp \
../src/ExtHandler.cpp \
../src/FviewHandler.cpp \
../src/FormHandler.cpp \
../src/ControllerHandler.cpp \
../src/AuthHandler.cpp \
../src/FilterHandler.cpp \
../src/SecurityHandler.cpp \
../src/AOPEngine.cpp \
../src/AfcUtil.cpp \
../src/Aspect.cpp \
../src/BinaryData.cpp \
../src/Block.cpp \
../src/Cache.cpp \
../src/CfusionEngine.cpp \
../src/Class.cpp \
../src/ClassInfo.cpp \
../src/Client.cpp \
../src/ConnectionSettings.cpp \
../src/Constructor.cpp \
../src/Date.cpp \
../src/DateFormat.cpp \
../src/Document.cpp \
../src/Element.cpp \
../src/Exception.cpp \
../src/FFEADContext.cpp \
../src/Field.cpp \
../src/HttpRequest.cpp \
../src/HttpResponse.cpp \
../src/HttpSession.cpp \
../src/Logger.cpp \
../src/Method.cpp \
../src/PropFileReader.cpp \
../src/MQueue.cpp \
../src/Reflection.cpp \
../src/Reflector.cpp \
../src/Renderer.cpp \
../src/SerializeBase.cpp \
../src/Server.cpp \
../src/Service.cpp \
../src/ServicePool.cpp \
../src/Task.cpp \
../src/TaskPool.cpp \
../src/TemplateEngine.cpp \
../src/PoolThread.cpp \
../src/ThreadPool.cpp \
../src/Timer.cpp \
../src/View.cpp \
../src/XmlParseException.cpp \
../src/XmlParser.cpp \
../src/AuthController.cpp \
../src/FileAuthController.cpp \
../src/DBAuthController.cpp \
../src/LDAPAuthController.cpp \
../src/CryptoHandler.cpp \
../src/HttpResponseParser.cpp \
../src/SSLClient.cpp \
../src/RestController.cpp \
../src/Filter.cpp \
../src/StringUtil.cpp \
../src/JSONElement.cpp \
../src/JSONUtil.cpp \
../src/Thread.cpp \
../src/Mutex.cpp \
../src/RegexUtil.cpp \
../src/SelEpolKqEvPrt.cpp \
../src/ClientInterface.cpp \
../src/HTTPResponseStatus.cpp \
../src/ContentTypes.cpp \
../src/JSONSerialize.cpp \
../src/CsvFileReader.cpp \
../src/Bigint.cpp \
../src/Bigdecimal.cpp \
../src/FutureTask.cpp \
../src/CORSHandler.cpp \
../src/CompressionUtil.cpp \
../src/MultipartContent.cpp \
../src/LoggerFactory.cpp \
../src/NBServer.cpp \
../src/ScriptHandler.cpp \
../src/SSLCommon.cpp



OBJS += \
./ServiceTask.o \
./SSLHandler.o \
./ConfigurationHandler.o \
./ConfigurationData.o \
./ExtHandler.o \
./FviewHandler.o \
./FormHandler.o \
./ControllerHandler.o \
./AuthHandler.o \
./FilterHandler.o \
./SecurityHandler.o \
./AOPEngine.o \
./AfcUtil.o \
./Aspect.o \
./BinaryData.o \
./Block.o \
./Cache.o \
./CfusionEngine.o \
./Class.o \
./ClassInfo.o \
./Client.o \
./ConnectionSettings.o \
./Constructor.o \
./Date.o \
./DateFormat.o \
./Document.o \
./Element.o \
./Exception.o \
./FFEADContext.o \
./Field.o \
./HttpRequest.o \
./HttpResponse.o \
./HttpSession.o \
./Logger.o \
./Method.o \
./PropFileReader.o \
./MQueue.o \
./Reflection.o \
./Reflector.o \
./Renderer.o \
./SerializeBase.o \
./Server.o \
./Service.o \
./ServicePool.o \
./Task.o \
./TaskPool.o \
./TemplateEngine.o \
./PoolThread.o \
./ThreadPool.o \
./Timer.o \
./View.o \
./XmlParseException.o \
./XmlParser.o \
./AuthController.o \
./FileAuthController.o \
./DBAuthController.o \
./LDAPAuthController.o \
./CryptoHandler.o \
./HttpResponseParser.o \
./SSLClient.o \
./RestController.o \
./Filter.o \
./StringUtil.o \
./JSONElement.o \
./JSONUtil.o \
./Thread.o \
./Mutex.o \
./RegexUtil.o \
./SelEpolKqEvPrt.o \
./ClientInterface.o \
./HTTPResponseStatus.o \
./ContentTypes.o \
./JSONSerialize.o \
./CsvFileReader.o \
./Bigint.o \
./Bigdecimal.o \
./FutureTask.o \
./CORSHandler.o \
./CompressionUtil.o \
./MultipartContent.o \
./LoggerFactory.o \
./NBServer.o \
./ScriptHandler.o \
./SSLCommon.o



CPP_DEPS += \
./ServiceTask.d \
./SSLHandler.d \
./ConfigurationHandler.d \
./ConfigurationData.d \
./ExtHandler.d \
./FviewHandler.d \
./FormHandler.d \
./ControllerHandler.d \
./AuthHandler.d \
./FilterHandler.d \
./SecurityHandler.d \
./AOPEngine.d \
./AfcUtil.d \
./Aspect.d \
./BeanContext.d \
./BinaryData.d \
./Block.d \
./Cache.d \
./CfusionEngine.d \
./Class.d \
./ClassInfo.d \
./Client.d \
./ConnectionSettings.d \
./Constructor.d \
./Date.d \
./DateFormat.d \
./Document.d \
./Element.d \
./Exception.d \
./FFEADContext.d \
./Field.d \
./HttpRequest.d \
./HttpResponse.d \
./HttpSession.d \
./Logger.d \
./Method.d \
./PropFileReader.d \
./MQueue.d \
./Reflection.d \
./Reflector.d \
./Renderer.d \
./SerializeBase.d \
./Server.d \
./Service.d \
./ServicePool.d \
./Task.d \
./TaskPool.d \
./TemplateEngine.d \
./PoolThread.d \
./ThreadPool.d \
./Timer.d \
./View.d \
./XmlParseException.d \
./XmlParser.d \
./AuthController.d \
./FileAuthController.d \
./DBAuthController.d \
./LDAPAuthController.d \
./CryptoHandler.d \
./HttpResponseParser.d \
./SSLClient.d \
./RestController.d \
./Filter.d \
./StringUtil.d \
./JSONElement.d \
./JSONUtil.d \
./Thread.d \
./Mutex.d \
./RegexUtil.d \
./SelEpolKqEvPrt.d \
./ClientInterface.d \
./HTTPResponseStatus.d \
./ContentTypes.d \
./JSONSerialize.d \
./CsvFileReader.d \
./Bigint.d \
./Bigdecimal.d \
./FutureTask.d \
./CORSHandler.d \
./CompressionUtil.d \
./MultipartContent.d \
./LoggerFactory.d \
./NBServer.d \
./ScriptHandler.d \
./SSLCommon.d


# Each subdirectory must supply rules for building sources it contributes
%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(PTHREADS) -l ../src/ -I../include -I/usr/local/include -O0 -Wall -g3 \
		$(DEFCOMP) $(DEFMI) $(DEFMSGH) $(DEFWEBSVC) $(DEFDSTC) $(DEFCIB) $(DEFBINSER) $(DEFXMLSER) \
		$(DEFDCP) $(DEFDVIEW) $(DEFTPE) $(DEFINTP) $(DEFAPPF) $(DEFJOBS) $(DEFSCRH) -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


