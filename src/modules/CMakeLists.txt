
if(MOD_MEMORY)
	file(GLOB mem_module_sources
	    "cache/memory/*.h"
	    "cache/memory/*.cpp"
	)
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/cache/memory/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
endif(MOD_MEMORY)

if(MOD_REDIS)
	file(GLOB red_module_sources
	    "cache/redis/*.h"
	    "cache/redis/*.cpp"
	)
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/cache/redis/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
endif(MOD_REDIS)

if(MOD_MEMCACHED)
	file(GLOB memc_module_sources
	    "cache/memcached/*.h"
	    "cache/memcached/*.cpp"
	)
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/cache/memcached/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
endif(MOD_MEMCACHED)

if(MOD_SDORM_SQL)
	file(GLOB sql_pq_module_sources
	    "sdorm/sql/libpq/*.h"
	    "sdorm/sql/libpq/*.cpp"
	)
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/sdorm/sql/libpq/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
	file(GLOB sql_module_sources
	    "sdorm/sql/*.h"
	    "sdorm/sql/*.cpp"
	)
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/sdorm/sql/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
endif(MOD_SDORM_SQL)

if(MOD_SDORM_MONGO)
	file(GLOB mongo_module_sources
	    "sdorm/mongo/*.h"
	    "sdorm/mongo/*.cpp"
	)
	file(GLOB mongo_raw_module_sources
	    "sdorm/mongo/raw/*.h"
	    "sdorm/mongo/raw/*.cpp"
	)
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/sdorm/mongo/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/sdorm/mongo/raw/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
endif(MOD_SDORM_MONGO)

if(MOD_SER_BIN)
	file(GLOB serb_module_sources
	    "serialization/binary/*.h"
	    "serialization/binary/*.cpp"
	)
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/serialization/binary/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
endif(MOD_SER_BIN)

if(MOD_JOBS)
	file(GLOB job_module_sources
	    "jobs/*.h"
	    "jobs/*.cpp"
	)
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/jobs/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
endif(MOD_JOBS)

if(OS_MINGW)
	file(GLOB wepoll_sources
	    "wepoll/*.h"
	    "wepoll/*.cpp"
	)
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/wepoll/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
endif(OS_MINGW)

file(GLOB comm_module_sources
    "common/*.h"
    "common/*.cpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/common/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
file(GLOB client_module_sources
    "client-util/*.h"
    "client-util/*.cpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/client-util/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
file(GLOB cache_module_sources
    "cache/*.h"
    "cache/*.cpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/cache/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
file(GLOB sdorm_module_sources
    "sdorm/*.h"
    "sdorm/*.cpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/sdorm/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
file(GLOB http_module_sources
    "http/*.h"
    "http/*.cpp"
    "http/http11/*.h"
    "http/http11/*.cpp"
    "http/http20/*.h"
    "http/http20/*.cpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/http/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/http/http11/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/http/http20/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
file(GLOB ioc_module_sources
    "ioc/*.h"
    "ioc/*.cpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/ioc/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
file(GLOB reflection_module_sources
    "reflection/*.h"
    "reflection/*.cpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/reflection/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
file(GLOB serialization_module_sources
    "serialization/*.h"
    "serialization/*.cpp"
    "serialization/xml/*.h"
    "serialization/xml/*.cpp"
    "serialization/json/*.h"
    "serialization/json/*.cpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/serialization/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/serialization/xml/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/serialization/json/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
#file(GLOB search_module_sources
#    "search/*.h"
#    "search/*.cpp"
#)
#install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/search/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
file(GLOB serv_module_sources
    "server-util/*.h"
    "server-util/*.cpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/server-util/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
file(GLOB ssl_module_sources
    "ssl/*.h"
    "ssl/*.cpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/ssl/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
file(GLOB threads_module_sources
    "threads/*.h"
    "threads/*.cpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/modules/threads/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")

add_library(ffead-modules ${sql_pq_module_sources} ${mongo_raw_module_sources} ${wepoll_sources} ${comm_module_sources} ${client_module_sources} ${cache_module_sources} ${sdorm_module_sources} ${http_module_sources} ${ioc_module_sources} ${reflection_module_sources} ${serialization_module_sources} ${search_module_sources} ${serv_module_sources} ${ssl_module_sources} ${threads_module_sources} ${mem_module_sources} ${red_module_sources} ${memc_module_sources} ${sql_module_sources} ${mongo_module_sources} ${serb_module_sources} ${job_module_sources})
target_link_libraries(ffead-modules ${HAVE_LIBPQ} ${HAVE_PCREPOSIXLIB} ${HAVE_WINREGEX} ${HAVE_WSOCK32} ${HAVE_WS232} ${HAVE_DLLIB} ${HAVE_UUIDLIB} ${HAVE_CURLLIB} ${HAVE_SSLLIB} ${HAVE_REDISLIB} ${HAVE_MEMCACHEDLIB} ${HAVE_MEMCACHEDUTILLIB} ${HAVE_ODBCLIB} ${HAVE_MONGOCLIB} ${HAVE_BSONLIB} ${HAVE_ZLIB} ${HAVE_CRYPTOLIB})
