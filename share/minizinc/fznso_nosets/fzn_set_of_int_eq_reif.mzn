% `set_of_int_eq_reif` for a solver with no set variables: the set becomes the
% array of Booleans saying which members it holds, and the constraint is stated
% over that. Shadows the abort in `fznso_constraints/`.

include "fznso_set_to_bools.mzn";

predicate fzn_set_of_int_eq_reif(var set of int: s, var set of int: t, var bool: b) :: promise_total =
  if is_fixed(b) then
    if true == fix(b) then s == t else s != t endif
  else
    let {
      array [int] of var bool: bx = set2bools(s);
      array [int] of var bool: by = set2bools(t);
    } in b <->
      forall (i in index_set(bx) union index_set(by)) (
        if not (i in index_set(bx)) then
          not by[i]
        elseif not (i in index_set(by)) then
          not bx[i]
        else
          bx[i] = by[i]
        endif
      )
  endif;
