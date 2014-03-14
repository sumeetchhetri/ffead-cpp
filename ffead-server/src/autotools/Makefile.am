
AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
AM_CPPFLAGS = -I"../../include"

prefix=${abs_builddir}

lib_LTLIBRARIES = libweblib.la
libweblib_la_SOURCES = ../ServiceTask.cpp \
		../SSLHandler.cpp \
		../ConfigurationHandler.cpp \
		../ConfigurationData.cpp \
		../ExtHandler.cpp \
		../FviewHandler.cpp \
		../FormHandler.cpp \
		../ControllerHandler.cpp \
		../AuthHandler.cpp \
		../FilterHandler.cpp \
		../SecurityHandler.cpp \
		../AOPEngine.cpp \
		../AfcUtil.cpp \
		../Aspect.cpp \
		../BinaryData.cpp \
		../Block.cpp \
		../Cache.cpp \
		../CfusionEngine.cpp \
		../Class.cpp \
		../ClassInfo.cpp \
		../Client.cpp \
		../ConnectionSettings.cpp \
		../Constructor.cpp \
		../Date.cpp \
		../DateFormat.cpp \
		../Document.cpp \
		../Element.cpp \
		../Exception.cpp \
		../FFEADContext.cpp \
		../Field.cpp \
		../HttpRequest.cpp \
		../HttpResponse.cpp \
		../HttpSession.cpp \
		../Logger.cpp \
		../Method.cpp \
		../PropFileReader.cpp \
		../MQueue.cpp \
		../Reflection.cpp \
		../Reflector.cpp \
		../Renderer.cpp \
		../SerializeBase.cpp \
		../Server.cpp \
		../Service.cpp \
		../ServicePool.cpp \
		../Task.cpp \
		../TaskPool.cpp \
		../TemplateEngine.cpp \
		../PoolThread.cpp \
		../ThreadPool.cpp \
		../Timer.cpp \
		../View.cpp \
		../XmlParseException.cpp \
		../XmlParser.cpp \
		../AuthController.cpp \
		../FileAuthController.cpp \
		../DBAuthController.cpp \
		../LDAPAuthController.cpp \
		../CryptoHandler.cpp \
		../HttpResponseParser.cpp \
		../SSLClient.cpp \
		../RestController.cpp \
		../Filter.cpp \
		../StringUtil.cpp \
		../JSONElement.cpp \
		../JSONUtil.cpp \
		../Thread.cpp \
		../Mutex.cpp \
		../RegexUtil.cpp \
		../SelEpolKqEvPrt.cpp \
		../ClientInterface.cpp \
		../HTTPResponseStatus.cpp \
		../ContentTypes.cpp \
		../JSONSerialize.cpp \
		../CsvFileReader.cpp \
		../Bigint.cpp \
		../Bigdecimal.cpp \
		../FutureTask.cpp \
		../CORSHandler.cpp \
		../CompressionUtil.cpp \
		../MultipartContent.cpp \
		../LoggerFactory.cpp \
		../NBServer.cpp \
		../ScriptHandler.cpp \
		../SSLCommon.cpp

if MOD_ALL
libweblib_la_SOURCES += ../modules/component/Component.cpp \
						../modules/component/ComponentHandler.cpp \
						../modules/component/RemoteComponent.cpp \
						../modules/component/ComponentGen.cpp \
						../modules/component/BeanContext.cpp \
						../modules/component/ServiceInt.cpp \
						../modules/component/AppContext.cpp \
						../modules/xmlserialize/XMLSerialize.cpp \
						../modules/cibernate/Cibernate.cpp \
						../modules/cibernate/CibernateConnPools.cpp \
						../modules/cibernate/CibernateConnectionPool.cpp \
						../modules/cibernate/CibernateQuery.cpp \
						../modules/cibernate/DialectHelper.cpp \
						../modules/methinvoker/MethodInvoc.cpp \
						../modules/msghandler/InvalidMessageException.cpp \
						../modules/msghandler/Message.cpp \
						../modules/msghandler/MessageHandler.cpp \
						../modules/msghandler/MessageUtil.cpp \
						../modules/msghandler/Destination.cpp \
						../modules/binserialize/BinarySerialize.cpp \
						../modules/binserialize/AMEFResources.cpp \
						../modules/binserialize/AMEFEncoder.cpp \
						../modules/binserialize/AMEFDecoder.cpp \
						../modules/binserialize/AMEFObject.cpp \
						../modules/webservice/SoapHandler.cpp \
						../modules/webservice/SOAPRequest.cpp \
						../modules/webservice/SOAPResponse.cpp \
						../modules/webservice/SOAPUtil.cpp \
						../modules/webservice/WsUtil.cpp \
						../modules/distocache/DistoCacheHandler.cpp \
						../modules/distocache/DistoCacheServiceHandler.cpp \
						../modules/distocache/CacheMap.cpp \
						../modules/distocache/DistoCacheClientUtils.cpp \
						../modules/distocache/BasicDistoCacheConnectionFactory.cpp \
						../modules/distocache/PooledDistoCacheConnectionFactory.cpp \
						../modules/dcp/DCPGenerator.cpp \
						../modules/dview/DynamicView.cpp \
						../modules/tpe/TemplateHandler.cpp \
						../modules/tpe/TemplateGenerator.cpp \
						../modules/interpreter/CppInterpreter.cpp \
						../modules/appflow/ApplicationUtil.cpp \
						../modules/jobs/JobScheduler.cpp \
						../modules/jobs/CronTimer.cpp
else
	if MOD_COMPONENT
	libweblib_la_SOURCES += ../modules/component/Component.cpp \
							../modules/component/ComponentHandler.cpp \
							../modules/component/RemoteComponent.cpp \
							../modules/component/ComponentGen.cpp \
							../modules/component/BeanContext.cpp \
							../modules/component/ServiceInt.cpp \
							../modules/component/AppContext.cpp \
							../modules/xmlserialize/XMLSerialize.cpp \
							../modules/cibernate/Cibernate.cpp \
							../modules/cibernate/CibernateConnPools.cpp \
							../modules/cibernate/CibernateConnectionPool.cpp \
							../modules/cibernate/CibernateQuery.cpp \
							../modules/cibernate/DialectHelper.cpp
	endif
	if MOD_METHINVOKER
	libweblib_la_SOURCES += ../modules/xmlserialize/XMLSerialize.cpp \
							../modules/methinvoker/MethodInvoc.cpp
	endif
	if MOD_MSGHANDLER
	libweblib_la_SOURCES += ../modules/msghandler/InvalidMessageException.cpp \
							../modules/msghandler/Message.cpp \
							../modules/msghandler/MessageHandler.cpp \
							../modules/msghandler/MessageUtil.cpp \
							../modules/msghandler/Destination.cpp \
							../modules/binserialize/BinarySerialize.cpp \
							../modules/binserialize/AMEFResources.cpp \
							../modules/binserialize/AMEFEncoder.cpp \
							../modules/binserialize/AMEFDecoder.cpp \
							../modules/binserialize/AMEFObject.cpp
	endif
	if MOD_WEBSERVICE
	libweblib_la_SOURCES += ../modules/webservice/SoapHandler.cpp \
							../modules/webservice/SOAPRequest.cpp \
							../modules/webservice/SOAPResponse.cpp \
							../modules/webservice/SOAPUtil.cpp \
							../modules/webservice/WsUtil.cpp
	endif
	if MOD_CIBERNATE
	libweblib_la_SOURCES += ../modules/cibernate/Cibernate.cpp \
							../modules/cibernate/CibernateConnPools.cpp \
							../modules/cibernate/CibernateConnectionPool.cpp \
							../modules/cibernate/CibernateQuery.cpp \
							../modules/cibernate/DialectHelper.cpp
	endif
	if MOD_DISTOCACHE
	libweblib_la_SOURCES += ../modules/binserialize/BinarySerialize.cpp \
							../modules/binserialize/AMEFResources.cpp \
							../modules/binserialize/AMEFEncoder.cpp \
							../modules/binserialize/AMEFDecoder.cpp \
							../modules/binserialize/AMEFObject.cpp \
							../modules/distocache/DistoCacheHandler.cpp \
							../modules/distocache/DistoCacheServiceHandler.cpp \
							../modules/distocache/CacheMap.cpp \
							../modules/distocache/DistoCacheClientUtils.cpp \
							../modules/distocache/BasicDistoCacheConnectionFactory.cpp \
							../modules/distocache/PooledDistoCacheConnectionFactory.cpp
	endif
	if MOD_XMLSERIALIZE
	libweblib_la_SOURCES += ../modules/xmlserialize/XMLSerialize.cpp
	endif
	if MOD_BINSERIALIZE
	libweblib_la_SOURCES += ../modules/binserialize/BinarySerialize.cpp \
							../modules/binserialize/AMEFResources.cpp \
							../modules/binserialize/AMEFEncoder.cpp \
							../modules/binserialize/AMEFDecoder.cpp \
							../modules/binserialize/AMEFObject.cpp
	endif
	if MOD_DCP
	libweblib_la_SOURCES += ../modules/dcp/DCPGenerator.cpp
	endif
	if MOD_DVIEW
	libweblib_la_SOURCES += ../modules/dview/DynamicView.cpp
	endif
	if MOD_TPE
	libweblib_la_SOURCES += ../modules/tpe/TemplateHandler.cpp \
							../modules/tpe/TemplateGenerator.cpp
	endif
	if MOD_INTERPRETER
	libweblib_la_SOURCES += ../modules/interpreter/CppInterpreter.cpp
	endif
	if MOD_APPFLOW
	libweblib_la_SOURCES += ../modules/appflow/ApplicationUtil.cpp
	endif
	if MOD_JOBS
	libweblib_la_SOURCES += ../modules/jobs/JobScheduler.cpp \
							../modules/jobs/CronTimer.cpp
	endif
endif


libweblib_la_LDFLAGS = -shared -no-undefined -module

noinst_PROGRAMS = CHS
CHS_SOURCES = ../CHServer.cpp
CHS_LDADD = -L".libs/" -lweblib

