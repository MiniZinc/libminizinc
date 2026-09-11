% `set_of_int_lt_imp` for a solver with no set variables.
%
% The chain is `std/nosets.mzn`'s, exactly as the reified form beside this file
% builds it; only the last step differs — `b -> chain[l]` rather than
% `b <-> chain[l]`, which is one clause instead of two.

include "fzn_bool_clause.mzn";
include "fznso_set_to_bools.mzn";

predicate fzn_set_of_int_lt_imp(var set of int: s, var set of int: t, var bool: b) =
  if ub(s) = {} then
    % An empty `s` is strictly below every *non-empty* `t`. Stated in plain
    % MiniZinc, exactly as the reified form beside this file states it: naming
    % `fzn_int_lin_le_imp` here would need a declaration that no overlay file
    % provides, and the only library that has one is a particular solver's.
    b -> card(t) >= 1
  elseif ub(t) = {} then
    fzn_bool_clause([], [b]) % Nothing is strictly below the empty set.
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
      constraint chain[u] = (not xb[u] /\ yb[u]);
    } in fzn_bool_clause([chain[l]], [b])
  endif;
