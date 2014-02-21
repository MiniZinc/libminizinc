#!/bin/sh
PATH="$(pwd)/..:":"$(pwd)/scripts":"$PATH"
MZN_STDLIB_DIR="$(pwd)/../mznlib"
exec run-tests mzn20_fd .mzn unit examples
