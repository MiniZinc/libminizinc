include "fzn_increasing_bool.mzn";
include "fzn_increasing_bool_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in increasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate increasing_bool(array [$X] of var bool: x) = fzn_increasing_bool(array1d(x));
