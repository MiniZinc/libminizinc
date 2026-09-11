% `int_in` for a solver with no set variables: the set becomes the array of
% Booleans saying which members it holds, and the constraint is stated over
% that. Shadows the abort in `fznso_constraints/`.

include "fznso_set_to_bools.mzn";

predicate fzn_int_in(int: x, var set of int: values) :: promise_total =
  let { array [int] of var bool: by = set2bools(values) } in by[x];

predicate fzn_int_in(var int: x, var set of int: values) :: promise_total =
  if ub(values) = {} then
    false
  else
    % The bounds are conjuncts rather than `let` constraints: this predicate is
    % `promise_total`, so a constraint in its `let` would be posted at the root
    % even where the membership itself is only implied — and then a model that
    % merely *may* need `x` in the set would be held to its bounds regardless.
    %
    % As conjuncts they still come first, which is what matters in a root
    % context: they narrow `x` before the array access below reads it. Without
    % that the access is partial for an `x` whose domain is wider than the set
    % — including one the model never bounded at all — and MiniZinc states the
    % partiality as one reified equality per value, which a solver needing
    % finite bounds has nothing to work with.
    x >= min(ub(values)) /\
      x <= max(ub(values)) /\
      (
        let {
          set of int: extent = ub(values);
        } in if is_fixed(values) /\ extent = min(extent)..max(extent) then
          % A fixed contiguous set is exactly the two bounds above, so the array
          % of Booleans has nothing left to say. Without this the membership
          % becomes an element lookup over `card(extent)` positions, and a
          % decomposing solver pays for every one of them.
          true
        else
          let {
            array [int] of var bool: by = set2bools(values);
          } in by[x]
        endif
      )
  endif;
