% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_sliding_among_int`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_int_sliding_among.mzn";
include "sliding_sum.mzn";

predicate fzn_sliding_among_int(
  int: lower_bound,
  int: upper_bound,
  int: window,
  array [int] of var int: xs,
  set of int: values,
) =
  fzn_int_sliding_among(array1d(xs), values, window, lower_bound, upper_bound);
