include "fzn_all_equal_set.mzn";   
include "fzn_all_equal_set_reif.mzn";   

%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all equal.
%-----------------------------------------------------------------------------%

predicate all_equal_set(array[int] of var set of int: x) =
  if length(x) <=1 then true else fzn_all_equal_set(x) endif;

predicate all_equal_set_reif(array[int] of var set of int: x, var bool: b) =
  if length(x) <= 1 then b=true else fzn_all_equal_set_reif(x,b) endif;
