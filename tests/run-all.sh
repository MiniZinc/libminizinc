#!/bin/sh
# set -x
export PATH="$(pwd)/..":"$(pwd)/scripts":"$PATH"
export MZN_STDLIB_DIR="$(pwd)/../share/minizinc"
exec run-tests mzn20_fd .mzn unit examples
