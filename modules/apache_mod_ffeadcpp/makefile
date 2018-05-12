APXS=apxs
APACHECTL=apachectl

# Get all of apxs's internal values.
APXS_CC=`$(APXS) -q CC`   
APXS_TARGET=`$(APXS) -q TARGET`
APXS_CFLAGS=`$(APXS) -q CFLAGS`
APXS_SBINDIR=`$(APXS) -q SBINDIR`  
APXS_CFLAGS_SHLIB=`$(APXS) -q CFLAGS_SHLIB`  
APXS_INCLUDEDIR=`$(APXS) -q INCLUDEDIR`
APXS_LD_SHLIB=`$(APXS) -q LD_SHLIB`
APXS_LIBEXECDIR=`$(APXS) -q LIBEXECDIR`
APXS_LDFLAGS_SHLIB=`$(APXS) -q LDFLAGS_SHLIB`
APXS_SYSCONFDIR=`$(APXS) -q SYSCONFDIR`
APXS_LIBS_SHLIB=`$(APXS) -q LIBS_SHLIB`
APR_INCLUDEDIR=`$(APXS) -q APR_INCLUDEDIR`

#the default target
all: mod_ffeadcpplib.so

# compile
mod_ffeadcpp.o: mod_ffeadcpp.cpp
	g++ -c -fPIC -I"$(APXS_INCLUDEDIR)" -I"$(APR_INCLUDEDIR)" -I"${FFEAD_CPP_PATH}/include" -I"." $(APXS_CFLAGS) $(APXS_CFLAGS_SHLIB) -fpermissive -w -o $@ $< 

# link
mod_ffeadcpplib.so: mod_ffeadcpp.o 
	g++ -fPIC -L"${FFEAD_CPP_PATH}/lib" -shared -o $@ $< $(APXS_LIBS_SHLIB) -lweblib -ldl -lcrypto -lssl -lapr-1 -laprutil-1 ${HTTPD_LIB}

# install the shared object file into Apache 
install: all
	$(APXS) -i -n 'ffeadcpp_module' mod_ffeadcpplib.so
	
# display the apxs variables
check_apxs_vars:
	@echo APXS_CC $(APXS_CC);\
	echo APXS_TARGET $(APXS_TARGET);\
	echo APXS_CFLAGS $(APXS_CFLAGS);\
	echo APXS_SBINDIR $(APXS_SBINDIR);\
	echo APXS_CFLAGS_SHLIB $(APXS_CFLAGS_SHLIB);\
	echo APXS_INCLUDEDIR $(APXS_INCLUDEDIR);\
	echo APXS_LD_SHLIB $(APXS_LD_SHLIB);\
	echo APXS_LIBEXECDIR $(APXS_LIBEXECDIR);\
	echo APXS_LDFLAGS_SHLIB $(APXS_LDFLAGS_SHLIB);\
	echo APXS_SYSCONFDIR $(APXS_SYSCONFDIR);\
	echo APXS_LIBS_SHLIB $(APXS_LIBS_SHLIB)

#   cleanup
clean:
	-rm -f *.so *.o *~

#   install and activate shared object by reloading Apache to
#   force a reload of the shared object file
reload: install restart

#   the general Apache start/restart/stop
#   procedures
start:
	$(APACHECTL) start
restart:
	$(APACHECTL) restart
stop:
	$(APACHECTL) stop

