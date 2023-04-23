
AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

AM_CPPFLAGS = -I"../../modules/common"

prefix=${abs_builddir}

lib_LTLIBRARIES = libffead-modules.la
libffead_modules_la_SOURCES =  ../../modules/common/Bigdecimal.cpp \
						../../modules/common/Bigint.cpp \
						../../modules/common/BinaryData.cpp \
						../../modules/common/CommonUtils.cpp \
						../../modules/common/Compatibility.cpp \
						../../modules/common/CompressionUtil.cpp \
						../../modules/common/Connection.cpp \
						../../modules/common/ConnectionPooler.cpp \
						../../modules/common/CryptoHandler.cpp \
						../../modules/common/SimpleCsvFileReader.cpp \
						../../modules/common/Date.cpp \
						../../modules/common/DateFormat.cpp \
						../../modules/common/DLogger.cpp \
						../../modules/common/Document.cpp \
						../../modules/common/Element.cpp \
						../../modules/common/Exception.cpp \
						../../modules/common/JSONElement.cpp \
						../../modules/common/JSONUtil.cpp \
						../../modules/common/Logger.cpp \
						../../modules/common/LoggerFactory.cpp \
						../../modules/common/PropFileReader.cpp \
						../../modules/common/RegexUtil.cpp \
						../../modules/common/Renderer.cpp \
						../../modules/common/StringUtil.cpp \
						../../modules/common/Timer.cpp \
						../../modules/common/View.cpp \
						../../modules/common/XmlParseException.cpp \
						../../modules/common/SimpleXmlParser.cpp \
						../../modules/common/MarkerHandler.cpp \
						../../modules/common/TemplateEngine.cpp \
						../../modules/common/CastUtil.cpp \
						../../modules/common/ReusableInstanceHolder.cpp \
						../../modules/common/picohttpparser_fcp.cpp \
						../../modules/common/yuarel.cpp \
						../../modules/common/hescape.cpp
if WITH_PICOEV
libffead_modules_la_SOURCES +=  ../../modules/common/picoev/picoev_epoll.cpp \
						../../modules/common/picoev/picoev_kqueue.cpp \
						../../modules/common/picoev/picoev_select.cpp
endif

if MOD_ALL
lib_LTLIBRARIES +=  libffead-framework.la
AM_CPPFLAGS +=  -I"../../framework" \
			    -I"../../modules/cache" \
				-I"../../modules/cache/memcached" \
				-I"../../modules/cache/memory" \
				-I"../../modules/cache/redis" \
				-I"../../modules/client-util" \
				-I"../../modules/component" \
				-I"../../modules/distocache" \
				-I"../../modules/http" \
				-I"../../modules/http/http11" \
				-I"../../modules/http/http20" \
				-I"../../modules/interpreter" \
				-I"../../modules/ioc" \
				-I"../../modules/jobs" \
				-I"../../modules/methinvoker" \
				-I"../../modules/msghandler" \
				-I"../../modules/reflection" \
				-I"../../modules/sdorm" \
				-I"../../modules/sdorm/gtm" \
				-I"../../modules/sdorm/mongo" \
				-I"../../modules/sdorm/sql" \
				-I"../../modules/serialization" \
				-I"../../modules/serialization/binary" \
				-I"../../modules/serialization/json" \
				-I"../../modules/serialization/xml" \
				-I"../../modules/server-util" \
				-I"../../modules/ssl" \
				-I"../../modules/search" \
				-I"../../modules/search/solr" \
				-I"../../modules/search/elasticsearch" \
				-I"../../modules/threads"
if HAVE_SSL
AM_CPPFLAGS += \
				-I"../../modules/client-util/ssl"
endif
libffead_framework_la_SOURCES = \
				../../framework/AfcUtil.cpp \
				../../framework/ApplicationUtil.cpp \
				../../framework/AuthController.cpp \
				../../framework/ConfigurationData.cpp \
				../../framework/ConfigurationHandler.cpp \
				../../framework/ControllerHandler.cpp \
				../../framework/DBAuthController.cpp \
				../../framework/DCPGenerator.cpp \
				../../framework/ExtHandler.cpp \
				../../framework/FileAuthController.cpp \
				../../framework/FilterHandler.cpp \
				../../framework/FormHandler.cpp \
				../../framework/FviewHandler.cpp \
				../../framework/LDAPAuthController.cpp \
				../../framework/ScriptHandler.cpp \
				../../framework/SecurityHandler.cpp \
				../../framework/ServiceTask.cpp \
				../../framework/SoapHandler.cpp \
				../../framework/TemplateGenerator.cpp \
				../../framework/WsUtil.cpp \
				../../framework/ServerInitUtil.cpp \
				../../framework/c_interface.cpp
libffead_modules_la_SOURCES += \
						../../modules/cache/CacheInterface.cpp \
						../../modules/cache/CacheManager.cpp \
						../../modules/cache/memcached/MemcachedImpl.cpp \
						../../modules/cache/memory/MemoryCacheImpl.cpp \
						../../modules/cache/redis/RedisCacheImpl.cpp \
						../../modules/search/SearchQuery.cpp \
						../../modules/search/IndexQuery.cpp \
						../../modules/search/DocumentQuery.cpp \
						../../modules/search/SearchEngineManager.cpp \
						../../modules/search/SearchEngineInterface.cpp \
						../../modules/search/solr/SolrSearch.cpp \
						../../modules/common/MurmurHash3.cpp \
						../../modules/search/elasticsearch/ElasticSearch.cpp \
						../../modules/client-util/Client.cpp \
						../../modules/client-util/ClientInterface.cpp \
						../../modules/client-util/HttpClient.cpp \
						../../modules/component/AppContext.cpp \
						../../modules/component/BeanContext.cpp \
						../../modules/component/Component.cpp \
						../../modules/component/ComponentGen.cpp \
						../../modules/component/ComponentHandler.cpp \
						../../modules/component/RemoteComponent.cpp \
						../../modules/component/Service.cpp \
						../../modules/component/ServiceInt.cpp \
						../../modules/component/ServicePool.cpp \
						../../modules/distocache/BasicDistoCacheConnectionFactory.cpp \
						../../modules/distocache/CacheMap.cpp \
						../../modules/distocache/DistoCacheClientUtils.cpp \
						../../modules/distocache/DistoCacheHandler.cpp \
						../../modules/distocache/DistoCacheServiceHandler.cpp \
						../../modules/distocache/PooledDistoCacheConnectionFactory.cpp \
						../../modules/http/ContentTypes.cpp \
						../../modules/http/CORSHandler.cpp \
						../../modules/http/http11/Http11Handler.cpp \
						../../modules/http/http11/Http11WebSocketDataFrame.cpp \
						../../modules/http/http11/Http11WebSocketHandler.cpp \
						../../modules/http/http20/Http2AlternativeServicesFrame.cpp \
						../../modules/http/http20/Http2ContinuationFrame.cpp \
						../../modules/http/http20/Http2DataFrame.cpp \
						../../modules/http/http20/Http2Frame.cpp \
						../../modules/http/http20/Http2FrameHeader.cpp \
						../../modules/http/http20/Http2GoAwayFrame.cpp \
						../../modules/http/http20/Http2Handler.cpp \
						../../modules/http/http20/Http2HeadersFrame.cpp \
						../../modules/http/http20/Http2HPACKContext.cpp \
						../../modules/http/http20/Http2PingFrame.cpp \
						../../modules/http/http20/Http2PriorityFrame.cpp \
						../../modules/http/http20/Http2PushPromiseFrame.cpp \
						../../modules/http/http20/Http2ReadWriteUtil.cpp \
						../../modules/http/http20/Http2ResetStreamFrame.cpp \
						../../modules/http/http20/Http2SettingsFrame.cpp \
						../../modules/http/http20/Http2StreamHandler.cpp \
						../../modules/http/http20/Http2WindowUpdateFrame.cpp \
						../../modules/http/HttpRequest.cpp \
						../../modules/http/HttpResponse.cpp \
						../../modules/http/HttpResponseParser.cpp \
						../../modules/http/HTTPResponseStatus.cpp \
						../../modules/http/HttpServiceHandler.cpp \
						../../modules/http/HttpSession.cpp \
						../../modules/http/MultipartContent.cpp \
						../../modules/http/WebSockHandler.cpp \
						../../modules/interpreter/CppInterpreter.cpp \
						../../modules/ioc/FFEADContext.cpp \
						../../modules/jobs/CronTimer.cpp \
						../../modules/jobs/JobScheduler.cpp \
						../../modules/methinvoker/MethodInvoc.cpp \
						../../modules/msghandler/Destination.cpp \
						../../modules/msghandler/InvalidMessageException.cpp \
						../../modules/msghandler/Message.cpp \
						../../modules/msghandler/MessageHandler.cpp \
						../../modules/msghandler/MessageUtil.cpp \
						../../modules/msghandler/MQueue.cpp \
						../../modules/reflection/Class.cpp \
						../../modules/reflection/ClassInfo.cpp \
						../../modules/reflection/Constructor.cpp \
						../../modules/reflection/Field.cpp \
						../../modules/reflection/Method.cpp \
						../../modules/reflection/Reflection.cpp \
						../../modules/reflection/Reflector.cpp \
						../../modules/sdorm/DataSourceInterface.cpp \
						../../modules/sdorm/DataSourceManager.cpp \
						../../modules/sdorm/DataSourceMapping.cpp \
						../../modules/sdorm/gtm/GTM.cpp \
						../../modules/sdorm/HiloIdGenerator.cpp \
						../../modules/sdorm/IDGenerator.cpp \
						../../modules/sdorm/mongo/MongoDBConnectionPool.cpp \
						../../modules/sdorm/mongo/MongoDBDataSourceImpl.cpp \
						../../modules/sdorm/Query.cpp \
						../../modules/sdorm/QueryBuilder.cpp \
						../../modules/sdorm/sql/DialectHelper.cpp \
						../../modules/sdorm/sql/SQLConnectionPool.cpp \
						../../modules/sdorm/sql/SQLDataSourceImpl.cpp \
						../../modules/serialization/binary/AMEFDecoder.cpp \
						../../modules/serialization/binary/AMEFEncoder.cpp \
						../../modules/serialization/binary/AMEFObject.cpp \
						../../modules/serialization/binary/AMEFResources.cpp \
						../../modules/serialization/binary/BinarySerialize.cpp \
						../../modules/serialization/json/JSONSerialize.cpp \
						../../modules/serialization/SerializeBase.cpp \
						../../modules/serialization/xml/XMLSerialize.cpp \
						../../modules/server-util/NBServer.cpp \
						../../modules/server-util/RequestReaderHandler.cpp \
						../../modules/server-util/RequestHandler2.cpp \
						../../modules/server-util/SelEpolKqEvPrt.cpp \
						../../modules/server-util/Server.cpp \
						../../modules/server-util/ServiceHandler.cpp \
						../../modules/server-util/SocketInterface.cpp \
						../../modules/ssl/SSLCommon.cpp \
						../../modules/ssl/SSLHandler.cpp \
						../../modules/threads/FutureTask.cpp \
						../../modules/threads/Mutex.cpp \
						../../modules/threads/PoolThread.cpp \
						../../modules/threads/Task.cpp \
						../../modules/threads/TaskPool.cpp \
						../../modules/threads/Thread.cpp \
						../../modules/threads/ThreadLocal.cpp \
						../../modules/threads/ThreadPool.cpp
if HAVE_SSL
libffead_modules_la_SOURCES += \
						../../modules/client-util/ssl/SSLClient.cpp
endif
else
if MOD_DEFAULT
lib_LTLIBRARIES +=  libffead-framework.la
AM_CPPFLAGS +=  -I"../../framework" \
	    -I"../../modules/cache" \
		-I"../../modules/cache/memory" \
		-I"../../modules/client-util" \
		-I"../../modules/http" \
		-I"../../modules/http/http11" \
		-I"../../modules/http/http20" \
		-I"../../modules/interpreter" \
		-I"../../modules/ioc" \
		-I"../../modules/jobs" \
		-I"../../modules/reflection" \
		-I"../../modules/sdorm" \
		-I"../../modules/serialization" \
		-I"../../modules/serialization/binary" \
		-I"../../modules/serialization/json" \
		-I"../../modules/serialization/xml" \
		-I"../../modules/server-util" \
		-I"../../modules/ssl" \
		-I"../../modules/search" \
		-I"../../modules/search/solr" \
		-I"../../modules/search/elasticsearch" \
		-I"../../modules/threads"
if HAVE_SSL
AM_CPPFLAGS += \
				-I"../../modules/client-util/ssl"
endif
libffead_framework_la_SOURCES = \
		../../framework/AfcUtil.cpp \
		../../framework/ApplicationUtil.cpp \
		../../framework/AuthController.cpp \
		../../framework/ConfigurationData.cpp \
		../../framework/ConfigurationHandler.cpp \
		../../framework/ControllerHandler.cpp \
		../../framework/DBAuthController.cpp \
		../../framework/DCPGenerator.cpp \
		../../framework/ExtHandler.cpp \
		../../framework/FileAuthController.cpp \
		../../framework/FilterHandler.cpp \
		../../framework/FormHandler.cpp \
		../../framework/FviewHandler.cpp \
		../../framework/LDAPAuthController.cpp \
		../../framework/ScriptHandler.cpp \
		../../framework/SecurityHandler.cpp \
		../../framework/ServiceTask.cpp \
		../../framework/SoapHandler.cpp \
		../../framework/TemplateGenerator.cpp \
		../../framework/WsUtil.cpp \
		../../framework/ServerInitUtil.cpp \
		../../framework/c_interface.cpp
libffead_modules_la_SOURCES += \
				../../modules/cache/CacheInterface.cpp \
				../../modules/cache/CacheManager.cpp \
				../../modules/cache/memory/MemoryCacheImpl.cpp \
				../../modules/search/SearchQuery.cpp \
				../../modules/search/IndexQuery.cpp \
				../../modules/search/DocumentQuery.cpp \
				../../modules/search/SearchEngineManager.cpp \
				../../modules/search/SearchEngineInterface.cpp \
				../../modules/client-util/Client.cpp \
				../../modules/client-util/ClientInterface.cpp \
				../../modules/client-util/HttpClient.cpp \
				../../modules/http/ContentTypes.cpp \
				../../modules/http/CORSHandler.cpp \
				../../modules/http/http11/Http11Handler.cpp \
				../../modules/http/http11/Http11WebSocketDataFrame.cpp \
				../../modules/http/http11/Http11WebSocketHandler.cpp \
				../../modules/http/http20/Http2AlternativeServicesFrame.cpp \
				../../modules/http/http20/Http2ContinuationFrame.cpp \
				../../modules/http/http20/Http2DataFrame.cpp \
				../../modules/http/http20/Http2Frame.cpp \
				../../modules/http/http20/Http2FrameHeader.cpp \
				../../modules/http/http20/Http2GoAwayFrame.cpp \
				../../modules/http/http20/Http2Handler.cpp \
				../../modules/http/http20/Http2HeadersFrame.cpp \
				../../modules/http/http20/Http2HPACKContext.cpp \
				../../modules/http/http20/Http2PingFrame.cpp \
				../../modules/http/http20/Http2PriorityFrame.cpp \
				../../modules/http/http20/Http2PushPromiseFrame.cpp \
				../../modules/http/http20/Http2ReadWriteUtil.cpp \
				../../modules/http/http20/Http2ResetStreamFrame.cpp \
				../../modules/http/http20/Http2SettingsFrame.cpp \
				../../modules/http/http20/Http2StreamHandler.cpp \
				../../modules/http/http20/Http2WindowUpdateFrame.cpp \
				../../modules/http/HttpRequest.cpp \
				../../modules/http/HttpResponse.cpp \
				../../modules/http/HttpResponseParser.cpp \
				../../modules/http/HTTPResponseStatus.cpp \
				../../modules/http/HttpServiceHandler.cpp \
				../../modules/http/HttpSession.cpp \
				../../modules/http/MultipartContent.cpp \
				../../modules/http/WebSockHandler.cpp \
				../../modules/interpreter/CppInterpreter.cpp \
				../../modules/ioc/FFEADContext.cpp \
				../../modules/jobs/CronTimer.cpp \
				../../modules/jobs/JobScheduler.cpp \
				../../modules/reflection/Class.cpp \
				../../modules/reflection/ClassInfo.cpp \
				../../modules/reflection/Constructor.cpp \
				../../modules/reflection/Field.cpp \
				../../modules/reflection/Method.cpp \
				../../modules/reflection/Reflection.cpp \
				../../modules/reflection/Reflector.cpp \
				../../modules/sdorm/DataSourceInterface.cpp \
				../../modules/sdorm/DataSourceManager.cpp \
				../../modules/sdorm/DataSourceMapping.cpp \
				../../modules/sdorm/HiloIdGenerator.cpp \
				../../modules/sdorm/IDGenerator.cpp \
				../../modules/sdorm/Query.cpp \
				../../modules/sdorm/QueryBuilder.cpp \
				../../modules/serialization/binary/AMEFDecoder.cpp \
				../../modules/serialization/binary/AMEFEncoder.cpp \
				../../modules/serialization/binary/AMEFObject.cpp \
				../../modules/serialization/binary/AMEFResources.cpp \
				../../modules/serialization/binary/BinarySerialize.cpp \
				../../modules/serialization/json/JSONSerialize.cpp \
				../../modules/serialization/SerializeBase.cpp \
				../../modules/serialization/xml/XMLSerialize.cpp \
				../../modules/server-util/NBServer.cpp \
				../../modules/server-util/RequestReaderHandler.cpp \
				../../modules/server-util/RequestHandler2.cpp \
				../../modules/server-util/SelEpolKqEvPrt.cpp \
				../../modules/server-util/Server.cpp \
				../../modules/server-util/ServiceHandler.cpp \
				../../modules/server-util/SocketInterface.cpp \
				../../modules/ssl/SSLCommon.cpp \
				../../modules/ssl/SSLHandler.cpp \
				../../modules/threads/FutureTask.cpp \
				../../modules/threads/Mutex.cpp \
				../../modules/threads/PoolThread.cpp \
				../../modules/threads/Task.cpp \
				../../modules/threads/TaskPool.cpp \
				../../modules/threads/Thread.cpp \
				../../modules/threads/ThreadLocal.cpp \
				../../modules/threads/ThreadPool.cpp
if HAVE_SSL
libffead_modules_la_SOURCES += \
				../../modules/client-util/ssl/SSLClient.cpp
endif
else
if MOD_COMPONENT
AM_CPPFLAGS += \
				-I"../../modules/component"
libffead_modules_la_SOURCES += \
						../../modules/component/AppContext.cpp \
						../../modules/component/BeanContext.cpp \
						../../modules/component/Component.cpp \
						../../modules/component/ComponentGen.cpp \
						../../modules/component/ComponentHandler.cpp \
						../../modules/component/RemoteComponent.cpp \
						../../modules/component/Service.cpp \
						../../modules/component/ServiceInt.cpp \
						../../modules/component/ServicePool.cpp \
						../../modules/serialization/xml/XMLSerialize.cpp \
						../../modules/sdorm/sql/DialectHelper.cpp \
						../../modules/sdorm/sql/SQLConnectionPool.cpp \
						../../modules/sdorm/sql/SQLDataSourceImpl.cpp
endif
if MOD_METHINVOKER
AM_CPPFLAGS += \
				-I"../../modules/serialization/xml" \
				-I"../../modules/methinvoker"
libffead_modules_la_SOURCES += \
						../../modules/serialization/xml/XMLSerialize.cpp \
						../../modules/methinvoker/MethodInvoc.cpp
endif
if MOD_MSGHANDLER
AM_CPPFLAGS += \
				-I"../../modules/serialization/binary" \
				-I"../../modules/msghandler"
libffead_modules_la_SOURCES += \
						../../modules/msghandler/Destination.cpp \
						../../modules/msghandler/InvalidMessageException.cpp \
						../../modules/msghandler/Message.cpp \
						../../modules/msghandler/MessageHandler.cpp \
						../../modules/msghandler/MessageUtil.cpp \
						../../modules/msghandler/MQueue.cpp \
						../../modules/serialization/binary/AMEFDecoder.cpp \
						../../modules/serialization/binary/AMEFEncoder.cpp \
						../../modules/serialization/binary/AMEFObject.cpp \
						../../modules/serialization/binary/AMEFResources.cpp \
						../../modules/serialization/binary/BinarySerialize.cpp
endif
if MOD_SDORM_SQL
AM_CPPFLAGS += \
				-I"../../modules/sdorm/sql"
libffead_modules_la_SOURCES += \
						../../modules/sdorm/sql/DialectHelper.cpp \
						../../modules/sdorm/sql/SQLConnectionPool.cpp \
						../../modules/sdorm/sql/SQLDataSourceImpl.cpp
AM_CPPFLAGS += \
				-I"../../modules/sdorm/sql/libpq"
libffead_modules_la_SOURCES += \
						../../modules/sdorm/sql/libpq/LibpqDataSourceImpl.cpp
endif
if MOD_SDORM_MONGO
AM_CPPFLAGS += \
				-I"../../modules/sdorm/mongo" -I"../../modules/sdorm/mongo/raw"
libffead_modules_la_SOURCES += \
						../../modules/sdorm/mongo/MongoDBDataSourceImpl.cpp \
						../../modules/sdorm/mongo/MongoDBConnectionPool.cpp \
						../../modules/sdorm/mongo/raw/MongoDBRawDataSourceImpl.cpp
endif
if MOD_SDORM_SCYLLA
AM_CPPFLAGS += \
				-I"../../modules/sdorm/scylla/raw"
libffead_modules_la_SOURCES += \
						../../modules/sdorm/scylla/raw/ScyllaDBRawDataSourceImpl.cpp
endif
if MOD_DISTOCACHE
AM_CPPFLAGS += \
				-I"../../modules/serialization/binary" \
				-I"../../modules/distocache"
libffead_modules_la_SOURCES += \
						../../modules/serialization/binary/AMEFDecoder.cpp \
						../../modules/serialization/binary/AMEFEncoder.cpp \
						../../modules/serialization/binary/AMEFObject.cpp \
						../../modules/serialization/binary/AMEFResources.cpp \
						../../modules/serialization/binary/BinarySerialize.cpp \
						../../modules/distocache/DistoCacheHandler.cpp \
						../../modules/distocache/DistoCacheServiceHandler.cpp \
						../../modules/distocache/CacheMap.cpp \
						../../modules/distocache/DistoCacheClientUtils.cpp \
						../../modules/distocache/BasicDistoCacheConnectionFactory.cpp \
						../../modules/distocache/PooledDistoCacheConnectionFactory.cpp
endif
if MOD_XMLSERIALIZE
AM_CPPFLAGS += \
				-I"../../modules/serialization/xml"
libffead_modules_la_SOURCES += \
						../../modules/serialization/xml/XMLSerialize.cpp
endif
if MOD_BINSERIALIZE
AM_CPPFLAGS += \
				-I"../../modules/serialization/binary"
libffead_modules_la_SOURCES += \
						../../modules/serialization/binary/AMEFDecoder.cpp \
						../../modules/serialization/binary/AMEFEncoder.cpp \
						../../modules/serialization/binary/AMEFObject.cpp \
						../../modules/serialization/binary/AMEFResources.cpp \
						../../modules/serialization/binary/BinarySerialize.cpp
endif
if MOD_INTERPRETER
AM_CPPFLAGS += \
				-I"../../modules/interpreter"
libffead_modules_la_SOURCES += \
						../../modules/interpreter/CppInterpreter.cpp
endif
if MOD_JOBS
AM_CPPFLAGS += \
				-I"../../modules/jobs"
libffead_modules_la_SOURCES += \
						../../modules/jobs/JobScheduler.cpp \
						../../modules/jobs/CronTimer.cpp
endif
if MOD_MEMCACHED
AM_CPPFLAGS += \
				-I"../../modules/cache/memcached"
libffead_modules_la_SOURCES += \
						../../modules/cache/memcached/MemcachedImpl.cpp
endif
if MOD_REDISCACHE
AM_CPPFLAGS += \
				-I"../../modules/cache/redis"
libffead_modules_la_SOURCES += \
						../../modules/cache/redis/RedisCacheImpl.cpp
endif
if MOD_GTM
AM_CPPFLAGS += \
				-I"../../modules/sdorm/gtm"
libffead_modules_la_SOURCES += \
						../../modules/sdorm/gtm/GTM.cpp
endif
if MOD_SOLR
AM_CPPFLAGS += \
				-I"../../modules/search/solr"
libffead_modules_la_SOURCES += \
						../../modules/search/solr/SolrSearch.cpp ../../modules/common/MurmurHash3.cpp
endif
if MOD_ELASTIC
AM_CPPFLAGS += \
				-I"../../modules/search/elasticsearch"
libffead_modules_la_SOURCES += \
						../../modules/elasticsearch/ElasticSearch.cpp
endif

if MOD_HTTPSERVER
lib_LTLIBRARIES +=  libffead-framework.la
AM_CPPFLAGS += \
				-I"../../framework"
libffead_framework_la_SOURCES = \
		../../framework/AfcUtil.cpp \
		../../framework/AuthController.cpp \
		../../framework/ConfigurationData.cpp \
		../../framework/ConfigurationHandler.cpp \
		../../framework/ControllerHandler.cpp \
		../../framework/DBAuthController.cpp \
		../../framework/ExtHandler.cpp \
		../../framework/FileAuthController.cpp \
		../../framework/FilterHandler.cpp \
		../../framework/FormHandler.cpp \
		../../framework/FviewHandler.cpp \
		../../framework/LDAPAuthController.cpp \
		../../framework/ScriptHandler.cpp \
		../../framework/SecurityHandler.cpp \
		../../framework/ServiceTask.cpp \
		../../framework/ServerInitUtil.cpp \
		../../framework/c_interface.cpp

if MOD_DCP
libffead_modules_la_SOURCES += \
						../../framework/DCPGenerator.cpp
endif
if MOD_TPE
libffead_modules_la_SOURCES += \
						../../framework/TemplateGenerator.cpp
endif
if MOD_APPFLOW
libffead_modules_la_SOURCES += \
						../../framework/ApplicationUtil.cpp
endif
if MOD_WEBSERVICE
libffead_modules_la_SOURCES += \
						../../framework/SoapHandler.cpp \
						../../framework/WsUtil.cpp
endif
endif
endif
endif

if MOD_SDORM_SQL
AM_CPPFLAGS += \
				-I"../../modules/sdorm/sql"
libffead_modules_la_SOURCES += \
						../../modules/sdorm/sql/DialectHelper.cpp \
						../../modules/sdorm/sql/SQLConnectionPool.cpp \
						../../modules/sdorm/sql/SQLDataSourceImpl.cpp
AM_CPPFLAGS += \
				-I"../../modules/sdorm/sql/libpq"
libffead_modules_la_SOURCES += \
						../../modules/sdorm/sql/libpq/LibpqDataSourceImpl.cpp
endif

if MOD_SDORM_MONGO
AM_CPPFLAGS += \
				-I"../../modules/sdorm/mongo" -I"../../modules/sdorm/mongo/raw"
libffead_modules_la_SOURCES += \
						../../modules/sdorm/mongo/MongoDBDataSourceImpl.cpp \
						../../modules/sdorm/mongo/MongoDBConnectionPool.cpp \
						../../modules/sdorm/mongo/raw/MongoDBRawDataSourceImpl.cpp
endif
if MOD_SDORM_SCYLLA
AM_CPPFLAGS += \
				-I"../../modules/sdorm/scylla/raw"
libffead_modules_la_SOURCES += \
						../../modules/sdorm/scylla/raw/ScyllaDBRawDataSourceImpl.cpp
endif
if MOD_MEMCACHED
AM_CPPFLAGS += \
				-I"../../modules/cache/memcached"
libffead_modules_la_SOURCES += \
						../../modules/cache/memcached/MemcachedImpl.cpp
endif
if MOD_REDISCACHE
AM_CPPFLAGS += \
				-I"../../modules/cache/redis"
libffead_modules_la_SOURCES += \
						../../modules/cache/redis/RedisCacheImpl.cpp
endif
if MOD_GTM
AM_CPPFLAGS += \
				-I"../../modules/sdorm/gtm"
libffead_modules_la_SOURCES += \
						../../modules/sdorm/gtm/GTM.cpp
endif
if USE_WEPOLL
AM_CPPFLAGS += \
				-I"../../modules/wepoll"
libffead_modules_la_SOURCES += \
						../../modules/wepoll/wepoll.cpp
endif

libffead_modules_la_SOURCES += \
						../../modules/common/GenericObject.cpp

libffead_modules_la_LDFLAGS = -no-undefined
libffead_framework_la_LDFLAGS = -lffead-modules -no-undefined

