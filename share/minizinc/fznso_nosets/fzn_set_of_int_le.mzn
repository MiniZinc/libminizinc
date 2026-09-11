% `set_of_int_le` for a solver with no set variables: the set becomes the array
% of Booleans saying which members it holds, and the constraint is stated over
% that. Shadows the abort in `fznso_constraints/`.

include "fznso_set_to_bools.mzn";

predicate fzn_set_of_int_le(var set of int: s, var set of int: t) =
  if ub(s) = {} then
    true
  elseif ub(t) = {} then
    s = {}
  else
    let {
      set of int: U = ub(s) union ub(t);
      int: l = min(U);
      int: u = max(U);
      array [l..u] of var bool: xb = array1d(l..u, [i in s | i in l..u]);
      var l - 1..u: xmax = max(s union {l - 1});
      array [l..u] of var bool: yb = array1d(l..u, [i in t | i in l..u]);
      var l - 1..u: ymax = max(t union {l - 1});
      array [l..u] of var bool: chain;
      constraint
        forall (i in l..u - 1) (
          chain[i] =
            (
              let {
                var 1..4: cmp = 2 * xb[i] + yb[i] + 1;
              } in [chain[i + 1], xmax < i, ymax > i, chain[i + 1]][cmp]
            )
        );
      constraint chain[u] = (xb[u] -> yb[u]);
    } in chain[l]
  endif;
