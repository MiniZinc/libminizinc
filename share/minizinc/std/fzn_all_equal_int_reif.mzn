%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all equal.
%-----------------------------------------------------------------------------%

predicate fzn_all_equal_int_reif(array[int] of var int: x, var bool: b) =
	b <-> forall(i, j in index_set(x) where i < j) ( x[i] = x[j] );
