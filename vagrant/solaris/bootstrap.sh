#!/usr/bin/env bash
#
# Prepares a virtual machine for running ffead-cpp

# A shell provisioner is called multiple times
if [ ! -e "~/.firstboot" ]; then

  # Install prerequisite tools
  echo "Installing dependencies"
  portsnap fetch extract
  cd /usr/ports/ports-mgmt/pkg && make install && make clean

  pkg install cmake
  pkg install openssl
  pkg install unixODBC
  pkg install hiredis
  pkg install libmemcached
  pkg install e2fsprogs-libuuid
  pkg install wget
  
  cd /tmp
  wget -q https://github.com/mongodb/mongo-c-driver/releases/download/1.4.0/mongo-c-driver-1.4.0.tar.gz
  tar xf mongo-c-driver-1.4.0.tar.gz
  cd mongo-c-driver-1.4.0/ && \
  ./configure --prefix=/usr/local --libdir=/usr/local --disable-automatic-init-and-cleanup && \
  make && make install
  cd /tmp
  rm -rf mongo-c-driver-1.4.0

  # Setting up passwordless sudo
  echo "vagrant ALL=(ALL:ALL) NOPASSWD: ALL" | sudo tee -a /etc/sudoers
  
fi
