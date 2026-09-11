% `int_in_reif` for a solver with no set variables: the set becomes the array of
% Booleans saying which members it holds, and the constraint is stated over
% that. Shadows the abort in `fznso_constraints/`.

include "fznso_set_to_bools.mzn";

predicate fzn_int_in_reif(int: x, var set of int: values, var bool: b) :: promise_total =
  if x in ub(values) then b <-> set2bools(values)[x] else not b endif;

predicate fzn_int_in_reif(var int: x, var set of int: values, var bool: b) :: promise_total =
  if ub(values) = {} then
    not b
  else
    let {
      int: lo = min(ub(values));
      int: hi = max(ub(values));
      array [int] of var bool: by = set2bools(values);
      % A copy of `x` pinned inside the array's index set, so that the access
      % below is *total*.
      %
      % `std/nosets.mzn` writes this as `b <-> set2bools(values)[x]`, which is
      % a partial access whenever `x` ranges wider than the set — and a partial
      % access inside a reification is the trap. MiniZinc totalises it into one
      % reified equality per value of `x`, each of which the overlay rewrites
      % onto `int_lin_eq_reif`; for a solver that implements *that* natively the
      % rewriting does not terminate, and the flattener simply never returns.
      % `-Glinear` never sees it because it decomposes `int_lin_eq_reif` too.
      %
      % `fzn_int_in.mzn` beside this file avoids the same partiality by stating
      % the bounds as leading conjuncts, which narrow `x` before the access
      % reads it. That works only in a root context: nothing narrows `x` when
      % the membership is merely reified, so the index is pinned here instead.
      % Where `x` is out of range the pinning says nothing and the range test
      % below is false anyway, which is what makes leaving `idx` free sound.
      var lo..hi: idx;
      constraint (lo <= x /\ x <= hi) -> idx = x;
    } in b <-> (lo <= x /\ x <= hi /\ by[idx])
  endif;
