include "fzn_decreasing_set.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in decreasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate decreasing_set(array[int] of var set of int: x) =
    fzn_decreasing_set(x);

