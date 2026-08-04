#!/usr/bin/env bash
# Build the CLI package: compiler + bundled solvers, no IDE.
#
# Usage: package_cli.sh <triple> <version>
#
# Uses the non-Qt `gecode` build (not gecode_gist), so the package has no Qt
# dependency; the IDE bundles gecode_gist separately.
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
    cp -a vendor/highs/lib64/libhighs.so* "$PACKAGE/lib/"
    ;;
esac

if [ -z "$EXE" ]; then
  for f in "$PACKAGE"/bin/*; do
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
