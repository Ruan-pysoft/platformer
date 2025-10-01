#!/bin/sh

set -x -e

# set up some variables

SRCDIR="src/"
BUILDDIR="build/"
NAME="game"

CC="x86_64-w64-mingw32-g++"
CFLAGS="-O2 -g"
CFLAGS="$CFLAGS -Wall -Wextra"
CFLAGS="$CFLAGS -I./include -I./raylib/src" # -I./raygui/src"
CFLAGS_EXTERNAL="-O2 -g"
CFLAGS_EXTERNAL="$CFLAGS_EXTERNAL -I./include -I./raylib/src" # -I./raygui/src"

LD="x86_64-w64-mingw32-g++"
LDFLAGS="-O2"
LDFLAGS="$LDFLAGS -L./raylib-5.5_win64_mingw-w64/lib -lraylib -lm -lgdi32 -lwinmm"
LDFLAGS="$LDFLAGS -static-libgcc -static-libstdc++"
LDFLAGS="$LDFLAGS -static"

# create the build directory

mkdir -p $BUILDDIR

# compile the object files for the kernel

OBJS=""

#$CC $CFLAGS_EXTERNAL -x c -c ./raygui/src/raygui.h -o $BUILDDIR/raygui.o -DRAYGUI_IMPLEMENTATION
#OBJS="$OBJS $BUILDDIR/raygui.o"
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
$CC $CFLAGS -c $SRCDIR/gui.cpp -o $BUILDDIR/gui.o
OBJS="$OBJS $BUILDDIR/gui.o"
$CC $CFLAGS -c $SRCDIR/level_select.cpp -o $BUILDDIR/level_select.o
OBJS="$OBJS $BUILDDIR/level_select.o"

# link the kernel

$LD -o "$BUILDDIR/$NAME" $OBJS $LDFLAGS
