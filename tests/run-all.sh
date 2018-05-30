#!/bin/sh
# set -x
export PATH="$(pwd)/..":"$(pwd)/scripts":"$PATH"
export MZN_STDLIB_DIR="$(pwd)/../share/minizinc"
#run-tests mzn20_fd .mzn unit examples
run-tests mzn-fzn_fd .mzn unit examples
#run-tests mzn20_fd_linear .mzn unit examples
#exec run-tests mzn20_mip .mzn unit examples
