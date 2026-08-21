% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_symmetric_all_different`; the registry names it for the
% type it takes and puts that first.
%
% The values in `xs` index `xs` itself, so the numbering they index from travels
% next to the array as a plain `int` — a FlatZinc array is one-based and cannot
% carry one.

include "fzn_int_all_different_symmetric.mzn";
include "all_different.mzn";
include "inverse.mzn";

predicate fzn_symmetric_all_different(array [int] of var int: xs) =
  fzn_int_all_different_symmetric(array1d(xs), min(index_set(xs)));
