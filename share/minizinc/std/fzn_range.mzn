/** @group globals
  Requires that the image of function \a x (represented as an array) on set
  of values \a s is \a t.  ub(\a s) must be a subset of index_set(\a x) otherwise an
  assertion failure will occur.
*/
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
