#!/bin/sh

set -x -e

# set up some variables

SRCDIR="src/"
BUILDDIR="build/"
NAME="game"

CC="g++"
CFLAGS="-O2 -g"
CFLAGS="$CFLAGS -Wall -Wextra -pedantic"
CFLAGS="$CFLAGS -I./include"

LD="g++"
LDFLAGS="-O2"
LDFLAGS="$LDFLAGS"

# create the build directory

mkdir -p $BUILDDIR

# compile the object files for the kernel

OBJS=""

$CC $CFLAGS -c $SRCDIR/main.cpp -o $BUILDDIR/main.o
OBJS="$OBJS $BUILDDIR/main.o"

LIBK_OBJS=""

# link the kernel

$LD -o "$BUILDDIR/$NAME" $OBJS
