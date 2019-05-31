include "fzn_increasing_bool.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in increasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate increasing_bool(array[int] of var bool: x) =
    fzn_increasing_bool(x);
