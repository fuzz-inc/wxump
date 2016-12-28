#!/bin/sh

fullpath() {
    ruby -e "print(File.expand_path('$1'))"
}

CURDIR=`dirname $0`
WXDIR=`fullpath $CURDIR/../../wxWidgets`
PREFIX=`fullpath $CURDIR/../../mac/wxWidgets`
WORKDIR=`fullpath $CURDIR/work/wxWidgets`

mkdir -p $WORKDIR
cd $WORKDIR
$WXDIR/configure CXX="clang++ -stdlib=libc++ -fvisibility=hidden -fvisibility-inlines-hidden" --disable-shared --enable-monolithic --disable-webview --with-libpng=builtin --with-libjpeg=builtin --with-libtiff=builtin --prefix=$PREFIX --with-macosx-version-min=10.7
make
make install
