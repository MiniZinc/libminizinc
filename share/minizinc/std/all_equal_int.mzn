include "fzn_all_equal_int.mzn";   
include "fzn_all_equal_int_reif.mzn";   

%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all equal.
%-----------------------------------------------------------------------------%

predicate all_equal_int(array[int] of var int: x) =
  if length(x) <= 1 then true else fzn_all_equal_int(x) endif;

predicate all_equal_int_reif(array[int] of var int: x, var bool: b) =
  if length(x) <= 1 then b=true else fzn_all_equal_int_reif(x,b) endif;
