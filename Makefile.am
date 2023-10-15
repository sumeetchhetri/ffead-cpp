SUBDIRS = src/autotools/libs src/autotools/bins
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

modules = all
packageIdentifier=${PACKAGE_NAME}-${PACKAGE_VERSION}-bin
distdir=${PACKAGE_NAME}-${PACKAGE_VERSION}-src
prefix=${abs_builddir}/${packageIdentifier}
bindir=${prefix}
libdir=${prefix}/lib

includedirsall=
if MOD_ALL
includedirsall += \
    src/modules/cache/*.h \
	src/modules/cache/memcached/*.h \
	src/modules/cache/memory/*.h \
	src/modules/cache/redis/*.h \
	src/modules/client-util/*.h \
	src/modules/component/*.h \
	src/modules/distocache/*.h \
	src/modules/http/*.h \
	src/modules/http/http11/*.h \
	src/modules/http/http20/*.h \
	src/modules/interpreter/*.h \
	src/modules/ioc/*.h \
	src/modules/jobs/*.h \
	src/modules/methinvoker/*.h \
	src/modules/msghandler/*.h \
	src/modules/reflection/*.h \
	src/modules/sdorm/*.h \
	src/modules/sdorm/gtm/*.h \
	src/modules/sdorm/mongo/*.h \
	src/modules/sdorm/sql/*.h \
	src/modules/serialization/*.h \
	src/modules/serialization/binary/*.h \
	src/modules/serialization/json/*.h \
	src/modules/serialization/xml/*.h \
	src/modules/server-util/*.h \
	src/modules/ssl/*.h \
	src/modules/threads/*.h \
	src/modules/search/*.h \
	src/framework/*.h
if HAVE_SSL
includedirsall += \
				src/modules/client-util/ssl/*.h
endif
else
if MOD_DEFAULT
includedirsall +=  \
    src/modules/cache/*.h \
	src/modules/cache/memory/*.h \
	src/modules/client-util/*.h \
	src/modules/http/*.h \
	src/modules/http/http11/*.h \
	src/modules/http/http20/*.h \
	src/modules/interpreter/*.h \
	src/modules/ioc/*.h \
	src/modules/jobs/*.h \
	src/modules/reflection/*.h \
	src/modules/sdorm/*.h \
	src/modules/serialization/*.h \
	src/modules/serialization/binary/*.h \
	src/modules/serialization/json/*.h \
	src/modules/serialization/xml/*.h \
	src/modules/server-util/*.h \
	src/modules/ssl/*.h \
	src/modules/threads/*.h \
	src/modules/search/*.h \
	src/framework/*.h
if HAVE_SSL
includedirsall += \
				src/modules/client-util/ssl/*.h
endif
else
if MOD_COMPONENT
includedirsall += \
				src/modules/component/*.h
endif
if MOD_METHINVOKER
includedirsall += \
				src/modules/serialization/xml/*.h \
				src/modules/methinvoker/*.h
endif
if MOD_MSGHANDLER
includedirsall += \
				src/modules/serialization/binary/*.h \
				src/modules/msghandler/*.h
endif
if MOD_SDORM_SQL
includedirsall += \
				src/modules/sdorm/sql/*.h
includedirsall += \
				src/modules/sdorm/sql/libpq/*.h
endif
if MOD_SDORM_MONGO
includedirsall += \
				src/modules/sdorm/mongo/*.h
includedirsall += \
				src/modules/sdorm/mongo/raw/*.h
endif
if MOD_SDORM_SCYLLA
includedirsall += \
				src/modules/sdorm/scylla/raw/*.h
endif
if MOD_DISTOCACHE
includedirsall += \
				src/modules/serialization/binary/*.h \
				src/modules/distocache/*.h
endif
if MOD_XMLSERIALIZE
includedirsall += \
				src/modules/serialization/xml/*.h
endif
if MOD_BINSERIALIZE
includedirsall += \
				src/modules/serialization/binary/*.h
endif
if MOD_INTERPRETER
includedirsall += \
				src/modules/interpreter/*.h
endif
if MOD_JOBS
includedirsall += \
				src/modules/jobs/*.h
endif
if MOD_MEMCACHED
includedirsall += \
				src/modules/cache/memcached/*.h
endif
if MOD_REDISCACHE
includedirsall += \
				src/modules/cache/redis/*.h
endif
if MOD_SOLR
includedirsall += \
				src/modules/search/solr/*.h
endif
if MOD_ELASTIC
includedirsall += \
				src/modules/search/elasticsearch/*.h
endif
if MOD_GTM
includedirsall += \
				src/modules/sdorm/gtm/*.h
endif
if MOD_HTTPSERVER
includedirsall += \
				src/framework/*.h
endif
endif
endif

if MOD_SDORM_SQL
includedirsall += \
				src/modules/sdorm/sql/*.h
includedirsall += \
				src/modules/sdorm/sql/libpq/*.h
endif

if MOD_SDORM_MONGO
includedirsall += \
				src/modules/sdorm/mongo/*.h
includedirsall += \
				src/modules/sdorm/mongo/raw/*.h
endif
if MOD_SDORM_SCYLLA
includedirsall += \
				src/modules/sdorm/scylla/raw/*.h
endif
if MOD_MEMCACHED
includedirsall += \
				src/modules/cache/memcached/*.h
endif
if MOD_REDISCACHE
includedirsall += \
				src/modules/cache/redis/*.h
endif
if MOD_SOLR
includedirsall += \
				src/modules/search/solr/*.h
endif
if MOD_ELASTIC
includedirsall += \
				src/modules/search/elasticsearch/*.h
endif
if MOD_GTM
includedirsall += \
				src/modules/sdorm/gtm/*.h
endif
if USE_WEPOLL
includedirsall += \
				src/modules/wepoll/*.h
endif
includedirsall += \
				src/modules/common/*.h


EXTRA_DIST = resources rtdcf script tests web public lib
dist-hook: clean
	rm -rf `find $(distdir)/resources -type d -name .svn`
	rm -rf `find $(distdir)/rtdcf -type d -name .svn`
	rm -rf `find $(distdir)/tests -type d -name .svn`
	rm -rf `find $(distdir)/web -type d -name .svn`
	rm -rf `find $(distdir)/src -type d -name .svn`
	rm -rf `find $(distdir)/public -type d -name .svn`
	rm -rf `find $(distdir)/lib -type d -name .svn`
	rm -rf `find $(distdir)/resources -type d -name .git`
	rm -rf `find $(distdir)/rtdcf -type d -name .git`
	rm -rf `find $(distdir)/tests -type d -name .git`
	rm -rf `find $(distdir)/web -type d -name .git`
	rm -rf `find $(distdir)/src -type d -name .git`
	rm -rf `find $(distdir)/public -type d -name .git`
	rm -rf `find $(distdir)/lib -type d -name .git`
	rm -rf include
#	cp include/* $(distdir)/include
	cp release-notes* $(distdir)
	cp *INSTALL-* $(distdir)
	-rm -rf $(distdir)/tests/autotools/.libs $(distdir)/tests/*.o $(distdir)/tests/*.lo \
		$(distdir)/tests/.deps/*.Plo
#-rm -rf $(distdir)/web/default/src/*.o $(distdir)/web/default/src/*.lo \
#	$(distdir)/web/default/src/.libs $(distdir)/web/default/src/.deps/*.Plo \
#	$(distdir)/web/default/src/autotools/.libs $(distdir)/web/default/src/autotools/lib*
#-rm -rf $(distdir)/web/flexApp/src/*.o $(distdir)/web/flexApp/src/*.lo \
#	$(distdir)/web/flexApp/src/.libs $(distdir)/web/flexApp/src/.deps/*.Plo \
#	$(distdir)/web/flexApp/src/autotools/.libs $(distdir)/web/flexApp/src/autotools/lib*
#-rm -rf $(distdir)/web/oauthApp/src/*.o $(distdir)/web/oauthApp/src/*.lo \
#	$(distdir)/web/oauthApp/src/.libs $(distdir)/web/oauthApp/src/.deps/*.Plo \
#	$(distdir)/web/oauthApp/src/autotools/.libs $(distdir)/web/oauthApp/src/autotools/lib*
#-rm -rf $(distdir)/web/markers/src/*.o $(distdir)/web/markers/src/*.lo \
#	$(distdir)/web/markers/src/.libs $(distdir)/web/markers/src/.deps/*.Plo \
#	$(distdir)/web/markers/src/autotools/.libs $(distdir)/web/markers/src/autotools/lib*

install-data-hook:
	-rm -rf ${prefix}
	mkdir ${prefix}
	mkdir ${prefix}/resources
	mkdir ${prefix}/lib
	mkdir ${prefix}/public
	mkdir ${prefix}/include
	mkdir ${prefix}/rtdcf
	mkdir ${prefix}/web
	mkdir ${prefix}/tmp
	mkdir ${prefix}/logs
	mkdir ${prefix}/tests
	-rm -rf include
	-mkdir include
	cp -f ${includedirsall} include/
	cp -Rf resources/* ${prefix}/resources/
	cp -Rf src/autotools/libs/.libs/*ffead-* ${prefix}/lib/
if IS_EXE
	cp -Rf src/autotools/bins/bin/ffead-cpp.exe ${prefix}/
else
	cp -Rf src/autotools/bins/bin/ffead-cpp ${prefix}/
endif
if SRV_ALL
if IS_EXE
	cp -Rf src/autotools/bins/bin/ffead-cpp-cinatra.exe ${prefix}/
	cp -Rf src/autotools/bins/bin/ffead-cpp-lithium.exe ${prefix}/
	cp -Rf src/autotools/bins/bin/ffead-cpp-drogon.exe ${prefix}/
else
	cp -Rf src/autotools/bins/bin/ffead-cpp-cinatra ${prefix}/
	cp -Rf src/autotools/bins/bin/ffead-cpp-lithium ${prefix}/
	cp -Rf src/autotools/bins/bin/ffead-cpp-drogon ${prefix}/
endif
endif
	cp -Rf public/* ${prefix}/public/
	cp -f ${includedirsall} ${prefix}/include/
	cp -Rf rtdcf/* ${prefix}/rtdcf/
	cp -Rf web/* ${prefix}/web
if MOD_SDORM_SQL
else
	rm -rf ${prefix}/web/t2
endif
if MOD_SDORM_MONGO
else
	rm -rf ${prefix}/web/t3
	rm -rf ${prefix}/web/t4
	rm -rf ${prefix}/web/t5
	rm -rf ${prefix}/web/t6
	rm -rf ${prefix}/web/t7
endif
	cp -Rf script/* ${prefix}/
	-rm -f ${prefix}/lib/*.la ${prefix}/lib/*.lai
	sed -i'' -e "s/TO_REPLACE_DEBUG/$(subst /,\/,@BUILD_DEBUG@)/" $(prefix)/resources/rundyn-configure.sh
	sed -i'' -e "s/TO_REPLACE_CPPFLAGS/$(subst /,\/,@BUILD_CPPFLAGS@)/" $(prefix)/resources/rundyn-configure.sh
	sed -i'' -e "s/TO_REPLACE_LDFLAGS/$(subst /,\/,@BUILD_LDFLAGS@)/" $(prefix)/resources/rundyn-configure.sh
	sed -i'' -e "s/TO_REPLACE_AM_CXXFLAGS/$(subst /,\/,@BUILD_AM_CXXFLAGS@)/" $(prefix)/resources/rundyn-configure.sh
	cd tests/autotools && make
if MOD_NGINXMOD
	cd modules/nginx_mod_ffeadcpp/autotools && make install
endif
if MOD_APACHEMOD	
	cd modules/apache_mod_ffeadcpp/autotools && make install
endif
if IS_EXE
	cp -f tests/autotools/tests.exe tests/test.csv \
		tests/testValues.prop tests/runTests.sh \
		tests/*.pem tests/security.prop ${prefix}/tests
else
	cp -f tests/autotools/tests tests/test.csv \
		tests/testValues.prop tests/runTests.sh \
		tests/*.pem tests/security.prop ${prefix}/tests
endif
	for webdir in web/*/ ; do \
		dir=$${webdir%*/}; \
		wdir=`echo $${dir##*/}`; \
		echo $${wdir}; \
		if [ -d "$${webdir}/src/" ] && [ -d "$${webdir}/src/autotools" ] && [ -d "$${webdir}/include" ]; then \
			hc=`ls $${webdir}/include/*.h 2>/dev/null|wc -l`; \
			sc=`ls $${webdir}/src/*.cpp 2>/dev/null|wc -l`; \
			if [ $$hc != 0 ] && [ $$sc != 0 ] && [ -f "$${webdir}/src/autotools/Makefile" ]; then \
				weblibname=`echo "$${wdir}"`; \
				cd $${webdir}/src/autotools && make clean && make; \
				cd -; \
				cp -f $${webdir}/src/autotools/.libs/*$${weblibname}* ${prefix}/lib; \
				rm -rf $(prefix)/web/$${wdir}/src/; \
			fi \
		fi \
	done
	-rm -f ${prefix}/lib/*.la ${prefix}/lib/*.lai
	
build-bin-dist: install
	tardir=$(packageIdentifier) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(packageIdentifier).tar.gz
	
build-bin-dist-noinst:
	tardir=$(packageIdentifier) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(packageIdentifier).tar.gz
	
build-apache-mod:
if MOD_APACHEMOD
	cd modules/apache_mod_ffeadcpp && make install
endif

build-nginx-mod:
if MOD_NGINXMOD
	cd modules/nginx_mod_ffeadcpp && make install
endif

dist_noinst_SCRIPTS = autogen.sh
