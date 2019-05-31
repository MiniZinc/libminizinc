include "fzn_element_int.mzn";

%-----------------------------------------------------------------------------%
% Requires that 'y' is the ith element of the array 'x'.
%-----------------------------------------------------------------------------%

predicate element_int(var int: i, array[int] of var int: x, var int: y) =
    element_int(i, x, y);
