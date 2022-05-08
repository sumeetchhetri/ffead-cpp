function do_setup() {
	LOG_MODE=1
	BUILD_SYS=emb
	BUILD_PLATFORM=c_cpp
}
function do_start() {
	set_out "."
	finc_cpp_compiler "Please install a c++ compiler to proceed"
	cpp_flags "@CPPFLAGS@"
	l_flags "@LFLAGS@"
	add_lib_path "/usr/local/opt/openssl/lib" "/usr/local/lib"
	add_inc_path "/usr/local/opt/openssl/include" "/usr/local/include" "/usr/include/libmongoc-1.0"
	add_inc_path "/usr/include/libbson-1.0" "/usr/local/include/libmongoc-1.0" "/usr/local/include/libbson-1.0"
	add_lib "@LIBS@"
	add_inc_path "../include"
	add_lib_path "../lib"
	for wedbdir in ../web/*
	do
		wedbdir=${wedbdir%*/}
		wedbdir="${wedbdir##*/}"
		add_inc_path "../web/$wedbdir/include"
	done
	set_src_files "ReflectorInterface.cpp,SerializeInterface.cpp,AjaxInterface.cpp,WsInterface.cpp" "shared:inter" "ffead-framework,ffead-modules"
	set_src_files "DCPInterface.cpp,TemplateInterface.cpp" "shared:dinter" "ffead-framework,ffead-modules"
}
function do_install() {
	mv ".bin/libinter.${SHLIB_EXT}" ".bin/libdinter.${SHLIB_EXT}" ../lib/
}