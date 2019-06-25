predicate fzn_range(array[int] of var int: x, var set of int: s,
                                          var set of int: t) =
        % All values in 's' must map to a value in 't'.
        forall(i in ub(s)) (
            i in s -> x[i] in t
        ) /\
        % All values in 't' must be mapped from a value in 's'.
        forall(i in ub(t)) (
            i in t -> exists(j in ub(s)) ( j in s /\ x[j] == i )
        );
