/* Abandon and mark search as complete when argument variable is assigned `true'. */
predicate fzn_on_restart_complete(var bool: marker) = abort("experimental on_restart feature `complete' not supported by target solver");
