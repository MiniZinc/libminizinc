% `set_of_int_eq_imp` for a solver with no set variables: the set becomes the
% array of Booleans saying which members it holds, and the implication is stated
% over that, bit by bit.
%
% Without this the flattener has nowhere to send `b -> s = t` but the full
% reification, which costs a Boolean per bit and the converse direction nobody
% asked for. `std/nosets.mzn:100` has the same predicate for the same reason;
% the FZnSO registry had no half-reified set constraint at all.
%
% Every body names an `fzn_` predicate explicitly. Bodies here are not dropped
% by `enable_native_predicates`, so writing `b -> bx[i] = by[i]` would lower
% through the rewrite layer and could return to this predicate.

include "fznso_set_to_bools.mzn";
include "fzn_bool_clause.mzn";
include "fzn_set_of_int_eq.mzn";

predicate fzn_set_of_int_eq_imp(
  var set of int: s,
  var set of int: t,
  var bool: b,
) =
  if is_fixed(b) then
    if fix(b) then fzn_set_of_int_eq(s, t) else true endif
  else
    let {
      array [int] of var bool: bx = set2bools(s);
      array [int] of var bool: by = set2bools(t);
    } in forall (i in index_set(bx) union index_set(by)) (
      if not (i in index_set(bx)) then
        % `s` cannot hold `i`, so `t` must not either.
        fzn_bool_clause([], [b, by[i]])
      elseif not (i in index_set(by)) then
        fzn_bool_clause([], [b, bx[i]])
      else
        % `b -> (bx[i] <-> by[i])`, as the two clauses an equivalence is.
        fzn_bool_clause([by[i]], [b, bx[i]]) /\ fzn_bool_clause([bx[i]], [b, by[i]])
      endif
    )
  endif;
