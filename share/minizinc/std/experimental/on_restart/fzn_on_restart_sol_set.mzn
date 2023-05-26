include "experimental/on_restart.mzn";

/* Set `out` to be a the last solution value of `input` on each restart */
predicate fzn_on_restart_sol_set(var set of int: input, var set of int: out) = 
	out = { m | m in ub(input) where sol(m in input)}
