% `set_of_int_subset_imp` for a solver with no set variables. See
% `fzn_set_of_int_eq_imp.mzn`: a half-reified set constraint built from the full
% reification costs a Boolean per bit and the converse direction nobody asked
% for, and `b -> (bx[i] -> by[i])` is one clause with `b` negated into it.

include "fznso_set_to_bools.mzn";
include "fzn_bool_clause.mzn";
include "fzn_set_of_int_subset.mzn";

predicate fzn_set_of_int_subset_imp(
  var set of int: s,
  var set of int: t,
  var bool: b,
) =
  if is_fixed(b) then
    if fix(b) then fzn_set_of_int_subset(s, t) else true endif
  else
    let {
      array [int] of var bool: bx = set2bools(s);
      array [int] of var bool: by = set2bools(t);
    } in forall (i in index_set(bx)) (
      if not (i in index_set(by)) then
        fzn_bool_clause([], [b, bx[i]])
      else
        fzn_bool_clause([by[i]], [b, bx[i]])
      endif
    )
  endif;
