#!/bin/sh

cd

export WLD=$HOME/opt
export LD_LIBRARY_PATH=$WLD/lib
export PKG_CONFIG_PATH=$WLD/lib/pkgconfig/:$WLD/share/pkgconfig/
export PATH=$WLD/bin:$PATH
export ACLOCAL_PATH=$WLD/share/aclocal
export ACLOCAL="aclocal -I $ACLOCAL_PATH"

mkdir -p $WLD/share/aclocal

cd

git clone git://anongit.freedesktop.org/wayland/wayland
cd wayland
./autogen.sh --prefix=$WLD --disable-documentation
make && make install

cd && rm -rf wayland

git clone git://anongit.freedesktop.org/wayland/wayland-protocols
cd wayland-protocols
./autogen.sh --prefix=$WLD
make install

cd && rm rf wayland-protocols

git clone git://anongit.freedesktop.org/wayland/weston
cd weston
export LD_LIBRARY_PATH=$WLD/lib:$WLD/lib64
./autogen.sh --prefix=$WLD --disable-setuid-install --enable-demo-clients-install --enable-egl --with-jpeg
make
make install


