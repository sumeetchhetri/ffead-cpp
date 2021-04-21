#!/bin/sh

rm -rf build ffead-cpp-6.0-bin CMakeLists.txt-e CMakeLists.txt.template-e CMakeLists.txt.template.old
rm -f .autotools compile config.guess config.log config.status config.sub configure configure.ac-e
rm -f configure.ac.old configure.ac.template-e depcomp ffead-cpp install_manifest.txt install-sh libtool
rm -f ltmain.sh Makefile Makefile.in mingw-64-toolchain.cmake missing
rm -rf include src/framework/ffead-cpp-3.0.build tests/ffead-cpp-3.0.build
find . -name "*.lo" -type f -delete
find . -name "*.o" -type f -delete
find . -name ".dirstamp" -type f -delete
find . -name ".DS_Store" -type f -delete
find . -name "*.cpgz" -type f -delete
find . -name "cmake_install.cmake" -type f -delete
find . -name ".libs" -type d -print0|xargs -0 rm -r --
find . -name ".deps" -type d -print0|xargs -0 rm -r --
find . -name "CMakeFiles" -type d -print0|xargs -0 rm -r --
