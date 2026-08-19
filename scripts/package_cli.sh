#!/usr/bin/env bash
# Build the CLI package: compiler + bundled solvers, no IDE.
#
# Usage: package_cli.sh <triple> <version>
#
# Takes the non-Qt `gecode` build, so the package has no Qt dependency; the IDE
# bundles gecode_gist instead. Also ships globalizer, findMUS and mzn-analyse.
#
# Expects minizinc/ (installed compiler) and vendor/ (fetch_vendor.sh) in $ROOT.
set -euo pipefail

TRIPLE="${1:?usage: package_cli.sh <triple> <version>}"
VERSION="${2:?usage: package_cli.sh <triple> <version>}"
ROOT="${ROOT:-$PWD}"
cd "$ROOT"

PACKAGE="MiniZinc-${VERSION}-${TRIPLE}"
rm -rf "$PACKAGE" && mkdir -p "$PACKAGE/bin" "$PACKAGE/share"

case "$TRIPLE" in
  *windows*) EXE=".exe" ;;
  *)         EXE=""     ;;
esac

cp -a minizinc/bin/. "$PACKAGE/bin/"
cp -a minizinc/share/. "$PACKAGE/share/"

for solver in gecode:fzn-gecode chuffed:fzn-chuffed or-tools:fzn-cp-sat; do
  dep="${solver%%:*}"
  bin="${solver##*:}"
  cp "vendor/$dep/bin/${bin}${EXE}" "$PACKAGE/bin/"
  cp -a "vendor/$dep/share/minizinc/." "$PACKAGE/share/minizinc/"
done

# The extra tools, laid out as <tool>/bin and <tool>/share/minizinc.
for tool in globalizer:minizinc-globalizer findMUS:findMUS mzn-analyse:mzn-analyse; do
  dir="${tool%%:*}"; bin="${tool##*:}"
  # GHC cannot target Windows ARM64, so Globalizer has no build there. Every
  # other absence is an error rather than something to skip past.
  if [ "$dir" = globalizer ] && [ "$TRIPLE" = aarch64-windows ]; then
    continue
  fi
  cp "$dir/bin/${bin}${EXE}" "$PACKAGE/bin/"
  # mzn-analyse installs only a binary; the others also carry an mznlib.
  if [ -d "$dir/share/minizinc" ]; then
    cp -a "$dir/share/minizinc/." "$PACKAGE/share/minizinc/"
  fi
done

# HiGHS is a loaded shared library and each platform puts it somewhere different.
case "$TRIPLE" in
  *windows*)
    cp vendor/highs/bin/*.dll "$PACKAGE/bin/"
    ;;
  *darwin*)
    mkdir -p "$PACKAGE/lib"
    cp -a vendor/highs/lib/libhighs*.dylib "$PACKAGE/lib/"
    ;;
  *)
    mkdir -p "$PACKAGE/lib"
    # lib64 on the manylinux (RedHat) images, lib on Alpine: GNUInstallDirs takes
    # the directory from the distro, so match either.
    cp -a vendor/highs/lib*/libhighs.so* "$PACKAGE/lib/"
    ;;
esac

# Windows binaries carry no embedded debug info to strip in the first place (no
# /DEBUG, no .pdb installed), and GNU strip isn't part of that toolchain anyway.
if [ -z "$EXE" ]; then
  for f in "$PACKAGE"/bin/* "$PACKAGE"/lib/*; do
    [ -f "$f" ] && [ -x "$f" ] && strip "$f" 2>/dev/null || true
  done
fi

mkdir -p dist
if [ -n "$EXE" ]; then
  rm -f "dist/${PACKAGE}.zip"
  if command -v zip >/dev/null; then
    zip -qr "dist/${PACKAGE}.zip" "$PACKAGE"
  else
    powershell -NoProfile -Command \
      "Compress-Archive -Path '${PACKAGE}' -DestinationPath 'dist/${PACKAGE}.zip' -Force"
  fi
  echo "dist/${PACKAGE}.zip"
else
  tar -czf "dist/${PACKAGE}.tar.gz" "$PACKAGE"
  echo "dist/${PACKAGE}.tar.gz"
fi
