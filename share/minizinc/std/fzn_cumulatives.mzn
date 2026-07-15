include "fzn_cumulatives_decomp.mzn";

% min_machine is the first machine (which is the minimum, of the index set of bounds).
% For native implementations, you can subtract (min_machine - 1) from the machines array
% values to obtain 1-based machine numbers.
predicate fzn_cumulatives(
  array [int] of var int: starts,
  array [int] of var int: durations,
  array [int] of var int: resources,
  array [int] of var int: machines,
  array [int] of var int: bounds,
  bool: is_upper_bound,
  int: min_machine,
) = fzn_cumulatives_decomp(starts, durations, resources, machines, bounds, is_upper_bound);
