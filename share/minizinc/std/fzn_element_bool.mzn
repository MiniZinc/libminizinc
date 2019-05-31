%-----------------------------------------------------------------------------%
% Requires that 'y' is the ith element of the array 'x'.
%-----------------------------------------------------------------------------%

predicate fzn_element_bool(var int: i, array[int] of var bool: x, var bool: y) =
    y = x[i];
