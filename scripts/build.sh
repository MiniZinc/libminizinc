#!/usr/bin/env bash
# Configure, build and install the compiler into $ROOT/minizinc, against the
# vendor deps already in $ROOT/vendor. Invoked by _build.yml, in a container or
# natively.
#
# Env: ROOT, CMAKE_GENERATOR, BUILD_REF
set -eux

: "${ROOT:?ROOT must be set}"

cmake -S "$ROOT" -B "$ROOT/build" -G "${CMAKE_GENERATOR:-Ninja}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_REF="${BUILD_REF:-0}" \
  -DGecode_ROOT="$ROOT/vendor/gecode" \
  -DCMAKE_INSTALL_PREFIX="$ROOT/minizinc" \
  -DCMAKE_OSX_ARCHITECTURES="arm64"
cmake --build "$ROOT/build" --parallel --target install
