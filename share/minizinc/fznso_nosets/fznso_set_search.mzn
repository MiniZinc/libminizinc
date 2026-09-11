% Search and warm-start annotations over set variables, for a solver with none.
%
% `std/nosets.mzn`'s own definitions. Without them a `set_search` keeps the set
% variable's declaration alive after every constraint on it has been decomposed,
% and it reaches a solver that has just said it has no such variable.
%
% `redefinitions.mzn` includes this because nothing else always would: an
% annotation has no FlatZinc builtin for the rewrite layer to hang a redefinition
% off. `fznso_constraints/` carries an empty file of this name for a solver that
% does have set variables.

include "fznso_set_to_bools.mzn";

% `indomain_min` on a set means the *smallest* set, so the bits it holds should
% be tried false first — the two are swapped on the way through.
function ann: set_search(array [int] of var set of $$E: x, ann: select, ann: choice, ann: explore) =
  let {
    ann: cc =
      if choice == indomain_min then
        indomain_max
      elseif choice == indomain_max then
        indomain_min
      else
        choice
      endif;
  } in seq_search([bool_search(set2bools(x[i]), select, cc, explore) | i in index_set(x)]);

function ann: warm_start(array [int] of var set of int: x, array [int] of set of int: v) =
  warm_start_array([
    let {
      array [int] of var bool: xb = set2bools(x[i]);
      set of int: is_var = ub(x[i]) diff lb(x[i]);
      int: iV = i - min(index_set(x)) + if 0 < length(v) then min(index_set(v)) else 0 endif;
    } in warm_start(
      [xb[k] | k in is_var],
      if iV in index_set(v) then [k in v[iV] | k in is_var] else [] endif,
    ) |
      i in index_set(x),
  ]);
