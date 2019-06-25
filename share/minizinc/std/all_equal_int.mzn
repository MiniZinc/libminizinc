include "fzn_all_equal_int.mzn";   
include "fzn_all_equal_int_reif.mzn";   

%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all equal.
%-----------------------------------------------------------------------------%

predicate all_equal_int(array[int] of var int: x) =
    fzn_all_equal_int(x);

predicate all_equal_int_reif(array[int] of var int: x, var bool: b) =
    fzn_all_equal_int_reif(x,b);
