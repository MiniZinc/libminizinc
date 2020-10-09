predicate fzn_diffn_reif(array[int] of var int: x,
                array[int] of var int: y,
                array[int] of var int: dx,
                array[int] of var int: dy,
                var bool: b) =
    b <-> forall(i,j in index_set(x) where i < j)(
        x[i] + dx[i] <= x[j] \/ y[i] + dy[i] <= y[j] \/
        x[j] + dx[j] <= x[i] \/ y[j] + dy[j] <= y[i]
    );

%-----------------------------------------------------------------------------%
