AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

packageIdentifier=${PACKAGE_NAME}-${PACKAGE_VERSION}-bin
distdir=${PACKAGE_NAME}-${PACKAGE_VERSION}-src
fprefix=../../../../${packageIdentifier}
prefix=${abs_builddir}

# build nginx
install-data-hook:
	cd ${prefix} && wget -q http://nginx.org/download/nginx-1.19.1.tar.gz && tar xf nginx-1.19.1.tar.gz
	cd ${prefix}/nginx-1.19.1 && ./configure \
	    --prefix=../../../../nginxfc --with-ld-opt="-lstdc++ @NGX_LD_FLAGS@ -L${fprefix}/lib" --add-module="../../" \
	    --with-cc-opt="-I${fprefix}/include @NGX_CPP_FLAGS@ -w -fpermissive -std=gnu++17"
	cd ${prefix}/nginx-1.19.1 && make && make install
	cp ${prefix}/../nginx.conf ../../../../nginxfc/conf/
	sed -i'' -e 's|FFEAD_PATH|'${prefix}/ffead-cpp-6.0'|g' ../../../../nginxfc/conf/nginx.conf
	mv ${prefix}/nginxfc ../../../

#dist_noinst_SCRIPTS = autogen.sh