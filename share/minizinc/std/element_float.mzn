%-----------------------------------------------------------------------------%
% Requires that 'y' is the ith element of the array 'x'.
%-----------------------------------------------------------------------------%

predicate element_float(var $$E: i, array[$$E] of var float: x, var float: y) =
    y = x[i];
