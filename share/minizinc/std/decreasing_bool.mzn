include "fzn_decreasing_bool.mzn";
include "fzn_decreasing_bool_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in decreasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate decreasing_bool(array [$X] of var bool: x) = fzn_decreasing_bool(array1d(x));
