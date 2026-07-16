include "fzn_cumulatives_decomp.mzn";

% min_machine is the first machine (which is the minimum, of the index set of bound).
% For native implementations, you can subtract (min_machine - 1) from the machine array
% values to obtain 1-based machine numbers.
predicate fzn_cumulatives(
  array [int] of var int: start,
  array [int] of var int: duration,
  array [int] of var int: resource,
  array [int] of var int: machine,
  array [int] of var int: bound,
  bool: is_upper_bound,
  int: min_machine,
) = fzn_cumulatives_decomp(start, duration, resource, machine, bound, is_upper_bound);
