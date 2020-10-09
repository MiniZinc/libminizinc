predicate fzn_roots_reif(array[int] of var int: x, var set of int: s,
                         var set of int: t, var bool: b) =
    b <-> (
      % All values in 's' must map to a value in 't'.
      forall(i in ub(s)) (
          i in s -> x[i] in t
      ) /\
      forall(i in ub(t)) (
          i in t -> forall(j in index_set(x)) (x[j] = i -> j in s )
      )
    );
