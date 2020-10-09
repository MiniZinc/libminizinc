# MiniZinc benchmarking automation

Allows checking of MiniZinc solutions by configurable checker profiles. The
solutions can be input from previous logs or produced by a chosen solver
profile, for 1 or several instances, with result comparison and some    
ranking. New solutions' summary logs, detailed stdout/err outputs, and      
statistics are saved in subfolder mzn-test/LOGS, /OUTPUTS, and /STATS, resp.

## USAGE EXAMPLES

(1) "mzn-test.py model.mzn data.dzn [--checkDZN stdout.txt
[--checkStderr stderr.txt]] [--chkPrf MINIZINC-CHK --chkPrf FZN-GECODE-CHK]
[--tCheck 15] [--addSolverOption "--fzn-flags '-D fPureCircuit=true'"]" :::
check the instance's solutions, optionally reading them from a DZN-formatted
file (otherwise solving first), optionally overriding default checker list
etc.

(2) "mzn-test.py --slvPrf MZN-CPLEX -t 300 -l instList1.txt -l
instList2.txt --name CPLEXTest_003 --result newLog00.json prevLog1.json
prevLog2.json --failed failLog.json" ::: solve instances using the specified
solver profile and wall time limit 300 seconds. The instances are taken from
the list files. The test is aliased CPLEXTest_003. Results are saved to
newLog00.json and compared/ranked to those in prevLog's. (Probably) incorrect
solutions are saved to failLog.json. 

(3) "mzn-test.py [-l instList1.txt] -c
prevLog1.json -c prevLog2.json [--runAndCmp]" ::: compare existing logs,
optionally limited to the given instances, optionally running new tests. USE
SINGLE QUOTES ONLY INSIDE ARGUMENTS PASSED TO THE BACKENDS when running
backends through shell.

