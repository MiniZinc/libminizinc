/* Set `out` to be a the last solution value of `input` on each restart */
predicate fzn_on_restart_sol_bool(var bool: input, var bool: out) = abort("experimental on_restart feature `sol bool' not supported by target solver");
