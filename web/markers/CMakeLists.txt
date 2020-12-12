
file(GLOB sources
    "include/*.h"
    "src/*.cpp"
)

include_directories("${CMAKE_SOURCE_DIR}/web/markers/include")
if(BUILD_STATIC_LIBS OR EMSCRIPTEN)
	add_library(markers STATIC ${sources})
else()
	add_library(markers ${sources})
endif()
target_link_libraries(markers ffead-modules ffead-framework ${HAVE_CURLLIB} ${HAVE_SSLLIB} ${HAVE_MEMCACHEDLIB} ${HAVE_ODBCLIB} ${HAVE_MONGOCLIB} ${HAVE_BSONLIB} ${HAVE_ZLIB} ${HAVE_CRYPTOLIB})
