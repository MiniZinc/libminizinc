#!/bin/bash
#
# Regression tests for MiniSearch
#
# NOTE: Set the paths and solvers according to your system

# path to MiniSearch executable
EXE_PATH="../../../build/"  
# the MiniSearch executable
EXE="mzn-gecode-lite"  
MZN_EXE=$EXE_PATH$EXE

# be verbose about the tests
VERBOSE=true
solver="mzn-gecode"

for file in $( ls *.mzn ); do
    if [ "$VERBOSE" = true ] ; then
	echo "$MZN_EXE $file > $file.$solver.out 2> $file.$solver.err"
    fi
    $MZN_EXE $file > $file.$solver.out 2> $file.$solver.err
    
    add_on=""
    if [ "$VERBOSE" = true ] ; then
	add_on="    $MZN_EXE $file > $file.$solver.out 2> $file.$solver.err"
    fi
    error_msg="ERROR: $solver: $file"$add_on
    success_msg="OK: $solver: $file"$add_on
    if [ -s $file.$solver.err ] # if $file.$solver.err is not empty
    then 
	echo $error_msg
        # else: err file is empty
    else 
	echo $success_msg
	rm $file.$solver.err
    fi
done