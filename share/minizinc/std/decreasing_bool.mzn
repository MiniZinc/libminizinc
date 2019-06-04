include "fzn_decreasing_bool.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in decreasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate decreasing_bool(array[int] of var bool: x) =
    fzn_decreasing_bool(x);
