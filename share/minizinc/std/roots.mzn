include "fzn_roots.mzn";

/** @group globals
  Requires that \a x[\p i] in \a t for all \p i in \a s
*/
predicate roots(array[int] of var int: x, var set of int: s,
                                          var set of int: t) =
    assert(ub(s) subset index_set(x),
        "roots: upper bound of 's' must be a subset of the index set of 'x'",
        fzn_roots(x,s,t)
    );
