predicate fzn_diffn_nonstrict(array[int] of var int: x,
                          array[int] of var int: y,
                          array[int] of var int: dx,
                          array[int] of var int: dy) =
    forall(i,j in index_set(x) where i < j)(
        dx[i] = 0 \/ dx[j] = 0 \/ dy[i]=0 \/ dy[j]=0 \/
        x[i] + dx[i] <= x[j] \/ y[i] + dy[i] <= y[j] \/
        x[j] + dx[j] <= x[i] \/ y[j] + dy[j] <= y[i]
    );
