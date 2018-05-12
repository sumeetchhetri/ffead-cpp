SUBDIRS = src/autotools
ACLOCAL_AMFLAGS = ${ACLOCAL_AMFLAGS} -I m4

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
	src/framework/*.h
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
	src/framework/*.h
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
endif
if MOD_SDORM_MONGO
includedirsall += \
				src/modules/sdorm/mongo/*.h
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
endif

if MOD_SDORM_MONGO
includedirsall += \
				src/modules/sdorm/mongo/*.h
endif
if MOD_MEMCACHED
includedirsall += \
				src/modules/cache/memcached/*.h
endif
if MOD_REDISCACHE
includedirsall += \
				src/modules/cache/redis/*.h
endif
if MOD_GTM
includedirsall += \
				src/modules/sdorm/gtm/*.h
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
#	@echo === install-data-hook ===
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
	cp -Rf src/autotools/.libs/*ffead_* ${prefix}/lib/
	cp -Rf src/autotools/.libs/CHS ${prefix}/
	cp -Rf public/* ${prefix}/public/
	cp -f ${includedirsall} ${prefix}/include/
	cp -Rf rtdcf/* ${prefix}/rtdcf/
	cp -Rf web/* ${prefix}/web
	cp -Rf script/* ${prefix}/
#cp -f web/default/src/autotools/.libs/*default* ${prefix}/lib
#cp -f web/flexApp/src/autotools/.libs/*flexApp* ${prefix}/lib
#cp -f web/oauthApp/src/autotools/.libs/*oauthApp* ${prefix}/lib
#cp -f web/markers/src/autotools/.libs/*marker* ${prefix}/lib
	-rm -f ${prefix}/lib/*.la ${prefix}/lib/*.lai
	sed -i "s/TO_REPLACE_DEBUG/$(subst /,\/,@BUILD_DEBUG@)/" $(prefix)/resources/rundyn-configure.sh
	sed -i "s/TO_REPLACE_CPPFLAGS/$(subst /,\/,@BUILD_CPPFLAGS@)/" $(prefix)/resources/rundyn-configure.sh
	sed -i "s/TO_REPLACE_LDFLAGS/$(subst /,\/,@BUILD_LDFLAGS@)/" $(prefix)/resources/rundyn-configure.sh
	sed -i "s/TO_REPLACE_AM_CXXFLAGS/$(subst /,\/,@BUILD_AM_CXXFLAGS@)/" $(prefix)/resources/rundyn-configure.sh
#-rm -rf $(prefix)/web/default/src/ $(prefix)/web/flexApp/src/ $(prefix)/web/oauthApp/src/ $(prefix)/web/markers/src/
		
build-apps: install
	cd tests/autotools && make
	cp -f tests/autotools/.libs/tests tests/test.csv \
		tests/testValues.prop tests/runTests.sh \
		tests/*.pem tests/security.prop ${prefix}/tests
	for webdir in web/*/ ; do \
		dir=$${webdir%*/}; \
		wdir=`echo $${dir##*/}`; \
		echo $${wdir}; \
		if [ -d "$${webdir}/src/" ] && [ -d "$${webdir}/src/autotools" ] && [ -d "$${webdir}/include" ]; then \
			hc=`ls $${webdir}/include/*.h 2>/dev/null|wc -l`; \
			sc=`ls $${webdir}/src/*.cpp 2>/dev/null|wc -l`; \
			if [ $$hc != 0 ] && [ $$sc != 0 ] && [ -f "$${webdir}/src/autotools/Makefile" ]; then \
				weblibname=`echo "$${wdir}" | tr '-' '_'`; \
				cd $${webdir}/src/autotools && make; \
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
	cd @GENERATE_APACHEMOD@ && make
endif

build-nginx-mod:
if MOD_NGINXMOD
	-mkdir tmp
	./@GENERATE_NGINXMOD@/build-nginx.sh "$(prefix)" "@INCPATHS_NGINXMOD@" "@LIBPATHS_NGINXMOD@" "${abs_builddir}/@GENERATE_NGINXMOD@"
endif

dist_noinst_SCRIPTS = autogen.sh
