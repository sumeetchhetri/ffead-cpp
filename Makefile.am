SUBDIRS = src/autotools tests/autotools web/default/src/autotools \
	web/flexApp/src/autotools web/oauthApp/src/autotools @GENERATE_APACHEMOD@

modules = all
packageIdentifier=${PACKAGE_NAME}-${PACKAGE_VERSION}-bin
distdir=${PACKAGE_NAME}-${PACKAGE_VERSION}-src
prefix=${abs_builddir}/${packageIdentifier}
bindir=${prefix}
libdir=${prefix}/lib

EXTRA_DIST = Debug Release resources rtdcf script tests web public lib apache_mod_ffeadcpp
dist-hook: clean
	rm -rf `find $(distdir)/Debug -type d -name .svn`
	rm -rf `find $(distdir)/Debug -type d -name .svn`
	rm -rf `find $(distdir)/resources -type d -name .svn`
	rm -rf `find $(distdir)/rtdcf -type d -name .svn`
	rm -rf `find $(distdir)/tests -type d -name .svn`
	rm -rf `find $(distdir)/web -type d -name .svn`
	rm -rf `find $(distdir)/include -type d -name .svn`
	rm -rf `find $(distdir)/src -type d -name .svn`
	rm -rf `find $(distdir)/public -type d -name .svn`
	rm -rf `find $(distdir)/lib -type d -name .svn`
	rm -rf `find $(distdir)/apache_mod_ffeadcpp -type d -name .svn`
	rm -rf `find $(distdir)/Debug -type d -name .git`
	rm -rf `find $(distdir)/Debug -type d -name .git`
	rm -rf `find $(distdir)/resources -type d -name .git`
	rm -rf `find $(distdir)/rtdcf -type d -name .git`
	rm -rf `find $(distdir)/tests -type d -name .git`
	rm -rf `find $(distdir)/web -type d -name .git`
	rm -rf `find $(distdir)/include -type d -name .git`
	rm -rf `find $(distdir)/src -type d -name .git`
	rm -rf `find $(distdir)/public -type d -name .git`
	rm -rf `find $(distdir)/lib -type d -name .git`
	rm -rf `find $(distdir)/apache_mod_ffeadcpp -type d -name .git`
	cp include/* $(distdir)/include
	cp release-notes* $(distdir)
	cp *INSTALL-* $(distdir)
	-rm -rf $(distdir)/tests/autotools/.libs $(distdir)/tests/*.o $(distdir)/tests/*.lo \
		$(distdir)/tests/.deps/*.Plo
	-rm -rf $(distdir)/web/default/src/*.o $(distdir)/web/default/src/*.lo \
		$(distdir)/web/default/src/.libs $(distdir)/web/default/src/.deps/*.Plo \
		$(distdir)/web/default/src/autotools/.libs $(distdir)/web/default/src/autotools/lib*
	-rm -rf $(distdir)/web/flexApp/src/*.o $(distdir)/web/flexApp/src/*.lo \
		$(distdir)/web/flexApp/src/.libs $(distdir)/web/flexApp/src/.deps/*.Plo \
		$(distdir)/web/flexApp/src/autotools/.libs $(distdir)/web/flexApp/src/autotools/lib*
	-rm -rf $(distdir)/web/oauthApp/src/*.o $(distdir)/web/oauthApp/src/*.lo \
		$(distdir)/web/oauthApp/src/.libs $(distdir)/web/oauthApp/src/.deps/*.Plo \
		$(distdir)/web/oauthApp/src/autotools/.libs $(distdir)/web/oauthApp/src/autotools/lib*

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
	-rm -f ${prefix}/lib/*.la ${prefix}/lib/*.lai
	-rm -rf $(prefix)/web/default/src/ $(prefix)/web/flexApp/src/ $(prefix)/web/oauthApp/src/
		
build-apps: install
	cp -f tests/autotools/.libs/tests tests/test.csv \
		tests/testValues.prop tests/runTests.sh \
		tests/*.pem tests/security.prop ${prefix}/tests
	cp -f web/default/src/autotools/.libs/*default* ${prefix}/lib
	cp -f web/flexApp/src/autotools/.libs/*flexApp* ${prefix}/lib
	cp -f web/oauthApp/src/autotools/.libs/*oauthApp* ${prefix}/lib
	-rm -f ${prefix}/lib/*.la ${prefix}/lib/*.lai
	-rm -rf $(prefix)/web/default/src/ $(prefix)/web/flexApp/src/ $(prefix)/web/oauthApp/src/
	
build-bin-dist: install
	tardir=$(packageIdentifier) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(packageIdentifier).tar.gz
	
build-bin-dist-noinst:
	tardir=$(packageIdentifier) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(packageIdentifier).tar.gz	

dist_noinst_SCRIPTS = autogen.sh
