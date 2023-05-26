include "experimental/on_restart.mzn";

/* Set `out` to be a the last assigned value of `input` on each restart */
predicate fzn_on_restart_last_val_set(var set of int: input, var set of int: out) = 
	out = { m | m in ub(input) where last_val(m in input)}
