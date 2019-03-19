AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

packageIdentifier=${PACKAGE_NAME}-${PACKAGE_VERSION}-bin
distdir=${PACKAGE_NAME}-${PACKAGE_VERSION}-src
fprefix=../../../../${packageIdentifier}
prefix=${abs_builddir}

# build nginx
install-data-hook:
	cd ${prefix} && wget -q http://nginx.org/download/nginx-1.13.1.tar.gz && tar xf nginx-1.13.1.tar.gz
	cd ${prefix}/nginx-1.13.1 && ./configure \
	    --prefix=../../../nginxfc --with-ld-opt="-lstdc++ -L${fprefix}/lib" --add-module="../../" \
	    --with-cc-opt="-I${fprefix}/include -I/usr/local/opt/openssl/include -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0 -I/usr/include/libmongoc-1.0 -I/usr/include/libbson-1.0 -w -fpermissive -std=gnu++11"
	sed -i'' -e 's|-o objs/addon/../ngx_http_ffeadcpp_module.o|@STDCXX_SWITCH@ -o objs/addon/../ngx_http_ffeadcpp_module.o|g' ${prefix}/nginx-1.13.1/objs/Makefile
	cd ${prefix}/nginx-1.13.1 && make && make install
	cp ${prefix}/modules/nginx_mod_ffeadcpp/nginx.conf ../../../nginxfc/conf/
	sed -i'' -e 's|FFEAD_PATH|'${prefix}/ffead-cpp-3.0'|g' ${prefix}/nginxfc/conf/nginx.conf
	mv ${prefix}/nginxfc ../../../

#dist_noinst_SCRIPTS = autogen.sh