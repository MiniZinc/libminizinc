/* 
  Set the variable `s` to one of the following numbers before solving and after
  each restart to reflect the result of searching since the last restart:
	 - 1 (START) -> The solver has not yet been restarted
	 - 2 (UNKNOWN) -> No solution has been found since the last restart
	 - 3 (UNSAT) -> The search space was (provably) exhausted and no solution was
     found since the last restart
	 - 4 (SAT) -> A solution was found since the last restart
	 - 5 (OPT) -> The (provably) optimal solution for the search space was found
     since the last restart
*/
predicate fzn_on_restart_status(var int: s) = abort("experimental on_restart feature `status' not supported by target solver");
