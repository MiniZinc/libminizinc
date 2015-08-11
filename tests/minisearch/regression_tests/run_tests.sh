#!/bin/bash
#
# Regression tests for MiniSearch
#
# NOTE: Set the paths and solvers according to your system

# path to MiniSearch executable
EXE_PATH="/home/arendl/work/libmzn/build/"  
# the MiniSearch executable
EXE="mzn-fzn-lite"  
MZN_EXE=$EXE_PATH$EXE
# the flatzinc solvers that should be tested
FZN_SOLVERS=("fzn-gecode" "fzn_chuffed" "fzn_choco" "fzn-ortools")

# be verbose about the tests
VERBOSE=false

for solver in "${FZN_SOLVERS[@]}"; do
    for file in $( ls *.mzn ); do
	if [ "$VERBOSE" = true ] ; then
	    echo "$MZN_EXE --solver $solver $file > $file.$solver.out 2> $file.$solver.err"
	fi
	$MZN_EXE --solver $solver $file > $file.$solver.out 2> $file.$solver.err
	if [ -s $file.$solver.err ] # if $file.$solver.err is not empty
	then 
	    echo "ERROR: $solver: $file"
	else 
	    echo "OK: $solver: $file"
	    rm $file.err
	fi
    done
done