#!/bin/sh

set -x -e

# set up some variables

SRCDIR="src/"
BUILDDIR="build/"
NAME="game"

CC="g++"
CFLAGS="-O2 -g"
CFLAGS="$CFLAGS -Wall -Wextra"
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
$CC $CFLAGS -c $SRCDIR/input_manager.cpp -o $BUILDDIR/input_manager.o
OBJS="$OBJS $BUILDDIR/input_manager.o"
$CC $CFLAGS -c $SRCDIR/actions.cpp -o $BUILDDIR/actions.o
OBJS="$OBJS $BUILDDIR/actions.o"
$CC $CFLAGS -c $SRCDIR/level.cpp -o $BUILDDIR/level.o
OBJS="$OBJS $BUILDDIR/level.o"
$CC $CFLAGS -c $SRCDIR/main_menu.cpp -o $BUILDDIR/main_menu.o
OBJS="$OBJS $BUILDDIR/main_menu.o"
$CC $CFLAGS -c $SRCDIR/levels_list.cpp -o $BUILDDIR/levels_list.o
OBJS="$OBJS $BUILDDIR/levels_list.o"

# link the kernel

$LD -o "$BUILDDIR/$NAME" $OBJS $LDFLAGS
