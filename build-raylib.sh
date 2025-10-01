#!/bin/sh

set -x -e

cd raylib/src

make PLATFORM=PLATFORM_DESKTOP RAYLIB_BUILD_MODE=DEBUG

cd ../..
