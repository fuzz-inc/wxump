#!/bin/sh
WORKSPACE_DIR=`dirname $WORKSPACE_PATH`
BINDIR=$WORKSPACE_DIR/../../bin/mac
mkdir -p $BINDIR
for target in $*; do
  rm -rf $BINDIR/$target
  cp -r $TARGET_BUILD_DIR/$target $BINDIR
done
