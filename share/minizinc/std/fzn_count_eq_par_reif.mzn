predicate fzn_count_eq_par_reif(array[int] of var int: x, int: y, int: c, var bool: b) =
    b <-> count_eq(x,y)=c;

%-----------------------------------------------------------------------------%
