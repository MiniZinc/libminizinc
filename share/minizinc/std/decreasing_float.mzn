include "fzn_decreasing_float.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in decreasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate decreasing_float(array[int] of var float: x) =
   fzn_decreasing_float(x);
