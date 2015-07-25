#!/usr/bin/env bash

CLANG_VERSION=3.6

apt-get -y update
apt-get -y upgrade

apt-get install -y git unzip

if [ ! -f "/usr/bin/clang++${CLANG_VERSION}" ]; then
  apt-get install -y clang-${CLANG_VERSION} lldb-${CLANG_VERSION}
  if [ -f "/usr/bin/clang++" ]; then
    mv /usr/bin/clang++ /usr/bin/clang++_old
    mv /usr/bin/clang /usr/bin/clang_old
  fi
  ln -s /usr/bin/clang++-${CLANG_VERSION} /usr/bin/clang++
  ln -s /usr/bin/clang-${CLANG_VERSION} /usr/bin/clang
  ln -s /usr/bin/lldb-${CLANG_VERSION} /usr/bin/lldb
fi

if [ ! -d "/var/www/deps" ]; then 
  mkdir /var/www/deps
fi

# Mapnik
# - building boost 1.57.0 from source as apt only has 1.54.0
if [ ! -d "/usr/local/boost_1_57_0" ]; then
  apt-get install -y build-essential g++ python-dev autotools-dev libicu-dev build-essential libbz2-dev libboost-all-dev

  wget "http://sourceforge.net/projects/boost/files/boost/1.57.0/boost_1_57_0.tar.bz2/download" -O /usr/local/boost_1_57_0.tar.bz2
  cd /usr/local
  tar --bzip -xf boost_1_57_0.tar.bz2
  cd /usr/local/boost_1_57_0
  ./bootstrap.sh
  ./b2 install
fi


# - getting a bunch of other mapnik dependencies.
apt-get install -y \
  libicu-dev \
  python-dev libxml2 libxml2-dev \
  libfreetype6 libfreetype6-dev \
  libjpeg-dev \
  libpng-dev \
  libproj-dev \
  libtiff-dev \
  libcairo2 libcairo2-dev python-cairo python-cairo-dev \
  libcairomm-1.0-1 libcairomm-1.0-dev \
  ttf-unifont ttf-dejavu ttf-dejavu-core ttf-dejavu-extra \
  git build-essential python-nose \
  libgdal1-dev python-gdal \
  postgresql-9.3 postgresql-server-dev-9.3 postgresql-contrib-9.3 postgresql-9.3-postgis-2.1 \
  libsqlite3-dev

# - building harfbuzz from source.
if [ ! -f "/usr/local/lib/libharfbuzz.la" ]; then
  cd /var/www/deps

  wget http://www.freedesktop.org/software/harfbuzz/release/harfbuzz-0.9.26.tar.bz2
  tar xf harfbuzz-0.9.26.tar.bz2
  cd harfbuzz-0.9.26
  ./configure && make && sudo make install
  sudo ldconfig
fi

# - building mapnik from source with clang.
if [ ! -f "/usr/local/lib/libmapnik.a" ]; then
  cd /var/www/deps
  wget "https://mapnik.s3.amazonaws.com/dist/v3.0.0/mapnik-v3.0.0.tar.bz2" -O /var/www/deps/mapnik-v3_0_0.tar.bz2
  mkdir mapnik
  tar --bzip -xf mapnik-v3_0_0.tar.bz2 -C mapnik --strip-components=1
  cd mapnik
  ./configure CXX=clang++ CC=clang
  make && sudo make install
fi

# Proxygen (http server)
if [ ! -f "/usr/local/lib/libproxygenlib.a" ]; then
  cd /var/www/deps
  git clone https://github.com/facebook/proxygen.git proxygen
  cd proxygen/proxygen
  git checkout v0.28.0
  BOOST_ROOT=/usr/local/boost_1_57_0 ./deps.sh && ./reinstall.sh
fi

# R3 (routing)
if [ ! -f "/usr/local/lib/libr3.a" ]; then
  cd /var/www/deps
  apt-get install check libpcre3 libpcre3-dev libjemalloc-dev libjemalloc1 build-essential libtool automake autoconf pkg-config
  git clone https://github.com/Goos/r3.git r3
  cd r3
  ./autogen.sh
  ./configure && make
  sudo make install
fi

