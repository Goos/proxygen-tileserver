#!/usr/bin/env bash

add-apt-repository -y ppa:mapnik/nightly-2.3
apt-get update

apt-get install -y git
apt-get install -y unzip

if [ ! -f "/usr/bin/clang++-3.5" ]; then
  apt-get install -y clang-3.5 lldb-3.6
  if [ -f "/usr/bin/clang++" ]; then
    mv /usr/bin/clang++ /usr/bin/clang++_old
  fi
  ln -s /usr/bin/clang++-3.5 /usr/bin/clang++
  ln -s /usr/bin/lldb-3.6 /usr/bin/lldb
fi

apt-get install -y libmapnik libmapnik-dev mapnik-utils python-mapnik

if [ ! -d "/var/www/deps" ]; then 
  mkdir /var/www/deps
fi

if [ ! -f "/usr/local/lib/libproxygenlib.a" ]; then
  cd /var/www/deps
  git clone https://github.com/facebook/proxygen.git proxygen
  cd proxygen/proxygen
  git checkout v0.28.0
  ./deps.sh && ./reinstall.sh
fi

if [ ! -f "/usr/local/lib/libr3.a" ]; then
  cd /var/www/deps
  apt-get install check libpcre3 libpcre3-dev libjemalloc-dev libjemalloc1 build-essential libtool automake autoconf pkg-config
  git clone https://github.com/Goos/r3.git r3
  cd r3
  ./autogen.sh
  ./configure && make
  sudo make install
fi

