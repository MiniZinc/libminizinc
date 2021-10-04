%-----------------------------------------------------------------------------%
% Requires that 'y' is the ith element of the array 'x'.
%-----------------------------------------------------------------------------%

predicate element(var $$E: i, array[$$E] of var bool: x, var bool: y) =
	y = x[i];

predicate element(var $$E: i, array[$$E] of var float: x, var float: y) =
	y = x[i];

predicate element(var $$E: i, array[$$E] of var $$T: x, var $$T: y) =
	y = x[i];

predicate element(
	var $$E: i,
	array[$$E] of var set of $$T: x,
	var set of $$T: y
) =
	y = x[i];
