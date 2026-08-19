#!/usr/bin/env bash
# Configure, build and install the compiler into $ROOT/minizinc, against the
# vendor deps already in $ROOT/vendor.
#
# Env: ROOT, CMAKE_GENERATOR, BUILD_REF, EXPECT_DEPS, CMAKE_OSX_ARCHITECTURES
set -eux
set -o pipefail # or the tee below would mask a cmake failure

: "${ROOT:?ROOT must be set}"

cmake -S "$ROOT" -B "$ROOT/build" -G "${CMAKE_GENERATOR:-Ninja}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_REF="${BUILD_REF:-0}" \
  -DGecode_ROOT="$ROOT/vendor/gecode" \
  -DOsiCBC_ROOT="$ROOT/vendor/cbc" \
  -DCMAKE_INSTALL_PREFIX="$ROOT/minizinc" \
  -DCMAKE_OSX_ARCHITECTURES="${CMAKE_OSX_ARCHITECTURES:-arm64}" \
  -DCMAKE_OSX_DEPLOYMENT_TARGET="12.0" 2>&1 | tee "$ROOT/configure.log"
  # Deployment target stated, not inherited: otherwise the binaries take the
  # runner image's own macOS version as their floor. Matches vendor and the IDE.

# CMake reports a missing optional backend as a status message, so a build that
# silently lost one still succeeds and publishes -- musl shipped without CBC that
# way. Assert instead that everything fetched reached the configuration summary,
# which only prints a line once the target exists. Not a blanket "nothing was
# missing": BISON, FLEX, Geas, chuffed and atlantis are absent by design.
for dep in ${EXPECT_DEPS:-}; do
	case "$dep" in
	gecode) label="Gecode" ;;
	cbc) label="OSICBC" ;;
	# Fail closed, so a new fetched dep cannot skip the check by omission.
	*)
		echo "ERROR: no CMake summary label known for fetched dependency '$dep'" >&2
		exit 1
		;;
	esac
	if ! grep -qE "^	${label} " "$ROOT/configure.log"; then
		echo "ERROR: '$dep' was fetched for this platform, but CMake did not build it in" >&2
		exit 1
	fi
done

cmake --build "$ROOT/build" --parallel --target install
