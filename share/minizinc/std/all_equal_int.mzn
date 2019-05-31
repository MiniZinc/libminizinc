include "fzn_all_equal_int.mzn";   

%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all equal.
%-----------------------------------------------------------------------------%

predicate all_equal_int(array[int] of var int: x) =
    fzn_all_equal_int(x);