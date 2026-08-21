% The constraint `int_regular_cost_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_regular_cost_reif` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

% `std` aborts on this, and nothing here can do better without a propagator: the
% augmented-automaton trick that reifies `int_regular` needs a *deterministic*
% transition to append the marker to. Declaring the name is still worth it — a
% solver with the reified propagator can now offer one, and its body is dropped
% before this is ever reached.

predicate fzn_int_regular_cost_reif(
  list of var int: xs,
  int: states,
  int: alphabet,
  int: alphabet_offset,
  list of int: transition,
  int: initial,
  set of int: accepting,
  list of int: weight,
  var int: total_cost,
  var bool: b,
) =
  abort("Reified cost_regular is not supported.");
