%-----------------------------------------------------------------------------%
% Requires that 'y' is the ith element of the array 'x'.
%-----------------------------------------------------------------------------%

include "fzn_element_bool.mzn";
include "fzn_element_float.mzn";
include "fzn_element_int.mzn";
include "fzn_element_set.mzn";

predicate element(var int: i, array[int] of var bool: x, var bool: y) =
    fzn_element_bool(i, x, y);

predicate element(var int: i, array[int] of var float: x, var float: y) =
    fzn_element_float(i, x, y);

predicate element(var int: i, array[int] of var int: x, var int: y) =
    fzn_element_int(i, x, y);

predicate element(var int: i, array[int] of var set of int: x,
        var set of int: y) =
    fzn_element_set(i, x, y);
