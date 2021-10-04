include "fzn_all_equal_int.mzn";   
include "fzn_all_equal_int_reif.mzn";   

%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all equal.
%-----------------------------------------------------------------------------%

predicate all_equal_int(array[$X] of var int: x) =
  if length(x) <= 1 then true else fzn_all_equal_int(array1d(x)) endif;
