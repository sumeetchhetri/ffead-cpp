install(CODE "execute_process(COMMAND bash -c \"cd ${PROJECT_SOURCE_DIR} && wget -q http://nginx.org/download/nginx-1.19.1.tar.gz && tar xf nginx-1.19.1.tar.gz\")")
install(CODE "execute_process(COMMAND bash -c \"cd ${PROJECT_SOURCE_DIR}/nginx-1.19.1 && ./configure --prefix=${PROJECT_SOURCE_DIR}/../nginxfc --with-ld-opt=\\\"-lstdc++ -L/usr/local/lib -L${PROJECT_SOURCE_DIR}/ffead-cpp-7.0-bin/lib\\\" --add-module=\\\"${PROJECT_SOURCE_DIR}/modules/nginx_mod_ffeadcpp\\\" --with-cc-opt=\\\"-I${PROJECT_SOURCE_DIR}/ffead-cpp-7.0-bin/include ${CMAKE_CXX_FLAGS_NGX} -w -fpermissive -std=gnu++17\\\"\")")
#install(CODE "execute_process(COMMAND bash -c \"sed -i'' -e 's|-o objs/addon/../ngx_http_ffeadcpp_module.o|@STDCXX_SWITCH@ -o objs/addon/../ngx_http_ffeadcpp_module.o|g' ${PROJECT_SOURCE_DIR}/nginx-1.19.1/objs/Makefile\")")
install(CODE "execute_process(COMMAND bash -c \"sed -i'' -e 's|-g||g' ${PROJECT_SOURCE_DIR}/nginx-1.19.1/objs/Makefile\")")
install(CODE "execute_process(COMMAND bash -c \"sed -i'' -e 's|-O|-O3|g' ${PROJECT_SOURCE_DIR}/nginx-1.19.1/objs/Makefile\")")
install(CODE "execute_process(COMMAND bash -c \"cd ${PROJECT_SOURCE_DIR}/nginx-1.19.1 && make && make install && cd ${PROJECT_SOURCE_DIR} && rm -rf nginx-1.19.1\")")
install(CODE "execute_process(COMMAND bash -c \"cp ${PROJECT_SOURCE_DIR}/modules/nginx_mod_ffeadcpp/nginx.conf ${PROJECT_SOURCE_DIR}/../nginxfc/conf/\")")
install(CODE "execute_process(COMMAND bash -c \"sed -i'' -e 's|FFEAD_PATH|'${PROJECT_SOURCE_DIR}/ffead-cpp-7.0'|g' ${PROJECT_SOURCE_DIR}/../nginxfc/conf/nginx.conf\")")
