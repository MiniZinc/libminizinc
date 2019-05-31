/** @group globals
  Requires that \a x[\p i] in \a t for all \p i in \a s
*/
predicate fzn_roots(array[int] of var int: x, var set of int: s,
                                          var set of int: t) =
        % All values in 's' must map to a value in 't'.
        forall(i in ub(s)) (
            i in s -> x[i] in t
        ) /\
        forall(i in ub(t)) (
            i in t -> forall(j in index_set(x)) (x[j] = i -> j in s )
        );
