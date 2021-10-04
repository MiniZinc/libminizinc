%-----------------------------------------------------------------------------%
% Requires that 'y' is the ith element of the array 'x'.
%-----------------------------------------------------------------------------%

predicate element_int(var $$E: i, array[$$E] of var $$T: x, var $$T: y) =
    y = x[i];
