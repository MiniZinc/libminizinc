include "fzn_strictly_increasing_int_opt.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in strict decreasing order
%-----------------------------------------------------------------------------%

predicate fzn_strictly_decreasing_int_opt(array[int] of var opt int: x) =
  fzn_strictly_increasing_int_opt(reverse(x));
