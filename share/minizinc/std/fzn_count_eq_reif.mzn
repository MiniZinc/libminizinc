predicate fzn_count_eq_reif(array[int] of var int: x, var int: y, var int: c, var bool: b) =
    b <-> count_eq(x,y)=c;

%-----------------------------------------------------------------------------%
