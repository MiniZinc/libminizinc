include "fzn_decreasing_int.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in decreasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate decreasing_int(array[int] of var int: x) =
    fzn_decreasing_int(x); 

