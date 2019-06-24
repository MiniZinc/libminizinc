predicate fzn_span(var opt int: s0, var int: d0,
                   array[int] of var opt int: s,
                   array[int] of var int: d) =
    (occurs(s0) <-> exists(i in index_set(s))(occurs(s[i]))) /\ 
    s0 = min(s) /\
    (absent(s0) -> d0 = 0) /\
    s0 ~+ d0 = max([s[i] ~+ d[i] | i in index_set(s)]);
