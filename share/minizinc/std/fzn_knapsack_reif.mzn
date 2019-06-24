predicate fzn_knapsack_reif(array[int] of int: w, array[int] of int:p, 
                            array[int] of var int:x, var int: W, var int: P, var bool: b) = 
    b <-> (
      forall (i in index_set(x)) (x[i] >= 0) /\
      W >= 0 /\ P >= 0 /\
      P = sum(i in index_set(p)) (x[i]*p[i]) /\
      W = sum(i in index_set(w)) (x[i]*w[i])
    );
