/* Set `out` to be a the last solution value of `input` on each restart */
predicate fzn_on_restart_sol_int(var int: input, var int: out) = abort("experimental on_restart feature `sol int' not supported by target solver");
