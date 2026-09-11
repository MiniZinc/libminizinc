% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` aborts on this, and nothing here can do better without a propagator: the
% augmented-automaton trick that reifies `int_regular` needs a *deterministic*
% transition to append the marker to. Declaring the name is still worth it — a
% solver with the reified propagator can now offer one, and its body is dropped
% before this is ever reached.

include "fzn_int_regular_cost_reif.mzn";

predicate fzn_cost_regular_reif(
  array [int] of var int: xs,
  int: states,
  int: alphabet,
  array [int, int] of int: transition,
  int: initial,
  set of int: accepting,
  array [int, int] of int: weight,
  var int: total_cost,
  var bool: b,
) =
  fzn_int_regular_cost_reif(
    array1d(xs),
    states,
    alphabet,
    1,
    array1d(transition),
    initial,
    accepting,
    array1d(weight),
    total_cost,
    b,
  );
