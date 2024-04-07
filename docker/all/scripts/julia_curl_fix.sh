#!/bin/sh

JUL_VER=`julia -version|cut -d" " -f 3`
cp /usr/lib/x86_64-linux-gnu/libcurl.so* /root/.julia/juliaup/julia-${JUL_VER}+0.x64.linux.gnu/lib/julia/