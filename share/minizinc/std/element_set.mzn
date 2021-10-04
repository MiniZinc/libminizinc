%-----------------------------------------------------------------------------%
% Requires that 'y' is the ith element of the array 'x'.
%-----------------------------------------------------------------------------%

predicate element_set(
	var $$E: i,
	array[$$E] of var set of $$T: x,
	var set of $$T: y
) =
	y = x[i];
