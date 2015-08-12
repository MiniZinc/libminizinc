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
FZN_SOLVERS_INCLUDES=("/home/arendl/software/gecode/gecode-install/share/gecode/mznlib" # gecode
                      "/home/arendl/software/chuffed-angee/binary/linux/mznlib"         # chuffed
                      "/home/arendl/software/choco/choco-master-03-2015/choco-parsers-master/src/chocofzn/globals/" # choco
                      "/home/arendl/software/or-tools/or-git/src/flatzinc/mznlib"        # ortools
                       )

# be verbose about the tests
VERBOSE=false

for ((i=0; i < ${#FZN_SOLVERS}; i++)) 
do
    solver=${FZN_SOLVERS[$i]}
    globals=${FZN_SOLVERS_INCLUDES[$i]}
    for file in $( ls *.mzn ); do
	if [ "$VERBOSE" = true ] ; then
	    echo "$MZN_EXE --solver $solver -I$globals $file > $file.$solver.out 2> $file.$solver.err"
	fi
	$MZN_EXE --solver $solver -I$globals $file > $file.$solver.out 2> $file.$solver.err

	add_on=""
	if [ "$VERBOSE" = true ] ; then
	    add_on="    $MZN_EXE --solver $solver -I$globals $file > $file.$solver.out 2> $file.$solver.err"
	fi
	error_msg="ERROR: $solver: $file"$add_on
	success_msg="OK: $solver: $file"$add_on
	if [ -s $file.$solver.err ] # if $file.$solver.err is not empty
	then 
	    # special case is Choco which always prints SLF4J error messages on stderr
            if [[ "$solver" = "fzn_choco" ]]; then 
                # check if there is a line that does not start with SLF4J
                error=false
		while read line
		do 
                    if [[ "$line" != SLF4J* ]]; then # line does not start with SLF4J
			error=true
		    fi		   
		done < $file.$solver.err
                if [ "$error" = false ] ; then
		    echo $success_msg
		    rm $file.$solver.err
		else 
		    echo $error_msg
		fi		    
            elif [[ "$solver" = "fzn_chuffed" ]]; then 
		error=false
		while read line
		do 
                    if [[ "$line" != %* ]]; then # line does not start with MiniZinc comment
			error=true
		    fi		   
		done < $file.$solver.err
		if  [ "$error" = false ] ; then
		    echo $success_msg
		    rm $file.$solver.err
		else 
		    echo $error_msg
		fi		    
	    else 
		echo $error_msg
	    fi
        # else: err file is empty
	else 
	    echo $success_msg
	    rm $file.$solver.err
	fi
    done
done