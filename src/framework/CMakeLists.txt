
file(GLOB framework_sources
    "*.h"
    "*.cpp"
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/framework/ DESTINATION ${PROJECT_NAME}-bin/include FILES_MATCHING PATTERN "*.h")
add_library(ffead-framework ${framework_sources})
target_link_libraries(ffead-framework ffead-modules ${HAVE_CURLLIB} ${HAVE_SSLLIB} ${HAVE_REDISLIB} ${HAVE_MEMCACHEDLIB} ${HAVE_MEMCACHEDUTILLIB} ${HAVE_ODBCLIB} ${HAVE_MONGOCLIB} ${HAVE_BSONLIB} ${HAVE_ZLIB} ${HAVE_CRYPTOLIB})
