
file(GLOB sources
    "include/*.h"
    "src/*.cpp"
)

include_directories("${CMAKE_SOURCE_DIR}/web/te-benchmark/include")
if(BUILD_STATIC_LIBS OR EMSCRIPTEN)
	add_library(te-benchmark STATIC ${sources})
else()
	add_library(te-benchmark ${sources})
endif()
target_link_libraries(te-benchmark ffead-modules ffead-framework ${HAVE_CURLLIB} ${HAVE_SSLLIB} ${HAVE_MEMCACHEDLIB} ${HAVE_ODBCLIB} ${HAVE_MONGOCLIB} ${HAVE_BSONLIB} ${HAVE_ZLIB} ${HAVE_CRYPTOLIB})
