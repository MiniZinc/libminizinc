include "fzn_increasing_int.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in increasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate increasing_int(array[int] of var int: x) =
    fzn_increasing_int(x);
