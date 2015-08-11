#!/bin/bash
#
# Regression tests for MiniSearch

EXE_PATH="/home/arendl/work/libmzn/build/"
EXE="mzn-fzn-lite"
MZN_EXE=$EXE_PATH$EXE
FZN_SOLVER="/home/arendl/software/gecode/svn-trunk/tools/flatzinc/fzn-gecode"
VERBOSE=false

for file in $( ls *.mzn ); do
  if [ "$VERBOSE" = true ] ; then
      echo "$MZN_EXE --solver $FZN_SOLVER $file > $file.out 2> $file.err"
  fi
  $MZN_EXE --solver $FZN_SOLVER $file > $file.out 2> $file.err
  if [ -s $file.err ] # if $file.err is not empty
  then 
       echo "ERROR: $file"
  else 
       echo "OK: $file"
       rm $file.err
  fi
done