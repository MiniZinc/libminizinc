% `set_of_int_eq` for a solver with no set variables: the set becomes the array
% of Booleans saying which members it holds, and the constraint is stated over
% that. Shadows the abort in `fznso_constraints/`.

include "fznso_set_to_bools.mzn";

predicate fzn_set_of_int_eq(var set of int: s, var set of int: t) :: promise_total =
  if not has_ub_set(s) /\ not has_ub_set(t) then
    assert(false, "fzn_set_of_int_eq: cannot determine bounds of set variables")
  elseif not has_ub_set(s) then
    fzn_set_of_int_eq(t, s)
  else
    % `std/nosets.mzn` posts `s subset t` and `t subset s` here first, to narrow
    % the two declared domains before the bits are taken. That is worth a row to
    % a solver holding the sets themselves; here the bitwise equality below says
    % both of them already, so the pair is pure duplication — a clause per
    % member, and a three-literal one whenever either side is an expression
    % rather than a plain set.
    let {
      array [int] of var bool: bx = set2bools(s);
      array [int] of var bool: by = set2bools(t);
    } in forall (
      i in index_set(bx) union index_set(by),
    ) (
      if not (i in index_set(bx)) then
        not by[i] %% Should be impossible
      elseif not (i in index_set(by)) then
        not bx[i]
      else
        bx[i] = by[i]
      endif
    )
  endif;
