SUBDIRS = src/autotools tests/autotools web/default/src/autotools web/flexApp/src/autotools web/oauthApp/src/autotools

modules = all
prefix=${abs_builddir}/ffead-server
bindir=${prefix}
libdir=${prefix}/lib

EXTRA_DIST = Debug Release resources rtdcf script tests web
dist-hook:
	rm -rf `find $(distdir)/Debug -type d -name .svn`
	rm -rf `find $(distdir)/Debug -type d -name .svn`
	rm -rf `find $(distdir)/resources -type d -name .svn`
	rm -rf `find $(distdir)/rtdcf -type d -name .svn`
	rm -rf `find $(distdir)/tests -type d -name .svn`
	rm -rf `find $(distdir)/web -type d -name .svn`
	rm -rf `find $(distdir)/Debug -type d -name .git`
	rm -rf `find $(distdir)/Debug -type d -name .git`
	rm -rf `find $(distdir)/resources -type d -name .git`
	rm -rf `find $(distdir)/rtdcf -type d -name .git`
	rm -rf `find $(distdir)/tests -type d -name .git`
	rm -rf `find $(distdir)/web -type d -name .git`

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
	cp -Rf resources/* ${prefix}/resources/
	cp -Rf src/autotools/.libs/*weblib* ${prefix}/lib/
	cp -Rf src/autotools/.libs/CHS ${prefix}/
	cp -Rf public/* ${prefix}/public/
	cp -Rf include/* ${prefix}/include/
	cp -Rf rtdcf/* ${prefix}/rtdcf/
	cp -Rf web/* ${prefix}/web
	cp -Rf script/* ${prefix}/
	cp -f tests/autotools/.libs/tests tests/test.csv \
		tests/testValues.prop tests/runTests.sh \
		tests/*.pem tests/security.prop ${prefix}/tests
	cp -f web/default/src/autotools/.libs/*default* ${prefix}/lib
	cp -f web/flexApp/src/autotools/.libs/*flexApp* ${prefix}/lib
	cp -f web/oauthApp/src/autotools/.libs/*oauthApp* ${prefix}/lib
	
build-apps:
	cp -f tests/autotools/.libs/tests tests/test.csv \
		tests/testValues.prop tests/runTests.sh \
		tests/*.pem tests/security.prop ${prefix}/tests
	cp -f web/default/src/autotools/.libs/*default* ${prefix}/lib
	cp -f web/flexApp/src/autotools/.libs/*flexApp* ${prefix}/lib
	cp -f web/oauthApp/src/autotools/.libs/*oauthApp* ${prefix}/lib

#install-data-hook:
#	@echo === install-data-hook ===
#	-rm $(DESTDIR)/$(libdir)/$(module_LTLIBRARIES)


dist_noinst_SCRIPTS = autogen.sh