
file(GLOB sources
    "include/*.h"
    "src/*.cpp"
)

include_directories("${CMAKE_SOURCE_DIR}/web/oauthApp/include")
if(BUILD_STATIC_LIBS OR EMSCRIPTEN)
	add_library(oauthApp STATIC ${sources})
else()
	add_library(oauthApp ${sources})
endif()
target_link_libraries(oauthApp ffead-modules ffead-framework ${HAVE_CURLLIB} ${HAVE_SSLLIB} ${HAVE_MEMCACHEDLIB} ${HAVE_ODBCLIB} ${HAVE_MONGOCLIB} ${HAVE_BSONLIB} ${HAVE_ZLIB} ${HAVE_CRYPTOLIB})
