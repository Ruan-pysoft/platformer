#!/bin/sh

set -x -e

# set up some variables

SRCDIR="src/"
BUILDDIR="build/"
NAME="game"

CC="g++"
CFLAGS="-O2 -g"
CFLAGS="$CFLAGS -Wall -Wextra -pedantic"
CFLAGS="$CFLAGS -I./include -I./raylib/src"

LD="g++"
LDFLAGS="-O2"
LDFLAGS="$LDFLAGS -L./raylib/src -lraylib"

# create the build directory

mkdir -p $BUILDDIR

# compile the object files for the kernel

OBJS=""

$CC $CFLAGS -c $SRCDIR/main.cpp -o $BUILDDIR/main.o
OBJS="$OBJS $BUILDDIR/main.o"
$CC $CFLAGS -c $SRCDIR/game.cpp -o $BUILDDIR/game.o
OBJS="$OBJS $BUILDDIR/game.o"
$CC $CFLAGS -c $SRCDIR/player.cpp -o $BUILDDIR/player.o
OBJS="$OBJS $BUILDDIR/player.o"

# link the kernel

$LD -o "$BUILDDIR/$NAME" $OBJS $LDFLAGS
