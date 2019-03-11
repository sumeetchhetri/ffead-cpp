
file(GLOB sources
    "*.cpp"
)

add_executable(tests ${sources})
set_property(TARGET tests PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(tests ffead-modules ffead-framework ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${HAVE_CURLLIB} ${HAVE_SSLLIB} ${HAVE_MEMCACHEDLIB} ${HAVE_ODBCLIB} ${HAVE_MONGOCLIB} ${HAVE_BSONLIB} ${HAVE_ZLIB} ${HAVE_CRYPTOLIB})
